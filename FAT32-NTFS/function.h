#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <iomanip>
#include <variant>
using namespace std;


//--------------------------------------KHU VỰC HÀM CHUNG (CHO CẢ NTFS VÀ FAT32)------------------------------------------

/// <summary>
/// Hàm để đọc 1 sector
/// </summary>
/// <param name="drive">: tên ổ đĩa</param>
/// <param name="readPoint">: bắt đầu đọc từ byte thứ readPoint của ổ drive</param>
/// <param name="sector">: biến lưu nội dung sector</param>
/// <returns></returns>
int ReadSector(LPCWSTR  drive, unsigned long long readPoint, BYTE* sector, int length);

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


/// <summary>
/// Hàm nhập vào tên ổ đĩa và kiểm tra ổ đĩa có dùng hệ thống NTFS hay không. Nếu có, trả về true, nếu không (là FAT32), trả về false.
/// Đồng thời, hàm lưu lại sector đầu tiên của ổ đĩa vào tham số sector
/// </summary>
/// <param name="sector">: mảng động đã được cấp phát vùng nhớ 512 byte để lưu sector đầu của ổ đĩa</param>
/// <returns></returns>
bool IsNTFS(BYTE* sector);

void test();
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

/// <summary>
/// Hàm để đọc 1 entry (MFT entry).
/// </summary>
/// <param name="Start_Address_MFT">Vị trí offset của entry</param>
/// <returns></returns>
bool Read_Entry(unsigned long long Start_Address_MFT);

/// <summary>
/// Hàm để đọc MFT (MFT entry).
/// </summary>
/// <param></param>
/// <returns></returns>
void Read_MFT();

/// <summary>
/// Hàm để vẽ cây thư mục
/// </summary>
/// <param name="node">Gốc cây thư mục cần vẽ</param>
/// <param name="level">Cấp của node hiện tại</param>
/// <returns></returns>
void Folder_Structure_BFS(long long node, int level);



//--------------------------------------KHU VỰC HÀM CHO FAT32------------------------------------------