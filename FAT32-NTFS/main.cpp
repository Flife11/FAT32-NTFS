#include "function.h"


int main(int argc, char** argv) //file main tui test thoi, mn cứ xóa nhe
{
    BYTE* sector = new BYTE[512];
    bool isNTFS = IsNTFS(sector);

    int choose_file = 1;

    //Nếu là hệ thống NTFS
    if (isNTFS == true) {
        Read_VBR(sector); //sucess
        Read_MFT(); //developing

        do
        { 
            Choose_File();
            cout << endl;
            cout << "Tiep tuc doc file/folder? (0: ngung, 1: tiep tuc): ";
            cin >> choose_file;
        } while (choose_file!=0);
        
    }

    //Nếu là FAT32
    else {
          LPCWSTR driver = L"\\\\.\\E:";
          ReadSector(L"\\\\.\\E:", 0, sector, 512);


          BootSector_FAT32 fat32 = read_BootSector(sector);
          Print_BootSector(fat32);

          cout << "-------------------------------------BootSector doc duoc-------------------------------------" << endl;
          PrintHexa(sector, 0, 512);
          unsigned int* FAT_table = nullptr;
          //Chuyen doi bang FAT
          read_FAT_table(L"\\\\.\\E:", fat32, FAT_table);
          cout << "-------------------------------------CAY THU MUC FAT32-------------------------------------\n";
          //Doc cay thu muc
          readDirectory(0, fat32.Root, FAT_table, fat32, driver, 1);
          int choiceFAT = 0;

          do
          {
              chooseFileFAT(fat32, driver);
              cout << endl;
              cout << "Tiep tuc doc file/folder? (0: ngung, khac: tiep tuc): ";
              cin >> choiceFAT;
          } while (choiceFAT != 0);
    }


   


    fclose(outputFile);*/

    delete[] sector;


    return 0;
}
