#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "elf.h"

char *getProtectionFlags(int flags);
char *getMappingFlags(int flags);

char *getFlags(int n) {
    int R = 0;
    int  W = 0;
    int  E = 0;
    char *str[50];

    //msb (bit 2) is set R flag to 1.
    if (n / 4 == 1){  
        R = 1;
    }
    n = n % 4;
    // second msb (bit 1) is set, W flag  is set to 1.
    if (n / 2 == 1){
        W = 1;
    }
    //lsb (bit 0) is set, E flag is set to 1.
    if (n % 2 == 1){
        E = 1;
    }
    if (R && W){
        strcpy(str, "RW");
    }
    else if (R){
        strcpy(str, "R");
    }
    else if (W){
        strcpy(str, "W");
    }

    if (E){
        if (R && W){
            strcat(str, "E");
        }
        else if (R || W){
           strcat(str, " E");
        }
        else{
            strcpy(str, "E");
        }
    }

    return strdup(str);
}


char *getType(int type){
    switch (type) {
        case PT_NULL: return "NULL";
        case PT_LOAD: return "LOAD";
        case PT_DYNAMIC: return "DYNAMIC";
        case PT_INTERP: return "INTERP";
        case PT_NOTE: return "NOTE";
        case PT_SHLIB: return "SHLIB";
        case PT_PHDR: return "PHDR";
        case PT_NUM: return "TLS";
        case PT_LOOS: return "LOOS";
        case PT_GNU_EH_FRAME: return "GNU_EH_FRAME";
        case PT_GNU_STACK: return "GNU_STACK";
        case PT_GNU_RELRO: return "GNU_RELRO";
        case PT_LOSUNW: return "LOSUNW";
        case PT_SUNWSTACK: return "PSUNWSTACK";
        case PT_HISUNW: return "HISUNW";
        case PT_LOPROC: return "LOPROC";
        case PT_HIPROC: return "HIPROC";
        default: return "UNKNOWN";
    }
}

   

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg){
    Elf32_Ehdr *Ehdr = (Elf32_Ehdr *)map_start;
    Elf32_Phdr *Phdr = (Elf32_Phdr *)(map_start + Ehdr->e_phoff);

    int EhdrNum = Ehdr->e_phnum;
    printf("Type\t\tOffset\t\tVirtAddr\tPhysAddr\tFileSiz\t\tMemSiz\t\tFlg\tAlign\tProtectionFlags\tMappingFlags\n");
    for (int i = 0; i < EhdrNum; i++){
        func(Phdr, i);
        
    }

    return 0;
}

void phdrPrint(Elf32_Phdr *phdr, int i){
    printf("%s\t", getType(phdr[i].p_type));
    printf("%#08x\t", phdr[i].p_offset);
    printf("%#08x\t", phdr[i].p_vaddr);
    printf("%#08x\t", phdr[i].p_paddr);
    printf("%#08x\t", phdr[i].p_filesz);
    printf("%#08x\t", phdr[i].p_memsz);
    printf("%s\t", getFlags(phdr[i].p_flags));
    printf("%#03x\t", phdr[i].p_align);
    printf("%s\t", getProtectionFlags(phdr[i].p_flags));
    //printf("%s  \n",getMappingFlags(phdr[i].p_flags));
    printf("%s\n", "MAP_FIXED & MAP_PRIVATE");

    
    

       
}

char *getProtectionFlags(int flags) {
    switch (flags) {
        case 1: return "PROT_EXEC";
        case 2: return "PROT_WRITE";
        case 3: return "PROT_EXEC & PROT_WRITE ";
        case 4: return "PROT_READ";
        case 5: return "PROT_READ & PROT_EXEC";
        case 6: return "PROT_READ & PROT_WRITE";
        case 7: return "PROT_READ & PROT_WRITE & PROT_EXEC";
        default: return "UNKNOWN";

    }
}
        
// char *getMappingFlags(int flags) {
//     switch (flags) {
//         case 1: return "MAP_SHARED";
//         case 2: return "MAP_PRIVATE";
//         case 3: return "MAP_FIXED ";
//         case 4: return "MAP_FIXED & MAP_PRIVATE";
//         case 5: return "MAP_FIXED & MAP_SHARED";
//         case 6: return "MAP_PRIVATE & MAP_SHARED";


//         default: return "UNKNOWN";

//     }
// }



int main(int argc, char **argv){
    void *map_start;  
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        fprintf(stderr,"open error");
        exit(1);
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    map_start = mmap(NULL, file_size, PROT_READ | PROT_WRITE | PROT_EXEC , MAP_PRIVATE , fd, 0);

    if (map_start == -1){
        fprintf(stderr,"mmap error");
        exit(1);
    }

    foreach_phdr(map_start, phdrPrint, 0);

    munmap(map_start, file_size);
    return 0;
}