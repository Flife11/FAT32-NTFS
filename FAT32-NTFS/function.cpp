#include "function.h"

//-------------------------------------- KHU VỰC HÀM CHUNG (CHO CẢ NTFS VÀ FAT32) ------------------------------------------

int ReadSector(LPCWSTR  drive, unsigned long long readPoint, BYTE sector[512]) {
    int retCode = 0;
    DWORD bytesRead;
    HANDLE device = NULL;

    device = CreateFile(drive,    // Drive to open
        GENERIC_READ,           // Access mode
        FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
        NULL,                   // Security Descriptor
        OPEN_EXISTING,          // How to create
        0,                      // File attributes
        NULL);                  // Handle to template

    if (device == INVALID_HANDLE_VALUE) // Open Error
    {
        printf("CreateFile: %u\n", GetLastError());
        return 1;
    }
    LARGE_INTEGER li;
    li.QuadPart = readPoint;

    SetFilePointerEx(device, li, NULL, FILE_BEGIN);//Set a Point to Read

    if (!ReadFile(device, sector, 512, &bytesRead, NULL))
    {
        printf("ReadFile: %u\n", GetLastError());
    }
    else
    {
        printf("Success!\n");
    }
}

unsigned long long LittleEndian_HexaToDecimal(BYTE byteArr[], int length) {

    unsigned long long result = 0;
    //unsigned long long Pow = 0;

    for (int i = 0; i < length; i++) {

        result += (unsigned long long)byteArr[i] << (i * 8);

        /*result += ((unsigned long long)byteArr[i] * pow(16, Pow));
        Pow += 2;*/
    }

    /*VD: cho một số hệ hexa 4649 lưu ở little endian, muốn chuyển 49 sang hệ thập phân ta có thể dùng công thức 9*16^2 + 4*16^3 = 18688.
    Tuy nhiên, nếu đổi 49 sang hệ thập phân trước, 49 = 9*16^0 + 4*16^1 = 73, rồi lấy 73 * 16^2 = 18688 (2^8 = 16^2)*/

    return result;
}

string HexaToBinary(BYTE hexa) {

    string binary = "";

    while (hexa > 0) {
        binary.insert(0, to_string(hexa % 2));
        hexa /= 2;
    }

    return binary;
}

int BinaryToDecimal(string binary) {

    int decimal = 0;

    if (binary[0] == '1') {
        decimal = -1;
    }

    for (int i = 0; i < binary.length(); i++) {
        
        decimal = (decimal * 2) + (binary[i] - '0');
    }

    return decimal;
}

string ByteArrToString(BYTE byteArr[], int length)
{
    string str = "";
    for (int i = 0; i < length; ++i)
    {
        str += static_cast<char>(byteArr[i]);
    }
    return str;
}


//------------------------------------- KHU VỰC HÀM CHO NTFS -------------------------------------------------------

int MFTEntry_Size(BYTE byte_40h_BPB) {
    int decimal = abs(BinaryToDecimal(HexaToBinary(byte_40h_BPB)));

    return (pow(2, decimal));
}


//------------------------------------- KHU VỰC HÀM CHO FAT32 -------------------------------------------------------