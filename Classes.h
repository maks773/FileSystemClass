#include "sqlite3.h"
#pragma pack(push,1)

typedef struct                //структура Загрузочного сектора для NTFS
{
	BYTE        jmp[3];
	BYTE        name[8];
	UINT16      sector_size;
	BYTE        sectors_count;
	BYTE        reserv_1[7];
	BYTE        dev_type;
	BYTE        reserv_2[2];
	UINT16      sectors_on_track;
	UINT16      magnetic_heads_count;
	BYTE        reserv_3[8];
	BYTE        reserv_4[2];
	BYTE        reserv_5[2];
	ULONGLONG   sectors_count_r;
	ULONGLONG   mft_cluster_number;
	ULONGLONG   mft_copy_number;
	UINT32      mft_size;
	UINT32      bufer_size;
	BYTE        ser_num[8];
} NTFS_BootRecord;

typedef struct               //структура Загрузочного сектора для FAT32
{
	BYTE        jmp[3];
	BYTE        name[8];
	UINT16      sector_size;
	BYTE        sectors_count;
	UINT16      reserv_sectors;
	BYTE        count_fat_table;
	BYTE        reserv_1[2];
	UINT16      sectors_count_r1;
	BYTE        dev_type;
	BYTE        reserv_2[2];
	UINT16      sectors_on_track;
	UINT16      magnetic_heads_count;
	UINT32      hidden_sectors;
	UINT32		sectors_count_r2;
	UINT32		fat_size;
	BYTE        reserved[4];
	UINT32      root_dir_cluster_number;
} FAT32_BootRecord;

#pragma pack(pop)

enum FS_type {NTFS=0, FAT32};   //тип ФС для реализации фабричного метода

class FileSystemClass           //Абстрактный класс Файловой системы
{
  protected:
	UINT16  Sector_size;         //размер сектора
	BYTE    Sectors_count;       //кластерный множитель
	UINT16  Cluster_size;        //размер кластера
	ULONGLONG Sectors_count_r;   //общее количество секторов
	ULONGLONG Clusters_count_r;  //общее количество кластеров
	UINT16 First_cluster;        //номер первого кластера
	double fs_size;              //размер файловой системы (в Гб)
	std::string fs_type;         //тип Файловой системы
  public:
	ULONGLONG SkipSectors;           //количество секторов перед самым первым кластером ФС
	UINT16 GetClusterSize();         //получить рамзер кластера
	ULONGLONG GetClustersCount();    //получить общее количество кластеров
	double GetFileSystemSize();      //получить размер ФС (в Гб)
	UINT16 GetSectorSize();          //получить размер сектора
	UINT16 GetFirstClusterNumber();  //получить номер первого кластера ФС
	std::string GetFileSystemType(); //получить тип ФС
	FileSystemClass* CreateFS(FS_type, BYTE *); //фабричный метод
	BYTE* ReadCluster(HANDLE, BYTE *);          //чтение кластера ФС
	void PrintClusterToFile(FILE *, BYTE *, ULONGLONG);  //печать кластера в файл
	virtual ULONGLONG GetFirstFileRecordCluster() = 0;   //получить номер кластера, где начинаются файловые записи
	BYTE* FindSignatureInCluster(BYTE *, BYTE *, bool);  //поиск сигнатуры в кластере
};


class NTFS_FileSystemClass : public FileSystemClass     //класс ФС NTFS
{
  private:
	NTFS_BootRecord *pNTFS_BootRecord;
  protected:
  public:
	NTFS_FileSystemClass(BYTE *);
	ULONGLONG GetFirstFileRecordCluster();
};

class FAT32_FileSystemClass : public FileSystemClass    //класс ФС FAT32
{
  private:
	FAT32_BootRecord *pFAT32_BootRecord;
  protected:
  public:
	FAT32_FileSystemClass(BYTE *);
	ULONGLONG GetFirstFileRecordCluster();
};

template <class Item>                        //интерфейс итератора
class Iterator
{
   public:
	 virtual void First() = 0;              //индекс первого элемента
	 virtual void Next() = 0;               //переход к следующему элементу
	 virtual bool IsDone() = 0;             //проверка на конец списка
	 virtual Item CurrentItem() = 0;        //возврат текущего значения
};

class ClustersIterator : public Iterator<BYTE*>   //итератор для обхода кластеров
{
   public:
	 ClustersIterator(FileSystemClass* _FS);
	 ~ClustersIterator();
	 void First();                          //возврат номера первого кластера
	 void Next();                           //переход к следующему кластеру
	 bool IsDone();                         //достигнут ли конец ФС
	 BYTE* CurrentItem();                   //возврат текущего кластера
	 ULONGLONG GetCurrentNum();             //возврат номера текущего кластера
  protected:
	 ULONGLONG current;                     //номер кластера
	 BYTE *cluster;                         //буфер для чтения в него кластера
	 FileSystemClass *FS;                   //укаатель класса ФС
	 HANDLE hDisk;                          //указатель на файл ФС
	 std::string fs_type;                   //тип ФС
};

class ClustersDecorator: public ClustersIterator    //декоратор для итератора
{                                                   //обходит непустые кластеры
  public:
	 ClustersDecorator(ClustersIterator *_It);
	 BYTE* CurrentItem();                           //возврат непустого кластера
};



//Объявления внеклассовых функций

std::string GetImageExt(int);      //определяет расширение изображения

std::string FindImage(BYTE *, FileSystemClass *);   //ищет сигнатуры изображений в кластере

std::string GetAudioExt(int);      //определяет расширение аудио

std::string FindAudio(BYTE *, FileSystemClass *);   //ищет сигнатуры аудиофайла в кластере

std::string GetVideoExt(int);      //определяет расширение видео

std::string FindVideo(BYTE *, FileSystemClass *);   //ищет сигнатуры видеофайла в кластере

std::string GetDocExt(int);        //определяет расширение документа

std::string FindDoc(BYTE *, FileSystemClass *);     //ищет сигнатуры документа в кластере

void DeleteAll(TVirtualStringTree *, sqlite3 *);    //удаление всех записей в БД и VirtualStringTree

void DeleteEntry(TVirtualStringTree *, sqlite3 *);  //удаление одной записи в БД и VirtualStringTree

FileSystemClass* IdentifyFS();  //определение типа ФС из загрузочного сектора

void WriteToDB(sqlite3 *, ULONGLONG, std::string);  //вывод данных в БД

void PrintError(int);           //вывод сообщения об ошибке на экран
