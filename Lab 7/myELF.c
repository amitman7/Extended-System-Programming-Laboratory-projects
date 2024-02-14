#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>
#include <unistd.h>
#include <errno.h>
#include "elf.h"

char file_name1[100];
char file_name2[100];
int fileDescriptor1 = -1;
int fileDescriptor2 = -1;
void* map_start1 = 0;
void* map_start2 = 0; 
int file1Length = 0;
int file2Length = 0;

struct fun_desc {
    char *name;
    void (*fun)();
};

int debug = 0;
int bound;

void debugMode(){
    if (debug == 1)
        fprintf(stderr, "Debug Off\n");
    else
        fprintf(stderr, "Debug On\n");
    debug = 1 - debug;
}

char* getSectionName(int sectionName) {
    switch (sectionName) {
        case 0: return "";
        case 1: return ".text";
        case 2: return ".data";
        case 3: return ".bss";
        default: return "Unknown";
    }
}

char* getSectionType(int type) {
    switch (type) {
        case 0: return "NULL";
        case 1: return "PROGBITS";
        case 2: return "SYMTAB";
        case 3: return "STRTAB";
        case 4: return "RELA";
        case 5: return "HASH";
        case 6: return "DYNAMIC";
        case 7: return "NOTE";
        case 8: return "NOBITS";
        case 9: return "REL";
        case 10: return "SHLIB";
        case 11: return "DYNSYM";
        default: return "Unknown";
    }
}


char* getDataScheme(char data){
    switch (data) {
    case 0:
        fprintf(stderr, "error in encoding\n");
        break;
    case 1: return "little endian";
    case 2: return "big endian";
    default:
        fprintf(stderr, "error in encoding\n");
        exit(1);
    }
}

int checkMagicByte(Elf32_Ehdr * header){
    if (!((header->e_ident[0] == 0x7f)
        && (header->e_ident[1] == 'E')
        && (header->e_ident[2] == 'L')
        && (header->e_ident[3] == 'F'))){
        fprintf(stderr, "error, wrong magic bytes\n");
        return 0;
    }

    return 1;

}


void printTask0(Elf32_Ehdr * header){
    printf("Magic Bytes: ");
    printf("%c ",header->e_ident[1]);
    printf("%c ",header->e_ident[2]);
    printf("%c \n",header->e_ident[3]);
    
    printf("The data encoding scheme of the file: ");
    printf("%s \n",getDataScheme(header->e_ident[5]));

    printf("Entry point: ");
    printf("0x%x \n",header->e_entry);

    printf("The file offset in which the section header table resides: ");
    printf("0x%x \n" , header->e_shoff);
    
    printf("The number of section header entries: ");
    printf("%d \n", header->e_shnum);

    printf("The size of each section header entry: ");
    printf("%d \n" , sizeof(Elf32_Shdr));

    printf("The file offset in which the program header table resides: ");
    printf("0x%x \n" , header->e_phoff);

    printf("The number of program header entries: ");
    printf("%d \n" , header->e_phnum);

    printf("The size of each program header entry: ");
    printf("%x \n" , header->e_phentsize);
    printf("\n");

}

