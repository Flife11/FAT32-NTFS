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

    cout << LittleEndian_HexaToDecimal(sector, 8);

    /*BYTE a = 0xF6;
    cout << HexaToBinary(a) << endl;
    cout << MFTEntry_Size(BinaryToDecimal(HexaToBinary(a))) << endl;*/

    return 0;
}