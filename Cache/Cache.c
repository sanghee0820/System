#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

typedef struct line{
    /* Valid and Dirty = > Boolean True or False
       If Dirty is false Read Main Memory Only */
    bool Valid;
    bool Dirty;
    /*  Index + Tag => Adress of Main Memory
        1 word = 4 bytes = 32 bits */
    int Tag;
    char** Data;
}line;
line** Cache = NULL;

int Index_Size = 0;
int Block_size = 0;
int Set_size = 0;

int index_bits = 0;
int tag_bits = 0;
int block_offset = 0;
int bytes_offset = 2;
// Cache_Size = Index_Size * Block_Size * Set_Size  bits

// Memmory Access ---------------------------------------------------------
//                                              (Byteoffset = log_2(Block_size))

void init_Cache(int argc, char* args[], FILE** fpp);

void int_to_Hexa(int, char*); // Data (int) to Hexa
void Hexa_to_Bin(char*, char*); // Adress(Hexa) to Binary
void get_Cache_Params(int*, int*, int*, int*, char*);  // Index, Tag, Block, Byte

void Draw_Cache(char*, char, int);
int Find_Cache(int, int, int, int, int*);
void Run_Command(int, int, int, int, int, char, int);

int main(int argc, char* args[]){
    FILE *fp = NULL;
    char adress_Hexa[9]; // Max Address = 2^32 bits = 16^8 bits
    char data_Hexa[9];
    char data_Bin[33];
    char command;
    int data;
    int count = 0;

    init_Cache(argc, args, &fp);

    Cache[0][0].Tag = 8192;
    Cache[0][0].Valid = true;
    Cache[0][0].Dirty = 0;
    Cache[32][0].Tag = 8192;
    Cache[32][0].Valid = true;
    Cache[32][0].Dirty = 0;
    Cache[64][0].Tag = 8192;
    Cache[64][0].Valid = true;
    Cache[64][0].Dirty = 0;
    Cache[64][0].Tag = 8192;
    Cache[64][0].Valid = true;
    Cache[64][0].Dirty = 0;

    while(!feof(fp)){
        fscanf(fp,"%s %c",adress_Hexa, &command);
        if(command == 'W') fscanf(fp, "%d", &data);
        Draw_Cache(adress_Hexa, command, data);
    }


    Draw_Cache(adress_Hexa, 'W', 10);

    fclose(fp);
    return 0;
}

void init_Cache(int argc, char* args[], FILE** fpp){

    int Cache_size = 0;

    for(int i=1; i < argc; i++){

        // Get Paramater
        if(strstr(args[i], "-s=")){
            for(int j = 3 ; j < strlen( args[i] ); j++ ){
                Cache_size = ( Cache_size * 10 + ( args[i][j] - 48) );
            }
        }
        else if(strstr(args[i], "-b=")){
            
            for(int j = 3 ; j < strlen( args[i] ); j++ ){
                Block_size = Block_size * 10 + ( args[i][j] - 48);
            }
        }
        else if(strstr(args[i], "-a=")){

            for(int j = 3 ; j < strlen( args[i] ); j++ ){
                Set_size = Set_size * 10 + ( args[i][j] - 48);
            }
        }

        // open = tracefile / args[i]+3 = Filename;
        else if (strstr(args[i], "-f=")){
            *fpp = fopen(args[i]+3,"r");
        }
        else{
            printf("paramater error\n");
            exit(1);
        }
    }

        //  init_Cache
        //                             ------------  Set Size ------------------
        //                              Cache 1                  Cache 2
        //                     index                     index                   index = (CacheSize (Bytes) / Setsize) / Blocksize Bytes 

    Index_Size = Cache_size / (Set_size*Block_size);  
    block_offset = log2(Block_size/4);
    tag_bits = 32 - (log2(Index_Size) + block_offset + 2);
    index_bits = log2(Index_Size);

    Cache = (line**)malloc(sizeof(line*) * Index_Size ); // Malloc Cache row;
    for(int i = 0; i < Index_Size; i++){

        Cache[i] = (line*)malloc( sizeof(line) * Set_size );

        for(int j = 0; j < Set_size; j++){
            Cache[i][j].Data = (char**)malloc( sizeof(char*) * Block_size / 4 ); // Bloack Size = 4Bytes^n  -> sizeof(int) * wordscount = Block_size;
            for(int k = 0; k < Block_size/4; k++) Cache[i][j].Data[k] = (char*)malloc(9);
        }

    }
}

