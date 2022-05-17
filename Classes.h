#pragma pack(push,1)

typedef struct
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

typedef struct
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

enum FS_type {NTFS=0, FAT32};

class FileSystemClass
{
  protected:
	UINT16  Sector_size;
	BYTE    Sectors_count;
	UINT16  Cluster_size;
	ULONGLONG Sectors_count_r;
	ULONGLONG Clusters_count_r;
	double fs_size;
  public:
    UINT16 GetClusterSize();
	ULONGLONG GetClustersCount();
	double GetFileSystemSize();
	FileSystemClass* CreateFS(FS_type, BYTE *); //фабричный метод
	BYTE* ReadCluster(HANDLE, BYTE *);
	void PrintClusterToFile(FILE *, BYTE *, ULONGLONG);
	virtual ULONGLONG GetFirstFileRecordCluster() = 0;
	BYTE* FindSignatureInCluster(BYTE *, BYTE *);
};


class NTFS_FileSystemClass : public FileSystemClass
{
  private:
	NTFS_BootRecord *pNTFS_BootRecord;
  protected:
  public:
	NTFS_FileSystemClass(BYTE *);
	ULONGLONG GetFirstFileRecordCluster();
};

class FAT32_FileSystemClass : public FileSystemClass
{
  private:
	FAT32_BootRecord *pFAT32_BootRecord;
  protected:
  public:
	FAT32_FileSystemClass(BYTE *);
	ULONGLONG GetFirstFileRecordCluster();
};
