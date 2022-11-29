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
    // DATA
    //        [ 1word ]   -> 4byte => 32bits
    // Block  [ 1word ]
    //        [ 1word ]
    //        [ 1word ]
}line;
line** Cache = NULL;
// Use Circular Queue to control last Infromation
int* Fastest_index;
int* Lastest_index;

typedef struct mm{
    // Adress
    int index;
    int Tag;
    int Block;
    int byte;
    char data[9];
}MM;
MM* MEM = NULL;
// Main Memory => realloc when MEM is FULL
int MEM_Size = 4;
int MEM_Last = -1;

int Index_Size = 0;
int Block_size = 0;
int Set_size = 0;

int Index_bits = 0;
int Tag_bits = 0;
int Block_offset = 0;
int Bytes_offset = 2;
// Cache_Size = Index_Size * Block_Size * Set_Size  bits

// Memmory Access ---------------------------------------------------------
//                                              (Byteoffset = log_2(Block_size))


int miss = 0;
int hit = 0;
int cycle = 0;

void init_Cache(int argc, char* args[], FILE** fpp);
void int_to_Hexa(int, char*); // Data (int) to Hexa
void Hexa_to_Bin(char*, char*); // Adress(Hexa) to Binary
void get_Cache_Params(int*, int*, int*, int*, char*);  // Index, Tag, Block, Byte

void Draw_Cache(char*, char, int);
int Find_Cache(int, int, int, int, int*);
void Run_Command(int, int, int, int, int, char, int);
void Add_MEM(int, int, int, int, char*);
void Update_Cache(int, int, int, int);
void Update_MEM(int, int, int, int);
int Print_Cache();

int main(int argc, char* args[]){
    FILE *fp = NULL;
    char adress_Hexa[9]; // Max Address = 2^32 bits = 16^8 bits
    char data_Hexa[9];
    char data_Bin[33];
    char command;
    int data;
    int count = 0;
    int dirty;

    init_Cache(argc, args, &fp);

    while(1){
        fscanf(fp,"%s %c",adress_Hexa, &command);
        if(feof(fp)) break;
        if(command == 'W') fscanf(fp, "%d", &data);
        Draw_Cache(adress_Hexa, command, data);
        command = '0'; 
        count++;
    }
    for(int i = 0; i < argc ; i++){
        printf("%s ", args[i]);
    }
    printf("\n");
    dirty = Print_Cache();
    printf("total number of hits: %d\n",hit);
    printf("total number of misses: %d\n", miss);
    printf("miss rate: %.1lf%%\n", (double)miss / (hit + miss)*100);
    printf("total number of dirty blocks: %d\n", dirty);
    printf("average memory access cycle: %.1lf\n", (double)cycle/count);
    fclose(fp);
    return 0;
}

