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

    }




    //const char* outputFileName = "sector_data.bin";

    //MFT start:
    //342258352128: from PhysicalDrive0
    //3221225472 =  786 432 * 8 * 512: from F

    

    //cout << HexaToUnicodeUTF16(sector, 242, 22);//success
    

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

    delete[] sector;

    return 0;
}