void examineElfFile() {
    FILE* input;
    Elf32_Ehdr *header;
    int size;
    int fileDescriptor;
    int currentLength;
    void *currentMap;
    int currentFileDescriptor;
    char fileName[100]; //it was 50 by the king basuty

    // which file to open
    if ((fileDescriptor1 != -1)
        && (fileDescriptor2 != -1)){
        fileDescriptor = -1;
    }
    else if (fileDescriptor1 == -1){
        fileDescriptor = 1;
    }
    else{
        fileDescriptor = 2;
    }
    if (fileDescriptor == -1){
        fprintf(stderr,"Too many files are open\n");
        return;
    }

    if(fileDescriptor1 != -1){
        if (close(fileDescriptor1) == -1){
                fprintf(stderr,"error in close\n");
                exit(1);
        }
    }

    if(fileDescriptor == 1){
        currentMap = map_start1;
    }
    else {
        currentMap = map_start2;
    }

    printf("Enter the File Name:\n");
    fgets(fileName, sizeof(fileName), stdin);
    //removing '\n'
    fileName[strlen(fileName)-1] = 0;
    if(fileDescriptor == 1){
        strcpy(file_name1,fileName);
    }
    else{
        strcpy(file_name2,fileName);
    }

    printf("File Name: %s\n",fileName);

    //Opening the file
    input = fopen(fileName, "r");
    if(input == NULL){
        if(fileDescriptor == 1){
            fileDescriptor1 = -1;
        }
        else{
            fileDescriptor2 = -1;
        }
        close(fileDescriptor);
        fprintf(stderr,"error in fopen");
        exit(1);

    }
    currentFileDescriptor = fileno(input);

    if(fileDescriptor == 1){
        fileDescriptor1 = currentFileDescriptor;
    }
    else{
        fileDescriptor2 = currentFileDescriptor;
    }

    fseek(input, 0, 2); 
    currentLength = ftell(input); //get current file pointer
    fseek(input, 0, 0); //seek back to beginning of file
    if(fileDescriptor == 1){
        file1Length = currentLength;
    }
    else{
        file2Length = currentLength;
    }

    if ((currentMap = mmap(0, currentLength, PROT_READ, MAP_SHARED, currentFileDescriptor, 0)) == -1) {
        fprintf(stderr,"error in mmap");
        if(fileDescriptor == 1) fileDescriptor1 = -1;
        else fileDescriptor2 = -1;
        close(fileDescriptor);
        exit(1);
    }
    if(fileDescriptor == 1) map_start1 = currentMap;
    else map_start2 = currentMap;

    header = currentMap;
 
    if(checkMagicByte(header) == 0){
        if(fileDescriptor == 1) fileDescriptor1 = -1;
        else fileDescriptor2 = -1;      
        munmap(currentMap, currentLength);
        close(currentFileDescriptor);
    }
    printTask0(header);
}


void printSectionNames() {
    Elf32_Ehdr *header = map_start1;
    Elf32_Ehdr *header2 = map_start2;

    if(fileDescriptor1 != -1)
        printSectionHelper(header, file_name1);

    if(fileDescriptor2 != -1)
        printSectionHelper(header2, file_name2);
}

void printSectionHelper(Elf32_Ehdr *header, char* file_name) {
    Elf32_Shdr* shdr = (Elf32_Shdr*)((char*)header + header->e_shoff);
    Elf32_Shdr* stringTab = &shdr[header->e_shstrndx];
    char* strtab_p = (char*)header + stringTab->sh_offset;

    printf("File  %s\n", file_name);
    for (int i = 0; i < header->e_shnum; i++) {
        printf("[%d]", i);
        printf("%s ", strtab_p + shdr[i].sh_name);
        printf("%08x ", shdr[i].sh_addr);
        printf("%08x ", shdr[i].sh_offset);
        printf("%08x ", shdr[i].sh_size);
        printf("%08x \n", shdr[i].sh_type);
    }
}

void printSymbols()
{
    Elf32_Ehdr *header;
    Elf32_Shdr* strtabSection;
    if (fileDescriptor1 != -1){
        header = map_start1;
        Elf32_Shdr* sections = (Elf32_Shdr*)(header->e_shoff + map_start1);
        char * sectionNames = (char *)(map_start1 + sections[header->e_shstrndx].sh_offset);
        printSymbolsHelper(header->e_shnum, header, sectionNames, sections, strtabSection, map_start1, file_name1);
    }
    if (fileDescriptor2 != -1) {
        header = map_start2;
        Elf32_Shdr* sections = (Elf32_Shdr*)(header->e_shoff + map_start2);
        char * sectionNames = (char *)(map_start2 + sections[header->e_shstrndx].sh_offset);
        printSymbolsHelper(header->e_shnum, header, sectionNames, sections, strtabSection, map_start2, file_name2);
    }
}

