﻿#include "function.h"


int main(int argc, char** argv) //file main tui test thoi, mn cứ xóa nhe
{

    BYTE sector[512];
    //const char* outputFileName = "sector_data.bin";

    //MFT start:
    //342258352128: from PhysicalDrive0
    //3221225472 =  786 432 * 8 * 512: from F
    LPCWSTR driver = L"\\\\.\\E:";
    ReadSector(L"\\\\.\\E:", 0, sector, 512);
    //cout << HexaToUnicodeUTF16(sector, 242, 22);//success
    //Read_VBR(sector); //sucess


    //cout << ByteArrToString(sector, 0,5); //success
    //cout << MFTEntry_Size(sector); //sucess
    
    /*FILE* outputFile = fopen(outputFileName, "wb");

    if (outputFile == NULL)
    {
        printf("Error opening the output file.\n");
        return 1;
    }
    int bytesWritten = fwrite(sector, 1, 512, outputFile);

    if (bytesWritten != 512)
    {
        printf("Error writing to the output file.\n");
        fclose(outputFile);
        return 1;
    }

    printf("Sector data successfully written to %s\n", outputFileName);

    fclose(outputFile);*/

    BootSector_FAT32 fat32 = read_BootSector(sector);
    Print_BootSector(fat32);
    cout << "-------------------------------------BootSector doc duoc-------------------------------------" << endl;
    //PrintHexa(sector, 0, 512);
    unsigned int* FAT_table = nullptr;

    read_FAT_table(L"\\\\.\\E:", fat32, FAT_table);
    if (!FAT_table)
        cout << "Table null";
    for (int i = 0; i < 10; i++)
        cout << FAT_table[i] << ' ';
    
    readDirectory(0, fat32.Root, FAT_table, fat32, driver, 1);
    
    return 0;
}
