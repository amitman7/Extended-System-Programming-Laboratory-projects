#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "elf.h"

char *getProtectionFlags(int flags);
char *getMappingFlags(int flags);
void phdrPrint(Elf32_Phdr *phdr, int i);


void load_phdr(Elf32_Phdr *phdr, int fd){
    int padding = 0xfffff000;// pading the adrress
    int phdrOffset = phdr->p_offset & padding; // offset of the program header
    void *phdrVaddr = phdr->p_vaddr & padding; // virtual address of the program header
    
    if(phdr->p_type == PT_LOAD){
        mmap(phdrVaddr, phdr->p_memsz+phdr->p_vaddr & padding,phdr->p_flags,MAP_PRIVATE | MAP_FIXED,fd, phdrOffset);
    }
    phdrPrint(phdr, 0);
    
}

char *getFlags(int n) {
    switch (n){
    case 0:
        return "   ";
        break;
    case 1:
        return "  E";
        break;
    case 2:
        return " W ";
        break;
    case 3:
        return " WE";
        break;
    case 4:
        return "R";
        break;
    case 5:
        return "R E";
        break;
    case 6:
        return "RW ";
        break;
    case 7:
        return "RWE";
        break;
    default:
        break;
    }
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
   

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *,int), int arg){
    Elf32_Ehdr *header =  map_start;
    Elf32_Phdr *phdr =  map_start + header->e_phoff;
    
    printf("Type\tOffset\t\tVirtAddr\tPhysAddr\tFileSiz\t\tMemSiz\t\tFlg\tAlign\tProtectionFlags\t\tMappingFlags\n");
    int size = header->e_phnum;
    for (int i=0; i<size; i++){
        func(&phdr[i], arg);
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
    printf("%s\t\t", getProtectionFlags(phdr[i].p_flags));
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
    
    int fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        fprintf(stderr,"open error");
        return 1;
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    map_start = mmap(0, file_size, PROT_READ | PROT_WRITE | PROT_EXEC , MAP_PRIVATE , fd, 0);
    if (map_start == -1){
        fprintf(stderr,"mmap error");
        return 1;
    }
   
    foreach_phdr(map_start,load_phdr, fd);
    Elf32_Ehdr *map_startElf = map_start; 

    startup(argc, argv, map_startElf->e_entry);
    munmap(map_start, file_size);
    
    return 0;
}



