#include "function.h"


int main(int argc, char** argv) //file main tui test thoi, mn cứ xóa nhe
{

    BYTE sector[512];
    const char* outputFileName = "sector_data1.bin";

    //342258352128: from PhysicalDrive0
    //3221225472 =  786 432 * 8 * 512: from F
    ReadSector(L"\\\\.\\F:", 3221225472, sector);   
    
    
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

    BYTE test[2];
    test[0] = sector[21];
    test[1] = sector[20];

    cout << test;
    //test

    return 0;
}