void printSymbolsHelper(int numOfHeaders, Elf32_Ehdr *header, char * sectionNames, Elf32_Shdr* sections, Elf32_Shdr* strtabSection, void* map_start1, char* file_name){
    printf("File %s\n", file_name);
    printf("[index] value section_index section_name symbol_name\n");
    for (int i = 0; i < numOfHeaders; i++) {
        Elf32_Shdr* sectionHeader = (Elf32_Shdr *)(header->e_shoff + map_start1 + (i * header->e_shentsize));
        if(sectionHeader->sh_type == 3){
            strtabSection = sectionHeader;
        }
    }
    for (int i = 0; i < numOfHeaders; i++)
    {
        Elf32_Shdr * sectionHeader = (Elf32_Shdr *)(header->e_shoff + map_start1 + (i * header->e_shentsize));
        if ((sectionHeader->sh_type == SHT_SYMTAB) || (sectionHeader->sh_type == SHT_DYNSYM))
        {
            Elf32_Off sh_offset = sectionHeader->sh_offset;
            Elf32_Sym * symTable = (Elf32_Sym *) (map_start1 + sh_offset);
            int numOfEntries = sectionHeader->sh_size/sectionHeader->sh_entsize;
            for (int j = 0; j < numOfEntries; j++){
                const char* section_name;
                if (symTable[j].st_shndx == SHN_UNDEF)
                    section_name = "UND";
                else if (symTable[j].st_shndx == SHN_ABS)
                    section_name = "ABS";
                else
                    section_name = (char *) &sectionNames[sections[symTable[j].st_shndx].sh_name];
                printf("[%d]    ",j);
                printf("%x            ", symTable[j].st_value);
                printf("%d            ", symTable[j].st_shndx);
                printf(section_name);
                printf("            %s \n",(char *) (map_start1 + strtabSection->sh_offset + symTable[j].st_name));
            }
        }
    }
}

Elf32_Sym* getSymbol(Elf32_Sym* symbol, int size, char* strtab, char* name){
    for(int i = 0; i < size; i++){
        if(strcmp(name, &strtab[symbol[i].st_name]) == 0){
            return &symbol[i];
        }
    }
    return NULL;
}

Elf32_Shdr* getSection(Elf32_Shdr* section, int size, char* strtab, char* name){
    for(int i = 0; i < size; i++){
        if(strcmp(name, &strtab[section[i].sh_name]) == 0){
            return &section[i];
        }
    }
    return NULL;
}

void checkMergeHelper(Elf32_Sym* symbol, Elf32_Sym* symbol1, int i, char* tab1){
    if(symbol1[i].st_shndx == SHN_UNDEF){
            if(symbol == NULL || symbol->st_shndx == SHN_UNDEF){
                printf("Sym %s is undefined!\n", &tab1[symbol1[i].st_name]);
            } 
        } else {
            if(symbol != NULL && symbol->st_shndx != SHN_UNDEF){
                printf("Sym %s is multiply defined!\n", &tab1[symbol1[i].st_name]);
            }
        }
}

void checkMerge(){
    if (fileDescriptor1 == -1 || fileDescriptor2 == -1){
        fprintf(stderr, "Two files must be loaded!\n");
        return 1;
    }
    int size1, size2;
    char* strtab1;
    char* strtab2;
    Elf32_Ehdr* header1 = map_start1;
    Elf32_Ehdr* header2 = map_start2;
    Elf32_Sym* symbol1;
    Elf32_Sym* symbol2;
    Elf32_Shdr* Shdr1 = (Elf32_Shdr*) (map_start1 + header1->e_shoff);
    Elf32_Shdr* Shdr2 = (Elf32_Shdr*) (map_start2 + header2->e_shoff);
    
    for(int i = 0; i < header1->e_shnum; i++){
        if(Shdr1[i].sh_type == SHT_SYMTAB || Shdr1[i].sh_type == SHT_DYNSYM){
            size1 = Shdr1[i].sh_size / Shdr1[i].sh_entsize;
            symbol1 = (Elf32_Sym*)(map_start1 + Shdr1[i].sh_offset);
            strtab1 = (char*)(map_start1 + Shdr1[Shdr1[i].sh_link].sh_offset);
        }
    }
    for(int i = 0; i < header2->e_shnum; i++){
        if(Shdr2[i].sh_type == SHT_SYMTAB || Shdr2[i].sh_type == SHT_DYNSYM){
            size2 = Shdr2[i].sh_size / Shdr2[i].sh_entsize;
            symbol2 = (Elf32_Sym*)(map_start2 + Shdr2[i].sh_offset);
            strtab2 = (char*)(map_start2 + Shdr2[Shdr2[i].sh_link].sh_offset);
        }
    }
    for(int i = 1; i < size1; i++){
        Elf32_Sym* symbol = getSymbol(symbol2, size2, strtab2, &strtab1[symbol1[i].st_name]);
        checkMergeHelper(symbol, symbol1, i, strtab1);
    }
    for(int i = 1; i < size2; i++){
        Elf32_Sym* symbol = getSymbol(symbol1, size1, strtab1, &strtab2[symbol2[i].st_name]);
        checkMergeHelper(symbol, symbol2, i, strtab2);
    }
}


