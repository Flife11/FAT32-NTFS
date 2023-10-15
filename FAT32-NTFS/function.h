#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
using namespace std;

/// <summary>
/// Hàm để đọc 1 sector
/// </summary>
/// <param name="drive">: tên ổ đĩa</param>
/// <param name="readPoint">: bắt đầu đọc từ byte thứ readPoint của ổ drive</param>
/// <param name="sector">: biến lưu nội dung sector</param>
/// <returns></returns>
int ReadSector(LPCWSTR  drive, unsigned long long readPoint, BYTE sector[512]);
