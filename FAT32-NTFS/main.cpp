#include "function.h"


int main(int argc, char** argv) //file main tui test thoi, mn cứ xóa nhe
{

    BYTE sector[512];
    const char* outputFileName = "sector_data.bin";

    //MFT start:
    //342258352128: from PhysicalDrive0
    //3221225472 =  786 432 * 8 * 512: from F
    ReadSector(L"\\\\.\\F:", 0, sector);
    Read_VBR(sector);

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

    

    return 0;
}