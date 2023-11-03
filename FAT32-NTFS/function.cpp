#include "function.h"
//-------------------------------------- BIẾN TOÀN CỤC CHUNG ---------------------------------------------------

LPCWSTR dirName; //Tên ổ đĩa


//-------------------------------------- BIẾN TOÀN CỤC CHO NTFS ---------------------------------------------------

int NTFS_sector_size = 0; //Kích thước một sector. Đơn vị tính là byte.
int NTFS_sector_per_cluster = 0; //Số sector trong một cluster.
unsigned long long NTFS_sector_startIndex_logic = 0; //Sector bắt đầu của ổ đĩa logic.
unsigned long long NTFS_numberOfSector_logic = 0; //Số sector của ổ đĩa logic.
unsigned long long NTFS_MFTcluster_startIndex = 0; //Cluster bắt đầu của MFT.
int NTFS_MTF_entry_size = 0; //Kích thước của một bản ghi MFT (MFT entry). Đơn vị tính là byte.
long long NTFS_MFT_size = 0; //Kích thước của MFT
int NTFS_root_directory_ID = 0; //ID của thư mục gốc
map<long long, vector<pair<long long, string> > > NTFS_Child_List;
map<long long,vector<pair<string, string> >>  NTFS_Child_Data; //first: tên file/folder, second: data bên trong (nếu là file)



//-------------------------------------- BIẾN TOÀN CỤC CHO FAT32 ---------------------------------------------------

vector <pair<MAIN_ENTRY, int>> FAT32_EntryList;  //Dung cho truy xuat


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
        //str += static_cast<char>(sector[i + startIndex]); 
            //CAP NHAT de loai bo chen ki tu '\0' va ki tu trong 0xFF
        char c = static_cast<char>(sector[i + startIndex]);
        if (c != '\0' && sector[i + startIndex] != 0xFF)
        {
            str += c;
        }
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
        // xuống dòng cho dễ nhìn
        //------------------------------------------------------------
       
        if ((i + 1) % 16 == 0)
            cout << endl;
        //----------------------------------------------------------------
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

