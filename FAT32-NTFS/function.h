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
/// Hàm để chuyển 1 chuỗi byte hexa (lưu ở little endian) sang decimal
/// </summary>
/// <param name="byteArr">: mảng chuỗi hexa (lưu ở little endian)</param>
/// <param name="length">: độ dài chuỗi</param>
/// <returns></returns>
unsigned long long LittleEndian_HexaToDecimal(BYTE byteArr[], int length);

string HexaToBinary(BYTE hexa);
int BinaryToDecimal(string binary);


/// <summary>
/// Hàm chuyển chuỗi BYTE thành chuỗi string (theo ASCII)
/// </summary>
/// <param name="byteArr">: chuỗi byte hexa</param>
/// <param name="length">: độ dài chuỗi (số byte)</param>
/// <returns></returns>
string ByteArrToString(BYTE byteArr[], int length);


//--------------------------------------KHU VỰC HÀM CHO NTFS------------------------------------------

/// <summary>
/// Hàm để tính kích thước của một bản ghi MFT (MFT entry). Đơn vị tính là byte.
/// </summary>
/// <param name="byte_40h_BPB">: giá trị hexa của 1 byte nằm ở vị trí 40h của bảng BPB.</param>
/// <returns></returns>
int MFTEntry_Size(BYTE byte_40h_BPB);

//--------------------------------------KHU VỰC HÀM CHO FAT32------------------------------------------