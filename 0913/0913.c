#define CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct
typedef struct CalculatorNode {
    int a, b;
    char name[20];
    int (*fptr) (int, int);
    struct CalculatorNode * next;
}CalculatorNode;
typedef CalculatorNode* CalNode;
CalNode Head = NULL;
CalNode Tail = NULL;

// Cal Fuc
int add(int a, int b);
int sub(int a, int b);
int mul(int a, int b);
int divi(int a, int b);

// Node Func
void makenode(int a, int b, char name[]);

// Menu Function
void option1();
void option2();
void option3();
void option4();

int main(){

    int inputa, inputb;
    int index = 0;
    char inputname[20];

    do{
        printf("----------------------------------------------------\n");
        printf("1. 계산 노드 생성 ( 'add', 'sub', 'mult', 'div' )\n");
        printf("2. 현재까지 생성된 계산 노드 출력\n");
        printf("3. 현재까지 생성된 계산 노드들의 계산 결과를 출력\n");
        printf("4. 전체삭제\n");
        printf("5. 프로그램 종료\n");
        printf("----------------------------------------------------\n");
        printf("Index : ");
        scanf("%d", & index);
        printf("\n\n");
        switch(index){
            case 1 : {
                option1();
                break;
            }
            case 2: {
                option2();
                break;
            }
            case 3: {
                option3();
                break;
            }
            case 4 :{
                option4();
                break;
            }
            case 5 : {
                break;
            }
            default : {
                printf("Wrong index\n\n");
            }
        }
        
    }while(index != 5);

    return 0;
}


// Cal Function
int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }
int divi(int a, int b){ return a / b; }


void makenode(int a, int b, char name[]){
    CalNode newNode = (CalNode)malloc(sizeof(CalculatorNode));
    newNode -> a = a;
    newNode -> b = b;
    strcpy(newNode -> name ,name);
    newNode -> next = NULL;

    if( !strcmp(newNode->name,"add") ) (newNode -> fptr) = add;
    else if( !strcmp(newNode->name,"sub") ) (newNode ->fptr) = sub;
    else if( !strcmp(newNode->name,"mult") ) (newNode ->fptr) = mul;
    else (newNode ->fptr) = divi;

    if(Head == NULL){
        Head = newNode;
    }
    else{    
        CalNode ptr = Head;
        while( (ptr -> next) != NULL ) ptr = ptr ->next;
        ptr -> next = newNode;
    }
}


void option1(){
    int a, b;
    char operator[30];
    

    printf("Menu 1 \n");
    printf("----------------------------------------------------\n");
    printf("Enter A & B : ");
    scanf("%d %d",&a,&b);
    printf("Enter Operator ( 'add', 'sub', 'mult', 'div' ) : ");
    scanf("%s", operator);
    printf("∏----------------------------------------------------\n");

    printf("\n\n");
    makenode(a, b, operator);

}

void option2(){
    CalNode ptr = Head;
    
    printf("Menu 2 \n");
    printf("----------------------------------------------------\n");
    printf("%-5s %-5s %-10s\n","A","B","Operator");
    while( ptr ){
        printf("%-5d %-5d %-6s\n", ptr->a, ptr->b, ptr ->name);
        ptr = ptr -> next;
    }
    printf("----------------------------------------------------\n");
    printf("\n\n");
}

void option3(){
    CalNode ptr = Head;

    printf("Menu 3 \n");
    printf("----------------------------------------------------\n");
    printf("%-5s %-5s %-10s %-5s\n","A","B","Operator", "Result");
    while( ptr ){
        printf("%-5d %-5d %-10s %-5d\n", ptr->a, ptr->b, ptr ->name, (ptr -> fptr(ptr ->a, ptr -> b )) );
        ptr = ptr -> next;
    }
    printf("----------------------------------------------------\n");
    printf("\n\n");
}
void option4(){
    CalNode ptr = Head;
    
    printf("Menu 4\n");
    printf("----------------------------------------------------\n");
    printf("Delete All\n\n");
    while( Head ){
        Head = ptr -> next;
        free(ptr);
        ptr = Head;
    }    
}