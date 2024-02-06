#include <stdlib.h>
#include <stdio.h>

typedef struct virus {
unsigned short SigSize;
char virusName[16];
unsigned char* sig;
} virus; 

void printHex(FILE* input,int length){
    unsigned * buf = malloc(length);
    fread(buf,length,1,input);
    unsigned int  x = buf[0];
    for (int i =0; i<13;i++){
       printf("%x ", buf[i]); 
    }

}

virus* readVirus(FILE* input){
    int len = 100 *sizeof(int);
    char* buf = malloc(len);
    fread(buf,len,1,input);
    
    
    virus* vir = malloc(18);


}
int main(int argc, char **argv){
FILE* fp ;
fp = fopen(argv[1], "r+");
printHex(fp,1000);
fclose(fp);
return 0;
}

