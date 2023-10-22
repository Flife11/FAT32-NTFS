#include "function.h"

//-------------------------------------- BIẾN TOÀN CỤC CHO NTFS ---------------------------------------------------

int NTFS_sector_size = 0; //Kích thước một sector. Đơn vị tính là byte.
int NTFS_sector_per_cluster = 0; //Số sector trong một cluster.
unsigned long long NTFS_sector_startIndex_logic = 0; //Sector bắt đầu của ổ đĩa logic.
unsigned long long NTFS_numberOfSector_logic = 0; //Số sector của ổ đĩa logic.
unsigned long long NTFS_MFTcluster_startIndex = 0; //Cluster bắt đầu của MFT.
int NTFS_MTF_entry_size = 0; //Kích thước của một bản ghi MFT (MFT entry). Đơn vị tính là byte.
long long NTFS_MFT_size = 0;
map<long long, vector<pair<long long, string> > > NTFS_Child_List;


//-------------------------------------- BIẾN TOÀN CỤC CHO FAT32 ---------------------------------------------------




//-------------------------------------- KHU VỰC HÀM CHUNG (CHO CẢ NTFS VÀ FAT32) ------------------------------------------

int ReadSector(LPCWSTR  drive, unsigned long long readPoint, BYTE* sector, int length) {
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

    if (!ReadFile(device, sector, length, &bytesRead, NULL))
    {
        return 0;
        //printf("ReadFile: %u\n", GetLastError());
    }
    else
    {
        return 1;
        //printf("Success!\n");
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
    cout << dec << endl;
}

string HexaToUnicodeUTF16(BYTE sector[], int startIndex, int length) {
    string unicode = "";
    int c = 0;
    for (int i = 0; i < length; i+=2)
    {
        c = LittleEndian_HexaToDecimal(sector, startIndex + i, 2);
        unicode.push_back((const char)c);
    }

    return unicode;
}

//------------------------------------- KHU VỰC HÀM CHO NTFS -------------------------------------------------------

int MFTEntry_Size(BYTE sector_VBR[]) {

    BYTE byte_40h_BPB = sector_VBR[64];

    int decimal = abs(BinaryToDecimal(HexaToBinary(byte_40h_BPB)));

    return (pow(2, decimal));
}

void Read_VBR(BYTE sector[]) {

    cout << "********************* [VBR - VOLUME BOOT RECORD] *********************" << endl;

    cout << "Jump Instruction: ";
    PrintHexa(sector, 0x00, 3);

    cout << "OEM ID: \"" << ByteArrToString(sector, 0x03, 8) << "\"" << endl;

    cout << "-------------- BPB - BIOS parameter block --------------" << endl;

    NTFS_sector_size = LittleEndian_HexaToDecimal(sector, 0x0B, 2);
    cout << "Bytes/Sector: " << NTFS_sector_size << endl;

    NTFS_sector_per_cluster = LittleEndian_HexaToDecimal(sector, 0x0D, 1);
    cout << "Sectors/Cluster: " << NTFS_sector_per_cluster << endl;
    
    cout << "Reserved Sectors: " << LittleEndian_HexaToDecimal(sector, 0x0E, 2) << endl;

    cout << "always 0: ";
    PrintHexa(sector, 0x10, 3);

    cout << "not used by NTFS: ";
    PrintHexa(sector, 0x13, 2);

    cout << "Media Descriptor: " << LittleEndian_HexaToDecimal(sector, 0x15, 1) << endl;

    cout << "always 0: ";
    PrintHexa(sector, 0x16, 2);

    cout << "sectors/track: " << LittleEndian_HexaToDecimal(sector, 0x18, 2) << endl;

    cout << "Number Of Heads: " << LittleEndian_HexaToDecimal(sector, 0x1A, 2) << endl;

    NTFS_sector_startIndex_logic = LittleEndian_HexaToDecimal(sector, 0x1C, 4);
    cout << "Hidden sectors: " << NTFS_sector_startIndex_logic << endl;

    cout << "not used by NTFS: ";
    PrintHexa(sector, 0x20, 4);

    cout << "Signature: "; 
    PrintHexa(sector, 0x24, 4);

    NTFS_numberOfSector_logic = LittleEndian_HexaToDecimal(sector, 0x28, 8); //
    cout << "Total Sectors: " << NTFS_numberOfSector_logic << endl;

    NTFS_MFTcluster_startIndex = LittleEndian_HexaToDecimal(sector, 0x30, 8);
    cout << "Logical Cluster Number for the file $MFT: " << NTFS_MFTcluster_startIndex << endl;

    cout << "Logical Cluster Number for the file $MFTMirr: " << LittleEndian_HexaToDecimal(sector, 0x38, 8) << endl;

    NTFS_MTF_entry_size = MFTEntry_Size(sector);
    cout << "MFT entry size (in bytes): " << NTFS_MTF_entry_size << endl;

    cout << "Clusters Per Index Buffer: " << LittleEndian_HexaToDecimal(sector, 0x44, 1) << endl;

    cout << "not used by NTFS: ";
    PrintHexa(sector, 0x45, 3);

    cout << "Volume Serial Number ";
    PrintHexa(sector, 0x48, 8);

    cout << "Checksum: " << LittleEndian_HexaToDecimal(sector, 0x50, 1) << endl;

    cout << "--------------------------------------------------------" << endl;

    cout << "Boostrap code: ";
    PrintHexa(sector, 0x54, 426);

    cout << "Signature (55 AA): ";
    PrintHexa(sector, 0x1FE, 2);

    cout << "************************************************************" << endl;
}

bool Read_Entry(unsigned long long Start_Address_MFT)
{
    BYTE* sector = new BYTE[NTFS_MTF_entry_size];
    bool readable = ReadSector(dirName, Start_Address_MFT, sector, NTFS_MTF_entry_size);


    //cout << LittleEndian_HexaToDecimal(sector, 0, 4) << "\n";

    int First_Attribute_Offset = LittleEndian_HexaToDecimal(sector, 0x14, 2);
    long long ID = LittleEndian_HexaToDecimal(sector, 0x2C, 4);
    // Duyệt qua các attribute.
    int Attribute_Index = LittleEndian_HexaToDecimal(sector, 0x28, 2);
    int current_Index = First_Attribute_Offset;
    for (int i = 0; i < Attribute_Index; ++i) {
        int Attribute_Type = LittleEndian_HexaToDecimal(sector, current_Index, 4);
        int Attribute_Size = LittleEndian_HexaToDecimal(sector, current_Index + 4, 4);
        int Attribute_Data_Offset = LittleEndian_HexaToDecimal(sector, current_Index + 20, 2);
        int Nonresident_Flag = LittleEndian_HexaToDecimal(sector, current_Index + 8, 1);

        // Xử lí resident 
        if (Nonresident_Flag == 0) {
            // $FILE_NAME
            if (Attribute_Type == 48) {
                int File_Name_Length = LittleEndian_HexaToDecimal(sector, current_Index + Attribute_Data_Offset + 64, 1);
                int Parent_ID = LittleEndian_HexaToDecimal(sector, current_Index + Attribute_Data_Offset, 6);
                string File_Name = ByteArrToString(sector, current_Index + Attribute_Data_Offset + 66, 2 * File_Name_Length);
                NTFS_Child_List[Parent_ID].push_back({ ID, File_Name });
                //cout << ID << " " << File_Name << "\n";                
            }
            // $STANDARD_INFORMATION
            if (Attribute_Type == 16) {
                int flag = LittleEndian_HexaToDecimal(sector, current_Index + Attribute_Data_Offset + 32, 4);

                if ((((flag & 2) != 0) || ((flag & 4) != 0) || ((flag & 32) != 0)) && (NTFS_MFT_size != 0)) return 1;
            }
        }
        // Xử lí non-resident
        else {
            // $DATA
            if (Attribute_Type == 128) {
                //// Lấy MFT size
                //if (NTFS_MFT_size == 0) {
                //    NTFS_MFT_size = LittleEndian_HexaToDecimal(sector, current_Index + 48, 8);
                //}

                // duyệt qua từng data run để đọc hết file 
                string data;
                int Cluster_Count = LittleEndian_HexaToDecimal(sector, current_Index + 65, 1);
                int First_Cluster_Offset = 0;
                First_Cluster_Offset = LittleEndian_HexaToDecimal(sector, current_Index + 66, 8);
                unsigned long long Current_Offset = 0;
                for (int j = 0; j < Cluster_Count; j++)
                {
                    Current_Offset += First_Cluster_Offset;
                    data = data + HexaToUnicodeUTF16(sector, Current_Offset, First_Cluster_Offset);
                }

            }
        }
        current_Index += Attribute_Size;
    }

    //Giải phóng vùng nhớ
    delete[] sector;

    return 1;
}

void Folder_Structure_BFS(long long node, int level)
{
    int n = NTFS_Child_List[node].size();
    for (int i = 0; i < n; ++i) {
        pair<long long, string> u = NTFS_Child_List[node][i];
        for (int j = 0; j < level * 2; ++j) cout << "--";
        cout << u.second << "\n";
        Folder_Structure_BFS(u.first, level + 1);
    }
}

void Read_MFT() {
    // Tính sector bắt đầu của MFT
    unsigned long long Start_Sector_MFT = NTFS_MFTcluster_startIndex * NTFS_sector_per_cluster;
    // Tính vị trí bắt đầu của MFT dựa vào số thứ tự sector
    unsigned long long Start_Address_MFT = Start_Sector_MFT * NTFS_sector_size;

    long long size = 0;
    do {
        Read_Entry(Start_Address_MFT);
        Start_Address_MFT += NTFS_MTF_entry_size;
        size += NTFS_MTF_entry_size;        
    } while (size < NTFS_MFT_size);
   
    Folder_Structure_BFS(39, 1);

    
}


//------------------------------------- KHU VỰC HÀM CHO FAT32 -------------------------------------------------------