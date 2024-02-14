
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char* dec_formats[] = {"%hhu\n", "%hu\n", "No such unit", "%u\n"};
static char* hex_formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};


typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
  char display_mode;

  /*
   .
   .
   Any additional fields you deem necessary
  */
} state;


struct fun_desc {
    char *name;
    void (*fun)(state*)
    };

char* map(char *array, int array_length, void (*f) (state*)){////// made some changes here!!!
  for (int i=0;i<array_length;i++){
     (*f)(&array[i]);
  }
  return array;
}



void Toggle_Debug_Mode(state* s){
    if (s->debug_mode == 0) {
        s->debug_mode = 1;
        printf("Debug flag now on\n");
    }
    else {
        s->debug_mode = 0;
        printf("Debug flag now off\n");
    }
}
void Set_File_Name(state* s){
    printf("enter the file name : ");
    fgets(s->file_name, 100, stdin);
    // Remove newline character from the file name
    s->file_name[strcspn(s->file_name, "\n")] = '\0';
    if (s->debug_mode ==1){
       printf("Debug: file name set to: %s", s->file_name);  
    }
}

void Set_Unit_Size(state* s){
    printf("enter unit size : ");
    char in[10];
     fgets(in, 10, stdin);
     int value =atoi(in);
    if (value == 1 || value == 2 || value == 4){
        s->unit_size = value;
        if (s->debug_mode ==1){
            printf("Debug: file name set to: %d\n", value);  
        }
    }

    else{
        if (s->debug_mode ==1){
            fprintf(stderr, "Debug: unvalid unit value\n");  
        }
    }
   
}
void Load_Into_Memory(state* s){
    if (strcmp(s->file_name, "") == 0){
       fprintf(stderr, "there is no name to the file\n");  
       return ;
    }
    FILE* file = fopen(s->file_name, "r");
    if (file == NULL) {
        printf("Error: Failed to open file '%s'.\n", s->file_name);
        return ;
    }

    char input[100];
    printf("Enter the location in hexadecimal and length in decimal: ");
    fgets(input, 100, stdin);

     int location;
     int length;
    input[strcspn(input, "\n")] = '\0';

    int num_matched = sscanf(input, "%x %d", &location, &length);
    if (num_matched != 2) {
        fprintf(stderr, "Error: Invalid input format.\n");
        fclose(file);
        return;
    }

    if (s->debug_mode == 1){
        printf("file name is: %s\n",s->file_name);  
        printf("file loaction is: %x\n",location);
        printf("file length is: %d\n",length);
    }
    
    fseek(file, location * s->unit_size, SEEK_SET);
    size_t num_read = fread(s->mem_buf, s->unit_size, length, file);
    if (num_read != length) {
        fprintf(stderr, "Error: Failed to read data from file.\n");
        fclose(file);
        return;
    }
    s->mem_count = num_read;

    printf("Loaded %d units into memory\n", s->mem_count);

    
}
void Toggle_Display_Mode(state* s){
    if (s->display_mode == 0) {
        s->display_mode = 1;
        printf("Display flag now on, hexadecimal representation\n");
    }
    else {
        s->display_mode = 0;
        printf("Display flag now off, decimal representation\n");
    }
}

