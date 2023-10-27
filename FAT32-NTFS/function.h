#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

//--------------------------------------KHU VỰC HÀM CHUNG (CHO CẢ NTFS VÀ FAT32)------------------------------------------

/// <summary>
/// Hàm để đọc 1 sector
/// </summary>
/// <param name="drive">: tên ổ đĩa</param>
/// <param name="readPoint">: bắt đầu đọc từ byte thứ readPoint của ổ drive</param>
/// <param name="sector">: biến lưu nội dung sector</param>
/// <returns></returns>
int ReadSector(LPCWSTR  drive, unsigned long long readPoint, BYTE sector[512]);

/// <summary>
/// Hàm để chuyển 1 mảng con hexa (lưu ở little endian) sang decimal
/// </summary>
/// <param name="sector">: mảng chứa dữ liệu của sector đã đọc (các byte hexa lưu ở little endian)</param>
/// <param name="startIndex">: vị trí bắt đầu của mảng con</param>
/// <param name="length">: độ dài mảng con muốn chuyển</param>
/// <returns></returns>
unsigned long long LittleEndian_HexaToDecimal(BYTE sector[], int startIndex, int length);


string HexaToBinary(BYTE hexa);
int BinaryToDecimal(string binary);


/// <summary>
/// Hàm chuyển 1 mảng con hexa thành chuỗi string (theo ASCII)
/// </summary>
/// <param name="sector">: mảng chứa dữ liệu của sector đã đọc (các byte hexa lưu ở little endian)</param>
/// <param name="startIndex">: vị trí bắt đầu của mảng con</param>
/// <param name="length">: độ dài mảng con muốn chuyển</param>
/// <returns></returns>
string ByteArrToString(BYTE sector[], int startIndex, int length);

/// <summary>
/// Hàm in ra màn hình 1 mảng con dưới dạng hexa trong mảng chứa dữ liệu của sector (có xuống dòng sau khi in)
/// </summary>
/// <param name="sector">: mảng chứa dữ liệu của sector đã đọc (các byte hexa lưu ở little endian)</param>
/// <param name="startIndex">: vị trí bắt đầu của mảng con</param>
/// <param name="length">: độ dài mảng con muốn in ra màn hình (số byte)</param>
void PrintHexa(BYTE sector[], int startIndex, int length);

/// <summary>
/// Hàm chuyển 1 mảng con hexa thành chuỗi string (theo Unicode UTF-16)
/// </summary>
/// <param name="sector": mảng chứa dữ liệu của sector đã đọc (các byte hexa lưu ở little endian)></param>
/// <param name="startIndex">: vị trí bắt đầu của mảng con</param>
/// <param name="length">: độ dài mảng con muốn chuyển</param>
/// <returns></returns>
string HexaToUnicodeUTF16(BYTE sector[], int startIndex, int length);

//--------------------------------------KHU VỰC HÀM CHO NTFS------------------------------------------

/// <summary>
/// Hàm để tính kích thước của một bản ghi MFT (MFT entry). Đơn vị tính là byte.
/// </summary>
/// <param name="sector_VBR">: mảng chứa dữ liệu của sector chứa VBR (Volume Boot Record)</param>
/// <returns></returns>
int MFTEntry_Size(BYTE sector_VBR[]);

/// <summary>
/// Hàm đọc các thông tin được mô tả trong Partition Boot Sector (chỉ đối với NTFS)
/// </summary>
/// <param name="sector">mảng chứa dữ liệu của Partition Boot Sector (các byte hexa lưu ở little endian)</param>
void Read_VBR(BYTE sector[]);

//--------------------------------------KHU VỰC HÀM CHO FAT32------------------------------------------

/// <summary>
/// Struct Bootsector chứa các thông số quan trọng, ở đây chỉ lấy 8 thuộc tính chứa các thông số cần thiết 
/// </summary>
struct BootSector_FAT32
{
	int byte_per_sector; //Số byte của 1 sector					(offset B - chiếm 2 bytes)
	int Sector_per_Cluster; //Số Sector của 1 Cluster	[Sc]	(offset D - chiếm 1 byte)
	int Reserved_Sector; // Số sector của BootSector	[SB]	(offset E - chiếm 2 bytes)
	int No_FAT; // Số bảng FAT							[NF]	(offset 10 - 1 byte)
	int RDET_Entries; //Số Entrys của RDET, thường = 0	[SRDET]	(offset 11 - 2 bytes)
	long Total_Sector;//Tổng số sector (kích thức Volume)[Sv]	(offset 20 - 4 bytes)
	int Sector_per_FAT;//Số sector mỗi bảng FAT			[Sf]	(offset 24 - 4 bytes)
	int Root; //Cluster bắt đầu của RDET						(offset 2C - 4 bytes) 
};

/// <summary>
/// Hàm để đọc một BootSector và lưu vào struct FAT32
/// </summary>
/// <param name = bootSector_ptr>: con trỏ chứa địa chỉ bootSector</param>
/// /// <returns = results> Kết quả đọc được từ BootSector_ptr</returns>
BootSector_FAT32 read_BootSector(BYTE* bootSector_ptr);


/// <summary>
/// Hàm in các thông số đáng chú ý của bootSector vừa đọc được từ hàm read_BootSector
/// </summary>
/// <param name="FAT32">Chứa thông tin của bootsector</param>
void Print_BootSector(BootSector_FAT32 FAT32);

/// <summary>
/// Hàm đọc 2 Bảng FAT
/// </summary>
/// <param name="drive">Tên Volume</param>
/// <param name="fat32">Các thông số của bootsector đã đọc</param>
/// <param name="FAT_table_result">Biến con trỏ lưu mảng kết quả của hàm này</param>
void read_FAT_table(LPCWSTR driver, BootSector_FAT32 fat32, int* FAT_table_result);
//void read_FAT_table(LPCWSTR driver, BootSector_FAT32 fat32, BYTE* FAT_table_result);


//void initFAT(int*& FAT, BootSector_FAT32 fat32, LPCWSTR drive1);

// Cau truc DIRECTORY
struct MAIN_ENTRY {
	string name;             // Tên thư mục/tập tin
	string extensionName;             // Tên thư mục/tập tin
	string attribute;              // Thuộc tính 
	int startCluster;           // Cluster bắt đầu
	int fileSize;               // Kích cỡ (tính theo byte)
	//DIRECTORY* next;            // Trỏ đến thư mục/ tập tin tiếp theo
	//DIRECTORY* dir;             // Trỏ đến thư mục con
};

struct DUMMY {

};
int firstSectorIndex_Cluster(int clusIndex, BootSector_FAT32 fat32);

void readDirectory(int firstRecordIndex, int clusIndex, int* FatTable, BootSector_FAT32 fat32, LPCWSTR driver, int level);
void printTree(MAIN_ENTRY entry, int level);
void readContentOfFile(BootSector_FAT32 fat32, int clusIndex, LPCWSTR drive1, string space);
//void freeDirEntries(DIRECTORY* dir);