void int_to_Hexa(int data, char* ary){
    int remain;
    int index = 7;
    for(int i = 0; i < 8; i++){
        remain = data % 16;
        data = data / 16;
        switch(remain){
            case 10:{
                ary[index--] = 'a';
                break;
            }
            case 11:{
                ary[index--] = 'b';
                break;
            }

            case 12:{
                ary[index--] = 'c';
                break;
            }
            case 13:{
                ary[index--] = 'd';
                break;
            }
            case 14:{
                ary[index--] = 'e';
                break;
            }
            case 15:{
                ary[index--] = 'f';
                break;
            }
            default:{
                ary[index--] = remain + 48;
                break;
            }
        }
    }
    ary[8] = '\0';
}

void Draw_Cache(char* address_Hexa, char command, int data){
    char address_Bin[33];
    int index, tag, block, byte;
    int Set;
    Hexa_to_Bin(address_Hexa, address_Bin);
    get_Cache_Params(&index, &tag, &block, &byte, address_Bin);
    
    if( Find_Cache(index, tag, block, byte, &Set) ){
        Run_Command(index, tag, block, byte, Set, command, data);
    }
}

int Find_Cache(int index, int tag, int block, int byte, int* Set){
    for(int i = 0; i < Set_size; i++){
        if( (Cache[index][i].Tag == tag ) && (Cache[index][i].Valid == true) ){
            *Set = i;
            return true;
        }
    }
    return false;
}

void Run_Command(int index, int tag, int block, int byte, int Set, char command, int data){
    char Hexa[9];
    if( command == 'W'){
        int_to_Hexa(data,Hexa);
        Cache[index][Set].Dirty = true;
        strcpy(Cache[index][Set].Data[block],Hexa);
        printf("Index = %d, Tag = %d, Block = %d, Byte = %d, Set = %d, Data = %s\n", index, tag, block, byte, Set, Cache[index][Set].Data[block]);
    }
}

void Hexa_to_Bin(char* adress_Hexa, char* address_Bin){
    address_Bin[0] = '\0';

    for(int i = 0; i < 8; i++){
        switch(adress_Hexa[i]){
            case '0': {
                strcat(address_Bin,"0000");
                break;
            }
            case '1': {
                strcat(address_Bin,"0001");
                break;
            }
            case '2': {
                strcat(address_Bin,"0010");
                break;
            }
            case '3': {
                strcat(address_Bin,"0011");
                break;
            }
            case '4': {
                strcat(address_Bin,"0100");
                break;
            }
            case '5': {
                strcat(address_Bin,"0101");
                break;
            }
            case '6': {
                strcat(address_Bin,"0110");
                break;
            }
            case '7': {
                strcat(address_Bin,"0111");
                break;
            }
            case '8': {
                strcat(address_Bin,"1000");
                break;
            }
            case '9': {
                strcat(address_Bin,"1001");
                break;
            }
            case 'A': {
                strcat(address_Bin,"1010");
                break;
            }
            case 'B': {
                strcat(address_Bin,"1011");
                break;
            }
            case 'C': {
                strcat(address_Bin,"1100");
                break;
            }
            case 'D': {
                strcat(address_Bin,"1101");
                break;
            }
            case 'E': {
                strcat(address_Bin,"1110");
                break;
            }
            case 'F': {
                strcat(address_Bin,"1111");
                break;
            }
            default:{
                printf("Hexadecimal Error\n");
                exit(1);
            }
        }
    }
    address_Bin[32] = '\0';
}

void get_Cache_Params(int* index, int* tag, int* block, int* byte, char* adress_Bin){
    *index = 0;
    *tag = 0;
    *block = 0;
    *byte = 0;

    for(int i = 0; i < index_bits; i++){
        *index = (*index * 2) + (adress_Bin[i] - 48);
    }
    for(int i = index_bits; i < index_bits + tag_bits; i++){
        *tag = (*tag * 2) + (adress_Bin[ i ] - 48);
    }
    for(int i = index_bits + tag_bits ; i < index_bits + tag_bits + block_offset ; i++){
        *block = (*block * 2) + (adress_Bin[ i ] - 48);
    }
    for(int i = index_bits + tag_bits + block_offset; i < index_bits + tag_bits + block_offset + bytes_offset ; i++){
        *byte = (*byte * 2) + (adress_Bin[ i ] - 48);
    }

}