bool IsNTFS(BYTE* sector) {
    string dirNamestring = "";
    cout << "Nhap ten o dia (VD: G): ";
    cin >> dirNamestring;

    // Xử lý chuyển từ tên ổ đĩa sang kiểu LPCWSTR để truyền vào hàm ReadSector
    dirNamestring.push_back(':');
    dirNamestring.insert(0, "\\\\.\\");

    wstring stemp = wstring(dirNamestring.begin(), dirNamestring.end());
    const wchar_t* dirNametemp = stemp.c_str();
    size_t stringLength = wcslen(dirNametemp) + 1;
    wchar_t* wideString = new wchar_t[stringLength];
    wcscpy_s(wideString, stringLength, dirNametemp);
    dirName = wideString;
    //tới chỗ này hoy :0


    ReadSector(dirName, 0, sector, 512);

    //Nếu là hệ thống NTFS
    if (ByteArrToString(sector, 0x03, 8) == "NTFS    ") {
        return true;
    }

    //Nếu là FAT32
    else {
        return false;
    }
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
    string File_Name = "";
    int Parent_ID = 0;
    
    
    //cout << LittleEndian_HexaToDecimal(sector, 0, 4) << "\n";
    
    int First_Attribute_Offet = LittleEndian_HexaToDecimal(sector, 0x14, 2);
    long long ID = LittleEndian_HexaToDecimal(sector, 0x2C, 4);
    // Duyệt qua các attribute.
    int Attribute_Index = LittleEndian_HexaToDecimal(sector, 0x28, 2);
    int current_Index = First_Attribute_Offet;
    for (int i = 0; i < Attribute_Index; ++i) {
        int Attribute_Type = LittleEndian_HexaToDecimal(sector, current_Index, 4);
        if (Attribute_Type == -1)
            return 1;
        int Attribute_Size = LittleEndian_HexaToDecimal(sector, current_Index + 4, 4);
        int Attribute_Data_Offset = LittleEndian_HexaToDecimal(sector, current_Index + 20, 2);
        int Nonresident_Flag = LittleEndian_HexaToDecimal(sector, current_Index + 8, 1);        

        if (Nonresident_Flag == 0) {
            // Xử lí resident ở đây
            
                // Đây là attribute $STANDARD_INFORMATION
            if (Attribute_Type == 16) {
                int flag = LittleEndian_HexaToDecimal(sector, current_Index + Attribute_Data_Offset + 32, 4);
                
                if ((((flag & 2) != 0) || ((flag & 4) != 0) || ((flag & 6) != 0)/* || ((flag & 32) != 0)*/) && (NTFS_MFT_size != 0)) return 1;
            }

                // Đây là attribute $FILE_NAME
            if (Attribute_Type == 48) {
                int File_Name_Length = LittleEndian_HexaToDecimal(sector, current_Index + Attribute_Data_Offset + 64, 1);
                Parent_ID = LittleEndian_HexaToDecimal(sector, current_Index + Attribute_Data_Offset, 6);
                if (NTFS_root_directory_ID == 0)
                    NTFS_root_directory_ID = Parent_ID;
                File_Name = HexaToUnicodeUTF16(sector, current_Index + Attribute_Data_Offset + 66, 2 * File_Name_Length);
                if (File_Name.length() > 0 && File_Name[0] != '$') {
                    NTFS_Child_List[Parent_ID].push_back({ ID, File_Name });
                    NTFS_Child_Data[Parent_ID].push_back({ File_Name,"" });
                }
                    
            }

                // Đây là attribute $DATA
            if (Attribute_Type == 128) {
                // Lấy MFT size
                if (NTFS_MFT_size == 0) { // Nếu NTFS_MFT_size =0 => chưa đọc NTFS_MFT_size, là đang ở $MFT => chỉ lấy size rồi skip
                    NTFS_MFT_size = LittleEndian_HexaToDecimal(sector, current_Index + 48, 8);
                }
                else {
                    if (File_Name.find(".txt") != string::npos) {
                        int Data_Length = LittleEndian_HexaToDecimal(sector,current_Index + 16, 4);
                        int Data_Start_Index = LittleEndian_HexaToDecimal(sector, current_Index + 20, 2);
                        string Data = ByteArrToString(sector, current_Index + Data_Start_Index, Data_Length);
                        NTFS_Child_Data[Parent_ID][NTFS_Child_Data[Parent_ID].size()-1].second = Data;
                    }
                }
            }
        }
        else {
            // Xử lí non-resident ở đây
            
                // Đây là attribute $DATA
            if (Attribute_Type == 128) {
                // Lấy MFT size
                if (NTFS_MFT_size == 0) { // Nếu NTFS_MFT_size =0 => chưa đọc NTFS_MFT_size, là đang ở $MFT => chỉ lấy size rồi skip
                    NTFS_MFT_size = LittleEndian_HexaToDecimal(sector, current_Index + 48, 8);
                }
                // read data run
                else {
                    if (File_Name.find(".txt") != string::npos) {
                        // vi tri bat dau doc
                        long long First_Cluster_Offset = LittleEndian_HexaToDecimal(sector, current_Index + 66, 6);
                        unsigned long long start_index = First_Cluster_Offset * NTFS_sector_size * NTFS_sector_per_cluster;

                        // real-size: dung luong data thuc te
                        int real_size = LittleEndian_HexaToDecimal(sector, current_Index + 48, 8);
                        // read_size: dung luong data phai doc. la boi so cua 512
                        int read_size = 0;
                        for (int i = 1; read_size < real_size; i++) {
                            read_size += NTFS_sector_size;
                        }

                        // doc datarun
                        BYTE* sector_read = new BYTE[read_size];
                        ReadSector(dirName, start_index, sector_read, read_size);

                        string Data = ByteArrToString(sector_read, 0, real_size);
                        NTFS_Child_Data[Parent_ID][NTFS_Child_Data[Parent_ID].size() - 1].second = Data;

                        //giai phong vung nho
                        delete[] sector_read;
                    }
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
        for (int j = 0; j < level ; ++j) {
            if (j == level  - 1)
                cout << "|__";
            else
                cout << "|  ";
        }
        cout << u.second << " [" << node << "-" << i << "]" << endl;
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
   
    cout << endl;
    cout << "(Thong tin trong dau ngoac vuong [ ] chi la index de truy xuat, khong nam trong ten thu muc)" << endl << endl;
    Folder_Structure_BFS(NTFS_root_directory_ID, 1);

    
}

void Choose_File() {
    int i = 0;
    int j = 0;
    cout << "\n" << "---------------------------------------------------------" << "\n";
    cout << "Nhap ma so file/tap tin nam trong dau ngoac vuong [ ] tren cay thu muc (VD: 3 0): ";
    cin >> i >> j;

    pair<string, string> u = NTFS_Child_Data[i][j];
    if (u.first.find(".txt") != string::npos) {
        cout << "Noi dung file " << u.first << ": " << endl;
        cout << u.second << endl << endl;
    }
    else if (u.first.find(".") == string::npos) {
        cout << "Thu muc " << u.first << ": " << endl;

        int FolderID = NTFS_Child_List[i][j].first;
        cout << endl;
        cout << u.first << endl;
        Folder_Structure_BFS(FolderID, 1);
    }
    else {
        cout << "Vui long dung phan mem tuong thich de doc noi dung!" << endl;
    }

}

//void test() {
//    cout << endl;
//    for (int i = 0; i < NTFS_Child_Data.size(); i++) {
//        for (int j = 0; j < NTFS_Child_Data[i].size(); j++)
//        {
//            cout << NTFS_Child_Data[i][j].first << " " << NTFS_Child_Data[i][j].second << endl;
//        }
//    }
//    //cout << NTFS_Child_Data[39][3].first << endl; //ok
//}


//------------------------------------- KHU VỰC HÀM CHO FAT32 -------------------------------------------------------
BootSector_FAT32 read_BootSector(BYTE* bootSector_ptr)
{
    BootSector_FAT32 result;
    result.byte_per_sector = LittleEndian_HexaToDecimal(bootSector_ptr, 0xB, 2);         //offset B (hex) = 11 (dec), chiếm 2 bytes
    result.Sector_per_Cluster = LittleEndian_HexaToDecimal(bootSector_ptr, 0xD, 1);      //offset D (hex) = 13 (dec), chiếm 1 byte
    result.Reserved_Sector = LittleEndian_HexaToDecimal(bootSector_ptr, 0xE, 2);         //offset E (hex) = 14 (dec), chiếm 2 byte
    result.No_FAT = LittleEndian_HexaToDecimal(bootSector_ptr, 0x10, 1);                 //offset 10(hex) = 16 (dec), 1 byte
    result.RDET_Entries = LittleEndian_HexaToDecimal(bootSector_ptr, 0x11, 2);           //offset 11(hex) = 17(dec), 2 bytes
    result.Total_Sector = LittleEndian_HexaToDecimal(bootSector_ptr, 0x20, 4);           //offset 20(hex) = 32(dec), 4 bytes
    result.Sector_per_FAT = LittleEndian_HexaToDecimal(bootSector_ptr, 0x24, 4);         //offset 24(hex) = 36(dec), 4 bytes
    result.Root = LittleEndian_HexaToDecimal(bootSector_ptr, 0x2C, 4);                   //offset 2C(hex) = 44(dec), 4 bytes
    return result;
}

void Print_BootSector(BootSector_FAT32 FAT32)
{
    cout << "----------------------------------------BOOTSECTOR----------------------------------------" << endl;
    cout << "\tFAT32" << endl;
    cout << "\tSo bytes tren 1 Sector: " << FAT32.byte_per_sector << " (bytes)." << endl;
    cout << "\tSo sector moi cluster (Sc): " << FAT32.Sector_per_Cluster << "(sectors)." << endl;
    cout << "\tSo sector cua BootSector (Sb): " << FAT32.Reserved_Sector << "(sectors)." << endl;
    cout << "\tSo bang FAT (NF): " << FAT32.No_FAT << "(sectors)" << endl;
    cout << "\tSo Entry cua RDET (SRDET): " << FAT32.RDET_Entries << "(Entries)." << endl;
    cout << "\tTong so sector (Sv): " << FAT32.Total_Sector << "(sectors)." << endl;
    cout << "\tSo sector moi bang FAT (SF): " << FAT32.Sector_per_FAT << "(sectors)." << endl;
    cout << "\tSector bat dau cu RDET: " << FAT32.Root << endl;
    cout << "------------------------------------------------------------------------------------------" << endl;

}

int read_FAT_table(LPCWSTR driver, BootSector_FAT32 fat32, unsigned int*& FAT_table_result) {
    if (FAT_table_result != nullptr) {
        delete[] FAT_table_result;
    }

    int Fat_table_size = fat32.byte_per_sector * fat32.Sector_per_FAT;
    int number_Of_entries = Fat_table_size / 4;

    FAT_table_result = new unsigned int[number_Of_entries];
    ifstream disk(driver, ios::binary);

    if (!disk) {
        cout << "Failed to open the disk" << endl;
        return -1;
    }

    char* buffer = new char[Fat_table_size];
    disk.seekg(fat32.Reserved_Sector * fat32.byte_per_sector, ios::beg);
    disk.read(buffer, Fat_table_size);
    //disk.close();

    BYTE* readBytes = (BYTE*)buffer;

    //Chuyen chuoi 4 byte little endian sang decimal
    for (int i = 0; i < number_Of_entries; i++) {
        FAT_table_result[i] = LittleEndian_HexaToDecimal(readBytes, i * 4, 4);
    }

    delete[] buffer; 

    return number_Of_entries;
}
//------------------------------------- KHU VỰC HÀM CHO CAY THU MUC FAT32 -------------------------------------------------------

int firstSectorIndex_Cluster(int clusterIndex, BootSector_FAT32 fat32) {
    //Sector dau tien cua DATA (RDET)
    int dataFirstSector = fat32.Reserved_Sector + fat32.No_FAT * fat32.Sector_per_FAT;
    return ((clusterIndex - 2) * fat32.Sector_per_Cluster) + dataFirstSector;
}

void allocatedSectors(unsigned int startCluster, unsigned int* fatTable, BootSector_FAT32 fat32) {
    //Start cluster = Phan tu FAT
    //Phan tu FAT bat dau dem tu 0....
    //int fatOffset = fat32.Reserved_Sector + (startCluster + 1) * fat32.byte_per_sector;
    //lay so sector dau cua vung data = Sb + Nf * Sf + SRDET


    int dataSector = fat32.Reserved_Sector + fat32.No_FAT * fat32.Sector_per_FAT + fat32.RDET_Entries;
 
    //Sector bat dau cua file do
    unsigned int startSector = dataSector + fat32.Sector_per_Cluster * (startCluster - fat32.Root); //Tru di 2 sector dau truoc rdet
    //cout << " " << startSector << " ";

    //So entry trong bang FAT
    unsigned int fat_table_entries = fat32.byte_per_sector * fat32.Sector_per_FAT / 4;
    int count = 0;
    unsigned int curent_Sector = startSector;

    //duyệt từng cluster theo danh sách liên kết 
    cout << " - Sector: ";
    
    while (startCluster != 0x0FFFFFFF)
    {
        if (startCluster > fat_table_entries) //Vuot qua gioi han
            break;
        cout << "(" << startSector
            << "-" << startSector + fat32.Sector_per_Cluster - 1 << "); ";
        
        startCluster = fatTable[startCluster]; //đọc phần tử thứ 2 để biết nó có phải thư mục không
        count++;
        // Calculate the starting sector of the next cluster
        startSector = dataSector + fat32.Sector_per_Cluster * (startCluster - fat32.Root);
    } 
    cout << endl;

}

void readDirectory(int firstRecordIndex, int clusIndex, unsigned int* FatTable, BootSector_FAT32 fat32, LPCWSTR driver, int level) {
    int clusterSizeByte = fat32.byte_per_sector * fat32.Sector_per_Cluster; //Moi sector co 32byte theo fat32

    //Tong entries
    int entriesCount = clusterSizeByte / 32;
    char* a = new char[clusterSizeByte];
    int firstSector = firstSectorIndex_Cluster(clusIndex, fat32);
    fstream diskStream(driver, std::ios::in);


    //Bo qua sector dau - System data
    diskStream.seekg(firstSector * fat32.byte_per_sector, SEEK_SET);

    //Doc ca cluster
    diskStream.read(a, clusterSizeByte);
    BYTE* readBytes = new BYTE[clusterSizeByte];
    readBytes = (BYTE*)a;
    
    //Khoi tao main entry
    MAIN_ENTRY mainEntry;
    //delete[] a;

    //Danh cho luu tru ten dai
    string stackName = "";

    bool hasSubEntry = false;

    int countMainEntry = 0;

    for (int i = firstRecordIndex; i < entriesCount; i++)
    {

        //cout << "Entry: " << readBytes[32 * i] << endl;
        // Entry trong
        if (readBytes[32 * i] == 0x0)
            break;
        // Tap tin da bi xoa
        else if (readBytes[32 * i] == 0xE5) 
            continue;

        int entryStatus = LittleEndian_HexaToDecimal(readBytes, 32 * i + 0xB, 1);

        //Case entry phu
        if (entryStatus == 0xF)
        {
            //Xu ly ten dai
            /*Doc tung entry, doc ten tu tren xuong
            push vo stack name
            de lat toi entry chinh pop ra -> lay duoc ten file
            */
            string tempString = "";
            string tempName = "";
            tempString = ByteArrToString(readBytes, 32 * i + 0x1, 10);
            //Offset 1 - 10 bytes
            tempName = tempName + tempString;
            tempString = "";

            //Offset 0xE - 12 bytes
            tempString = ByteArrToString(readBytes, 32 * i + 0xE, 12);
            tempName = tempName + tempString;
            tempString = "";
            //Offset 1xC - 4 bytes
            tempString = ByteArrToString(readBytes, 32 * i + 0x1C, 4);
            tempName = tempName + tempString;
            tempString = "";


            hasSubEntry = true;
            stackName.insert(0, tempName);
            tempName = "";
            //Nhay qua entry tiep theo
            continue;
        }

        //Cac loai khac 0xF
        else {
            //Neu co subentry
            if (hasSubEntry) {
         
                //Lay 3 ki tu cuoi cung
                if (entryStatus == 0x20) { //Neu la file
                    mainEntry.name = stackName.substr(0, stackName.size() - 4);
                    mainEntry.extensionName = stackName.substr(stackName.size() - 3);
                }
                else {
                    mainEntry.name = stackName;

                    mainEntry.extensionName = "";
                }
            
            } //Khong cos subentry
            else {
                mainEntry.name = ByteArrToString(readBytes, 0x0, 8);
                if (entryStatus == 0x20) { //Neu la file
                    mainEntry.extensionName = stackName.substr(stackName.size() - 3);
                }
                else {
                    mainEntry.extensionName = "";
                }
            }
            stackName = "";
            countMainEntry++;


            //Decode loai
            //0 - read only
            //1 - hidden
            //2 - system
            //3 - volable
            //4 - directory
            //5 - archive - file
            //Chuyen hex sang nhi phan, tim vi tri cua bit 1 trong day nhi phan, vi tri = type
            char statuses[] = {'R', 'H', 'S', 'V', 'D', 'A', '0', '0'};
          

            //tim vi tri xong xuat ra cai nay
            string status, statusBin;
            //Doc byte tai 0xB
            statusBin = HexaToBinary(entryStatus);
            for (int i = statusBin.size() - 1; i >= 0; i--)
            {
                if (statusBin[i] == '1') {
                    int temp = statusBin.size() - 1 - i;
                    status += statuses[temp];
                  
                }
            }

            //Gan gia tri cua 
            mainEntry.attribute = status;
            mainEntry.attributeInHex = entryStatus;

            //2 byte cao + 2 byte thap
            int startClusterHigh = LittleEndian_HexaToDecimal(readBytes, 32 * i + 0x14, 2);
            int startClusterLow = LittleEndian_HexaToDecimal(readBytes, 32 * i + 0x1A, 2);

            mainEntry.startCluster = startClusterHigh + startClusterLow;
            mainEntry.fileSize = LittleEndian_HexaToDecimal(readBytes, 32 * i + 0x1C, 4);



        }
     
       
        FAT32_EntryList.push_back(make_pair(mainEntry, FAT32_EntryList.size()));
        cout << "[" << FAT32_EntryList.size() - 1 << "] ";
        for (int i = 0; i < level; i++)
        {
            
            if (i == level - 1)
            {
                cout << "|__";

            }
            else {
                cout << "|  ";
            }
            
            
        }
       
        cout << mainEntry.name << " - " << mainEntry.attribute << " - " << mainEntry.fileSize << " bytes";
        allocatedSectors(mainEntry.startCluster, FatTable, fat32);

        //La thu muc
        if (entryStatus == 0x10) {

            //Goi ham de quy xuong thu muc con
            readDirectory(2, mainEntry.startCluster, FatTable, fat32, driver, level + 1);
        }

 

    }

}

void readContentOfFile(MAIN_ENTRY entry, BootSector_FAT32 fat32, unsigned int clusIndex, LPCWSTR drive1)
{
     ////----------------------------------------- là tập tin----------------
    bool viewable = false;

    if (entry.attributeInHex == 0x20)
    {
        if (entry.extensionName == "txt" || entry.extensionName == "TXT")// file text
        {
               
            cout << ">> Noi dung file: " << endl;
            cout << "...Dang doc file" << endl;
            viewable = true;
        }
        else// file khác
        {
           
            cout << ">> Vui long cho trinh duyet phu hop de doc noi dung file" << endl;
            return;
        }
    }
    else {
        cout << ">> Vui long chon file phu hop" << endl;

    }
        ////-----------------------------------------------------------------------------
    if (viewable) {
        string content;
        unsigned int* Fat_table = NULL;
        unsigned int FAT_size = read_FAT_table(drive1, fat32, Fat_table);

        int bytes_per_Cluster = fat32.byte_per_sector * fat32.Sector_per_Cluster;
        //BYTE* buffer = new BYTE[bytes_per_Cluster];
        BYTE* buffer = new BYTE[fat32.byte_per_sector];

        do
        {
            long long data_offset = firstSectorIndex_Cluster(clusIndex, fat32) * fat32.byte_per_sector;


            int dataSector = fat32.Reserved_Sector + fat32.No_FAT * fat32.Sector_per_FAT + fat32.RDET_Entries;

            //Sector bat dau cua file do
            unsigned int startSector = dataSector + fat32.Sector_per_Cluster * (clusIndex - fat32.Root); //Tru di 2 sector dau truoc rdet
            for (int i = 0; i < fat32.Sector_per_Cluster; i++)
            {
                ReadSector(drive1, (startSector + i) * fat32.byte_per_sector, buffer, fat32.byte_per_sector);
                content += ByteArrToString(buffer, 0, fat32.byte_per_sector);

            }



            //cout << buffer;
            if (FAT_size > clusIndex)
                clusIndex = Fat_table[clusIndex];
            else
                break;
        } while (clusIndex != 0x0FFFFFFF);
        if (content.empty())
            cout << "File rong." << endl;
        else
            cout << content << endl;

        //giải phóng vùng nhớ.
        delete[]buffer;
    }
    
}



void chooseFileFAT(BootSector_FAT32 fat32, LPCWSTR drive1) {
    int choice = 0;
    cout << ">> Chon file muon truy xuat [0, 1...]: ";
    cin >> choice;

    readContentOfFile(FAT32_EntryList[choice].first, fat32, FAT32_EntryList[choice].first.startCluster, drive1);


}


