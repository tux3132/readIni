#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "readIni.h"

char buffIni[40];
char iniFile[20];

int main(int argc, char* argv[])
{
    if(argc>1)
    {
        strcpy(iniFile,argv[1]);
    }
    else
    {
        strcpy(iniFile,"config.ini");
    }

    ini_gets("DEBUG","CONFIG","0",buffIni,2,"config.ini");
    bool DEBUG_CONFIG=atoi(buffIni);
    printf("Setting DEBUG_CONFIG %i\n", DEBUG_CONFIG);

    ini_gets("MCP2515","fOsc","20",buffIni,3,"config.ini");
    uint8_t MCP2515_fOsc=atoi(buffIni);
    printf("Setting MCP2515_fOsc %i\n", MCP2515_fOsc);

    ini_gets("CANBUS","SPPos","0.75",buffIni,5,"config.ini");
    float CANBUS_SPPos=atof(buffIni);
    printf("Setting CANBUS_SPPos %f\n", CANBUS_SPPos);

    if(!ini_gets("BOND","James","65535",buffIni,6,"config.ini"))
    {
        printf("Setting BOND_James = %s\n", buffIni);
    }
    else
    {
        printf("BOND_James Key not found : setting to default = %s\n", buffIni);
    }

    uint16_t BOND_James=atoi(buffIni);
    printf("BOND_James %i\n", BOND_James);

    return EXIT_SUCCESS;
}