// we got a tiny bit of help from chatgpt with this function
void mergeFiles() {
    if (fileDescriptor1 == -1 || fileDescriptor2 == -1){
        fprintf(stderr, "Two files must be loaded!\n");
        return 1;
    }
    Elf32_Ehdr *head1 = map_start1;
    Elf32_Ehdr *head2 = map_start2;
    Elf32_Ehdr newHeader = *head1;
    size_t size1 = head1->e_shentsize * head1->e_shnum;
    Elf32_Shdr newSections[head1->e_shnum];
    Elf32_Shdr* header1 = ((char*)map_start1 + head1->e_shoff);
    FILE* output = fopen("out.ro", "wb");
    fwrite(&newHeader, sizeof(Elf32_Ehdr), 1, output);
    memcpy(newSections, header1, size1);
    
    for (int i = 0; i < head1->e_shnum; i++) {
        Elf32_Shdr* currentSection = &newSections[i];
        char* name = (char*)map_start1 + header1[head1->e_shstrndx].sh_offset + currentSection->sh_name;
        fseek(output, 0, SEEK_END);
        size_t section_offset = ftell(output);
        size_t section_size = currentSection->sh_size;
        fwrite((char*)map_start1 + currentSection->sh_offset, section_size, 1, output);

        if (strcmp(name, ".text") == 0 || strcmp(name, ".data") == 0 || strcmp(name, ".rodata") == 0) {
            Elf32_Shdr* currentSections2 = (Elf32_Shdr*)((char*)map_start2 + head2->e_shoff);
    
            for (int j = 1; j < head2->e_shnum; j++) {
                Elf32_Shdr* currentSection2 = &currentSections2[j];
                char* name2 = (char*)map_start2 + currentSections2[head2->e_shstrndx].sh_offset + currentSection2->sh_name;
                    if (strcmp(name,name2) == 0) {
                        fseek(output, 0, SEEK_END);
                        fwrite((char*)map_start2 + currentSection2->sh_offset, currentSection2->sh_size, 1, output);
                        section_size += currentSection2->sh_size;
                        break;
                    }
            }
        }
        currentSection->sh_offset = section_offset;
        currentSection->sh_size = section_size;
    }
    int offset = ftell(output);
    fwrite((char*)newSections, 1, head1->e_shnum * head1->e_shentsize, output);
    fseek(output, 32, SEEK_SET);
    fwrite((char*)(&offset), 1, sizeof(int), output);
    fclose(output);
}


void quit() {
    if(fileDescriptor1 != -1){
        munmap(map_start1, file1Length);
        close(fileDescriptor1);
    }
    if(fileDescriptor2 != -1){
        munmap(map_start2, file2Length);
        close(fileDescriptor2);
    }
    exit(0); 
}

int getBound(struct fun_desc * menu) {
    int bound = 1;
    int j = 0;
    while (menu[j].name != NULL)
    {
        j++;
        bound = j;
    }
    return bound;
}


int main(int argc, char **argv) {

    struct fun_desc menu[] = {
        { "Toggle Debug Mode", &debugMode },
        { "Examine ELF File", &examineElfFile },
        { "Print Section Names", &printSectionNames},
        { "Print Symbols", &printSymbols},
        { "Check Files for Merge", &checkMerge},
        { "Merge ELF Files", &mergeFiles},
        { "quit", &quit},
        { NULL, NULL }
    };

    int j = 0;
    while (menu[j].name != NULL){
        j++;
        bound = j;
    }
    
    while(1){
        char input[100];
        int option;
        printf("Choose option:\n");
        int i = 0;
        while (menu[i].fun != NULL){
            printf("%d. %s\n", i, menu[i].name);
            i++;
        }
        fgets(input,100,stdin);
        sscanf(input, "%d\n", &option);

        if((option < 0) || (bound <= option)){
            fprintf(stderr, "Not within bounds\n");
            exit(1);
        }
        menu[option].fun();
    }

    return 0;
}