void init_Cache(int argc, char* args[], FILE** fpp){

    int Cache_size = 0;

    for(int i=1; i < argc; i++){

        // Get Paramater
        if( strstr(args[i], "-s=")){
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
            *fpp = fopen(args[i]+3,"r,css=UTF-8");
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
    Block_offset = log2(Block_size/4);
    Tag_bits = 32 - (log2(Index_Size) + Block_offset + 2);
    Index_bits = log2(Index_Size);

    Cache = (line**)malloc(sizeof(line*) * Index_Size ); // Malloc Cache row;
    Fastest_index = (int*)malloc(sizeof(int) * Index_Size);
    Lastest_index = (int*)malloc(sizeof(int) * Index_Size);
    MEM = (MM*)malloc(sizeof(MM) * MEM_Size);
    for(int i = 0; i < Index_Size; i++){

        Cache[i] = (line*)malloc( sizeof(line) * Set_size );
        Fastest_index[i] = -1;

        for(int j = 0; j < Set_size; j++){
            Cache[i][j].Data = (char**)malloc( sizeof(char*) * Block_size / 4 ); // Bloack Size = 4Bytes^n  -> sizeof(int) * wordscount = Block_size;
            for(int k = 0; k < Block_size/4; k++){
                Cache[i][j].Data[k] = (char*)malloc(9);
                strcpy(Cache[i][j].Data[k],"00000000");
            }
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
                ary[index--] = 'A';
                break;
            }
            case 11:{
                ary[index--] = 'B';
                break;
            }

            case 12:{
                ary[index--] = 'C';
                break;
            }
            case 13:{
                ary[index--] = 'D';
                break;
            }
            case 14:{
                ary[index--] = 'E';
                break;
            }
            case 15:{
                ary[index--] = 'F';
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
    int Set = 0;
    Hexa_to_Bin(address_Hexa, address_Bin);
    get_Cache_Params(&index, &tag, &block, &byte, address_Bin);
    
    if( Find_Cache(index, tag, block, byte, &Set) ){
        hit++;
        cycle++;
        Run_Command(index, tag, block, byte, Set, command, data);
    }
    else{
        // miss ++;
        // Access Cycle + 200;
        // is FULL?? Yes -> add MEM, and get from MEM
        // if Dirty is false -> get from MEM;
        // if( (Cache[index][ Fastest_index[index]].Valid == 1) && ( Cache[index][ Fastest_index[index] ].Dirty == 1)  ){
        if( Fastest_index[index] == Lastest_index[index]){
            
            if( (Cache[index][ Fastest_index[index]].Valid == 1) && ( Cache[index][ Fastest_index[index] ].Dirty == 1)  )cycle += 401;
            else cycle += 201;
            Update_MEM(index, tag, block, byte);
            Update_Cache(index, tag, block, byte);
            miss += 1;
        }
        else{
            Update_Cache(index, tag, block, byte);
            miss += 1;
            cycle += 201;
        }
        if(command == 'W' && Block_size == 4) cycle -=200;
        Find_Cache(index, tag, block, byte, &Set);
        Run_Command(index, tag, block, byte, Set, command, data);
    }
}

int Find_Cache(int index, int tag, int block, int byte, int* Set){
    // if find in Cache / return true;
    for(int i = 0; i < Set_size; i++){
        if( (Cache[index][i].Tag == tag ) && (Cache[index][i].Valid == true) ){
            *Set = i;
            return true;
        }
    }
    return false;
}

void Run_Command(int index, int tag, int block, int byte, int Set, char command, int data){
    // if Command is Write Dirty = 0 
    char Hexa[9];
    if( command == 'W'){
        int_to_Hexa(data, Hexa);
        Cache[index][Set].Dirty = true;
        strcpy(Cache[index][Set].Data[block], Hexa);
    }

    //printf("Index = %d, Tag = %d, Block = %d, Byte = %d, Set = %d, Data = %s\n", index, tag, block, byte, Set, Cache[index][Set].Data[block]);
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

    for(int i = 0; i < Tag_bits; i++){
        *tag = (*tag * 2) + (adress_Bin[i] - 48);
    }
    for(int i = Tag_bits; i < Index_bits + Tag_bits; i++){
        *index = (*index * 2) + (adress_Bin[ i ] - 48);
    }
    for(int i = Index_bits + Tag_bits ; i < Index_bits + Tag_bits + Block_offset ; i++){
        *block = (*block * 2) + (adress_Bin[ i ] - 48);
    }
    for(int i = Index_bits + Tag_bits + Block_offset; i < Index_bits + Tag_bits + Block_offset + Bytes_offset ; i++){
        *byte = (*byte * 2) + (adress_Bin[ i ] - 48);
    }

}


void Update_Cache(int index, int tag, int block, int byte){
    //Updated Block
    bool* BLK = (bool*)malloc(sizeof(bool) * (Block_size/4) );
    int i = 0;

    for(i = 0; i < MEM_Last+1; i++){ // MEM Search
        if( (MEM[i].index == index) && (MEM[i].Tag == tag)){ // if Index and Tag Match
            for(int j = 0; j < Block_size / 4; j++){ // Search Block
                if ( MEM[i].Block == block + j ){ // If Matched all

                    Cache[index][ Lastest_index[index] ].Tag = tag; // Update Fastest Index Tag
                    Cache[index][ Lastest_index[index] ].Valid = true; // Valid = true
                    Cache[index][ Lastest_index[index] ].Dirty = false; // Dirty = 0 didn't update
                    Cache[index][ Lastest_index[index] ].Data[j][0] = '\0';
                    strcpy( Cache[index][ Lastest_index[index] ].Data[j], MEM[i].data); // Update Data;
                    BLK[j] = true;

                }
            }
        }
    }

    for(i = 0; i < Block_size / 4 ; i++){
        if(!BLK[i]){ // if Didn't Updated -> 
            Cache[index][ Lastest_index[index] ].Tag = tag; // Update Fastest Index Tag
            Cache[index][ Lastest_index[index] ].Valid = true; // Valid = true
            Cache[index][ Lastest_index[index] ].Dirty = false; // Dirty = 0 didn't update
            strcpy( Cache[index][ Lastest_index[index] ].Data[i], "00000000"); // Update Data;
            BLK[i] = true;
        }
    }
    Lastest_index[index] = (Lastest_index[index] + 1) % (Set_size);
    if(Fastest_index[index] == -1) Fastest_index[index] = 0;
    free(BLK);
}

void Update_MEM(int index, int tag, int block, int byte){
    bool* BLK = (bool*)malloc(sizeof(bool) * (Block_size/4) );

    for(int i = 0; i < MEM_Last; i++){ // MEM Search

        if( (MEM[i].index == index) && (MEM[i].Tag == Cache[index][ Fastest_index[index]].Tag) ){ // if Index and Tag Match

            for(int j = 0; j < Block_size / 4; j++){ // Search Block

                if ( MEM[i].Block == block + j ){ // If Matched all
                    MEM[i].data[0] = '\0';
                    strcat( MEM[i].data, Cache[index][ Fastest_index[index] ].Data[j] ); // Update Data;
                    BLK[j] = true;
                }
            }
        }
    }
    if(MEM_Size == MEM_Last){
        MEM_Size *= 2;
        MM* temp;
        temp = (MM*)realloc(MEM, sizeof(MM)* MEM_Size);
        MEM = temp;
    }
    for(int i = 0; i < Block_size/4; i++){
        if(MEM_Size == ++MEM_Last){
                MEM_Size *= 2;
                MM* temp;
                temp = (MM*)realloc(MEM, sizeof(MM)* MEM_Size);
                MEM = temp;
            }
        if(!BLK[i]){
            MEM[MEM_Last].index = index;
            MEM[MEM_Last].Tag = Cache[index][Fastest_index[index]].Tag;
            MEM[MEM_Last].Block = i;
            strcpy( MEM[MEM_Last].data, Cache[index][Fastest_index[index]].Data[i]); // Update Data;
            
        }
    }
    Fastest_index[index] = (Fastest_index[index] + 1) % (Set_size);
    free(BLK);
}

int Print_Cache(){
    int dirty = 0;
    for(int i = 0; i < Index_Size; i++){
        printf("%d:",i); // print index

        for(int j = 0; j < Set_size; j++){
            for(int k = 0; k < Block_size/4; k++){
                printf(" %s", Cache[i][j].Data[k]);
            }
            printf(" v:%d", Cache[i][j].Valid);
            if(j == Set_size-1) printf(" d:%d\n", Cache[i][j].Dirty);
            else printf(" d:%d\n  ", Cache[i][j].Dirty);
            if(Cache[i][j].Dirty) dirty++;
        }
    }
    printf("\n");
    return dirty;
}