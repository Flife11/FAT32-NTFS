#include "function.h"

//-------------------------------------- BIẾN TOÀN CỤC CHO NTFS ---------------------------------------------------

int NTFS_sector_size = 0; //Kích thước một sector. Đơn vị tính là byte.
int NTFS_sector_per_cluster = 0; //Số sector trong một cluster.
int NTFS_sector_startIndex_logic = 0; //Sector bắt đầu của ổ đĩa logic.
int NTFS_numberOfSector_logic = 0; //Số sector của ổ đĩa logic.
int NTFS_cluster_startIndex = 0; //Cluster bắt đầu của MFT.


//-------------------------------------- BIẾN TOÀN CỤC CHO FAT32 ---------------------------------------------------




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

unsigned long long LittleEndian_HexaToDecimal(BYTE sector[], int startIndex, int length) {

    unsigned long long result = 0;
    //unsigned long long Pow = 0;

    for (int i = 0; i < length; i++) {

        result += (unsigned long long)sector[i+ startIndex] << (i * 8);

        /*result += ((unsigned long long)sector[i] * pow(16, Pow));
        Pow += 2;*/
    }

    /*VD: cho một số hệ hexa 4649 lưu ở little endian, muốn chuyển 49 sang hệ thập phân ta có thể dùng công thức 9*16^2 + 4*16^3 = 18688.
    Tuy nhiên, nếu đổi 49 sang hệ thập phân trước, 49 = 9*16^0 + 4*16^1 = 73, rồi lấy 73 * 16^2 = 18688 (2^8 = 16^2)*/

    return result;
}

string ByteArrToString(BYTE sector[], int startIndex, int length)
{
    string str = "";
    for (int i = 0; i < length; ++i)
    {
        str += static_cast<char>(sector[i + startIndex]);
    }
    return str;
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

void PrintHexa(BYTE sector[], int startIndex, int length) {

    for (int i = 0; i < length; i++)
    {
        cout << hex << setfill('0') << setw(2) << static_cast<int>(sector[startIndex + i]) << " ";
    }
    cout << endl;
}


//------------------------------------- KHU VỰC HÀM CHO NTFS -------------------------------------------------------

int MFTEntry_Size(BYTE sector_VBR[]) {

    BYTE byte_40h_BPB = sector_VBR[64];

    int decimal = abs(BinaryToDecimal(HexaToBinary(byte_40h_BPB)));

    return (pow(2, decimal));
}

void Read_VBR(BYTE sector[]) {

    cout << "Jump Instruction to bypass the DPB: ";
    PrintHexa(sector, 0x00, 3);

    cout << "Driver Sig: \"" << ByteArrToString(sector, 0x03, 8) << "\"" << endl;

    NTFS_sector_size = LittleEndian_HexaToDecimal(sector, 0x0B, 2);
    cout << "Bytes/Sector: " << NTFS_sector_size << endl;

    NTFS_sector_per_cluster = LittleEndian_HexaToDecimal(sector, 0x0D, 1);
    cout << "Sectors/Cluster: " << NTFS_sector_per_cluster << endl;
    
    cout << "Reserved Sectors: " << LittleEndian_HexaToDecimal(sector, 0x0E, 2) << endl;

    cout << "# of FAT's: " << LittleEndian_HexaToDecimal(sector, 0x10, 1) << endl;

    cout << "max. # of root dir entries: " << LittleEndian_HexaToDecimal(sector, 0x11, 2) << endl;

    cout << "total sectors (partitions < 32MB): " << LittleEndian_HexaToDecimal(sector, 0x13, 2) << endl;

    cout << "Media Descriptor: ";
    PrintHexa(sector, 0x15, 1);

    cout << "sectors/FAT: " << LittleEndian_HexaToDecimal(sector, 0x16, 2) << endl;

    cout << "sectors/track: " << LittleEndian_HexaToDecimal(sector, 0x18, 2) << endl;

    cout << "total heads: " << LittleEndian_HexaToDecimal(sector, 0x1A, 2) << endl;

    cout << "Hidden sectors: " << LittleEndian_HexaToDecimal(sector, 0x1C, 4) << endl;

    cout << "total sectors (partitions > 32MB): " << LittleEndian_HexaToDecimal(sector, 0x20, 4) << endl;

    cout << "sectors/FAT: " << LittleEndian_HexaToDecimal(sector, 0x24, 4) << endl;

    cout << "flags: " << LittleEndian_HexaToDecimal(sector, 0x13, 2) << endl;

    cout << "total sectors (partitions < 32MB): " << LittleEndian_HexaToDecimal(sector, 0x13, 2) << endl;

    cout << "total sectors (partitions < 32MB): " << LittleEndian_HexaToDecimal(sector, 0x13, 2) << endl;

    cout << "total sectors (partitions < 32MB): " << LittleEndian_HexaToDecimal(sector, 0x13, 2) << endl;

    cout << "total sectors (partitions < 32MB): " << LittleEndian_HexaToDecimal(sector, 0x13, 2) << endl;

    cout << "total sectors (partitions < 32MB): " << LittleEndian_HexaToDecimal(sector, 0x13, 2) << endl;

    cout << "total sectors (partitions < 32MB): " << LittleEndian_HexaToDecimal(sector, 0x13, 2) << endl;

    cout << "total sectors (partitions < 32MB): " << LittleEndian_HexaToDecimal(sector, 0x13, 2) << endl;

    cout << "total sectors (partitions < 32MB): " << LittleEndian_HexaToDecimal(sector, 0x13, 2) << endl;

    cout << "total sectors (partitions < 32MB): " << LittleEndian_HexaToDecimal(sector, 0x13, 2) << endl;

    cout << "total sectors (partitions < 32MB): " << LittleEndian_HexaToDecimal(sector, 0x13, 2) << endl;

    cout << "total sectors (partitions < 32MB): " << LittleEndian_HexaToDecimal(sector, 0x13, 2) << endl;
}

//------------------------------------- KHU VỰC HÀM CHO FAT32 -------------------------------------------------------