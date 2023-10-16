#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
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

//--------------------------------------KHU VỰC HÀM CHO NTFS------------------------------------------

/// <summary>
/// Hàm để tính kích thước của một bản ghi MFT (MFT entry). Đơn vị tính là byte.
/// </summary>
/// <param name="sector_VBR">: mảng chứa dữ liệu của sector chứa VBR (Volume Boot Record)</param>
/// <returns></returns>
int MFTEntry_Size(BYTE sector_VBR[]);


//--------------------------------------KHU VỰC HÀM CHO FAT32------------------------------------------