void Memory_Display(state* s){
    char input[100];
    printf("Enter address and length: ");
    fgets(input, 100, stdin);

     int address;
     int length;

    int num_matched = sscanf(input, "%x %u", &address, &length);
    if (num_matched != 2) {
        fprintf(stderr, "Error: Invalid input format.\n");
        return;
    }
     char* start_addr;
    if (address == 0) {
        start_addr = s->mem_buf;
    } 
    else {
        start_addr = (char*)(address * s->unit_size);
    }

    if (s->display_mode == 1){
        printf("Hexadecimal\n");
        printf("=============================\n");
    }
    else{
        printf("Decimal\n");
        printf("=============================\n");

    }
for (unsigned int i = 0; i < length; i++) {
        unsigned char* current_address = start_addr + (i * s->unit_size);
        unsigned int value = 0;

        // Read the value based on the unit size
        if (s->unit_size == 1) {
            value = *current_address;
        }
        else if (s->unit_size == 2) {
            value = *((unsigned short*)current_address);
        }
        else if (s->unit_size == 4) {
            value = *((unsigned int*)current_address);
        }
        // Print the value based on the display mode
        if (s->display_mode == 1) {
            printf(hex_formats[s->unit_size - 1], value);
        }
        else {
            printf(dec_formats[s->unit_size - 1], value);
        }
    }
}
void Save_Into_File(state* s){
    if (strcmp(s->file_name, "") == 0) {
        fprintf(stderr, "there is no name to the file\n");
        return;
    }
    FILE* file = fopen(s->file_name, "r+b");
    if (file == NULL) {
        printf("Error: Failed to open file '%s'.\n", s->file_name);
        return;
    }

    char input[100];
    printf("Enter the source address in hexadecimal, the target location in hexadecimal, and the length in decimal: ");
    fgets(input, 100, stdin);

    int source_address;
    int target_location;
    int length;

    int num_matched = sscanf(input, "%x %x %d", &source_address, &target_location, &length);
    if (num_matched != 3) {
        fprintf(stderr, "Error: Invalid input format.\n");
        fclose(file);
        return;
    }
    if (s->debug_mode == 1) {
        printf("Source address: %x\n", source_address);
        printf("Target location: %x\n", target_location);
        printf("Length: %u\n", length);
    }
     fseek(file, 0, SEEK_END);
     int size = ftell(file);
     if (target_location >= size){
         printf("error: targer cnat be bigger ");
         fclose(file);
         return;  
     }
     
     unsigned char* ptr;
     if (source_address == 0){
         ptr = (unsigned char*)s->mem_buf;
     }
     else{
        ptr = s->mem_buf + (source_address * s->unit_size);
     }

    fseek(file, target_location, SEEK_SET);

    fwrite(ptr,s->unit_size,length,file);
    fclose(file);

}

    

void Memory_Modify(state* s){

    char input[100];
    printf("Enter loaction in hexadecimal and the value in hexadecimal: ");
    fgets(input, 100, stdin);

    int location;
    int value;

    int num_matched = sscanf(input, "%x %x", &location, &value);
    if (num_matched != 2) {
        fprintf(stderr, "Error: Invalid input format.\n");
        return;
    }

    if (s->debug_mode == 1) {
        printf("loaction: %x\n",location);
        printf("value : %x\n", value);
    }

    if (location < 0 || location >= s->mem_count) {
        fprintf(stderr, "Error: Invalid location.\n");
        return;
    }
    // Modify the memory buffer at the specified location with the given value
    unsigned char* mem_location = &(s->mem_buf[location]);
    memcpy(mem_location, &value, s->unit_size);
}
void quit(state* s){
    printf("quitting\n");
    exit(0);
}

void debug_mode_print(state* s){
    printf("unit_size: %d\n", s->unit_size);
    printf("file name: %s\n", s->file_name);
    printf("mem count: %s\n",s->mem_count);
}


int main(int argc, char **argv){

     state program_state; 

    // Initialize the program_state struct
    program_state.debug_mode = 0;
    program_state.unit_size = 0; 
    program_state.mem_count = 0;
    program_state.display_mode = 0;

   


    struct fun_desc menu[] ={ 
        {"Toggle Debug Mode", Toggle_Debug_Mode },
        {"Set File Name", Set_File_Name },
        {"Set Unit Size", Set_Unit_Size }, 
        {"Load Into Memory", Load_Into_Memory},
        {"Toggle Display Mode", Toggle_Display_Mode},
        {"Memory Display", Memory_Display},
        {"Save Into File", Save_Into_File},
        {"Memory Modify", Memory_Modify},
        {"quit", quit},
        {NULL, NULL} }; 

    char* carray = malloc(9); 
    carray[0] = '\0'; 
    
    while (1) {
        int len=0;
        while (menu[len].name != NULL){
        len++;  
        }

        if (program_state.debug_mode == 1){
            debug_mode_print(&program_state);
        }
        
        printf("Please choose a function (ctrl^D for exit):\n");
        for (int i = 0; i < len; i++){
            printf("%d",i);
            printf(") ");
            printf("%s\n",menu[i].name);
            
        }
        char in[10];
        if (fgets(in, 10, stdin) == NULL) {
            break; 
        }
    
        int bound =atoi(in);
        int lower =0;

        if (bound <lower || bound >len-1){
            printf("Not within bounds");
            break; 
        }
        printf("within bounds\n");
        
        carray = map(carray, 1, menu[bound].fun);////// made a change here from 9 to 1!!!
        
        printf("Done.\n");


    }
    
    free(carray);
    return 0; 
    
}