
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


struct fun_desc {
    char *name;
    char (*fun)(char);
    };

char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length));
  for (int i=0;i<array_length;i++){
      mapped_array[i] = (*f)(array[i]);
  }
  return mapped_array;
}


char Get_Stirng(char c){
    return (char)fgetc(stdin);
/* Ignores c, reads and returns a character from stdin using fgetc. */
}


char print_String(char c){
    if (c>= 0x20 && c<=0x7E){
        printf("%c\n",c);
    }
    else{
         printf(".\n");
    }
    return c;
}
/* If c is a number between 0x20 and 0x7E, cprt prints the character of ASCII value c followed by a new line. Otherwise, cprt prints the dot ('.') character. After printing, cprt returns the value of c unchanged. */

char encrypt(char c){
    if (c>= 0x20 && c<=0x7E){
        c = c+3;
    }
    return c;
}
/* Gets a char c and returns its encrypted form by adding 1 to its value. If c is not between 0x20 and 0x7E it is returned unchanged */

char decrypt(char c){
    if (c>= 0x20 && c<=0x7E){
        c = c-3;
    }
    return c;
}
/* Gets a char c and returns its decrypted form by reducing 1 from its value. If c is not between 0x20 and 0x7E it is returned unchanged */

char Print_Hex(char c){
    printf("%x\n",c);

    return c;
}
/* xprt prints the value of c in a hexadecimal representation followed by a new line, and returns c unchanged. */


int main(int argc, char **argv){

    struct fun_desc menu[] ={ { "Get Stirng", Get_Stirng }, {"print String", print_String }, {"Encrypt", encrypt }, {"Decrypt", decrypt}, {"Print Hex", Print_Hex}, {NULL, NULL} }; 

    char* carray = malloc(5); 
    carray[0] = '\0'; 
    
    while (1) {
        int len=0;
        while (menu[len].name != NULL){
        len++;  
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
        
        carray = map(carray, 5, menu[bound].fun);
        
        printf("Done.\n");


    }
    
    free(carray);
        
    
    return 0; 
    
}






// int main(int argc, char **argv){
    
//     while (1) {
//         char* in = malloc(10);
//         printf("Select operation from the following menu:\n");
//         if (fgets(in, 10, stdin) == NULL) {
//             break; 
//         }
//         free(in);

//     }
    
        
//     return 0; 
    
// }







    