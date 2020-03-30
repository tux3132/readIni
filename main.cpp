#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "readIni.h"
#include "myFuncts.h"

char buffIni[40];
char iniFile[20];

bool DEBUG_CONFIG;
uint8_t MCP2515_fOsc;
float CANBUS_SPPos;
uint16_t BOND_James;

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

    if(!initFuncts(iniFile))
    {
        puts("initFuncts error");
        return EXIT_FAILURE;
    }

    puts("initFuncts OK");

    printf("Setting DEBUG_CONFIG %i\n", DEBUG_CONFIG);
    printf("Setting MCP2515_fOsc %i\n", MCP2515_fOsc);
    printf("Setting CANBUS_SPPos %f\n", CANBUS_SPPos);
    printf("BOND_James %i\n", BOND_James);

    return EXIT_SUCCESS;
}
