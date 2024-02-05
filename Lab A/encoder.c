#include <stdlib.h>
#include <stdio.h>


int main(int argc, char **argv){

   FILE* inFile= stdin;
   FILE* outFile= stdout;
   int c;
   int i = 0;
   int state = 0;
   
   int out =0;
   //part 1 - debug
   while (argv[i] != NULL){
      if (*argv[i] == '+') {
         if (argv[i][1] == 'D' && argv[i][2] =='\0' && state != 1){
            i++;
            state = 1;
         }
      }     
      if (*argv[i] == '-'){
         if (argv[i][1] == 'D' && argv[i][2] =='\0' && state != 0){
            i++;
            state = 0;
            
         }   
      }
      if (state == 1){
         fprintf(stderr,"%s\n",argv[i]);
      }
      
      i++;        
      
   }

   
   if (argv[out] == NULL){
      exit(1);
   }
  
   // encoder
   int mode =0;
   int keyIndex =0;
   int keysNum =0;
   int j=0;
   
   
   // creating an array of the keys and checking mode of encode 
   int *keys = malloc(keysNum);
   int t =0;
   
   while (argv[t] != NULL){
      if (argv[t][0] == '+' && argv[t][1]  == 'e'){
         mode =1;
         while (argv[t][j] !='\0'){
            keysNum++;
            j++;
         }
         keysNum = keysNum -2;
         for (int i = 0; i < keysNum; i++) {
            keys[i] = argv[t][i + 2] - '0';
         }
         
      }
      if (argv[t][0] == '-' && argv[t][1]  == 'e'){
         mode =-1;
         while (argv[t][j] !='\0'){
            keysNum++;
            j++;
         }
         keysNum = keysNum -2;
         for (int i = 0; i < keysNum; i++) {
            keys[i] = argv[t][i + 2] - '0';
         }
      }
      // part 3 - the file
      if (argv[t][0] == '-' && argv[t][1] == 'i'){
         inFile = fopen(&argv[t][2], "r");
         if (!inFile) {
            fprintf(stderr, "error in open input file: %s\n", &argv[t][2]);
            exit(1);
         }

    } 
      if (argv[t][0] == '-' && argv[t][1] == 'o') {
         outFile = fopen(&argv[t][2], "w");
         if (!outFile) {
            fprintf(stderr, "error in open output file: %s\n", &argv[t][2]);
            exit(1);
         }
      } 
   t++;
   }

   // the main loop of the encoder 
   while ((c = fgetc(inFile)) != EOF && mode !=0){
      
      // BIG
      if (c >= 'A' && c<= 'Z') {
         if (mode == 1) {
            c += keys[keyIndex];
         }
         if (mode == -1) {
            c -= keys[keyIndex];
         }
      
         if (c> 'Z'){
            c = c-26;
         }
         if (c< 'A'){
            c = c+ 26; 
         }
      }
      // lOW     
      if (c >= 'a' && c<= 'z') {
         if (mode == 1) {
            c += keys[keyIndex];
         }
         if (mode == -1) {
         c -= keys[keyIndex];
         }
         
         if (c> 'z'){
            c = c-26;
         }
         if (c< 'a'){
            c = c+ 26;
         }
      }
      // NUMRIC
      if (c >= '0' && c<= '9') {
         if (mode == 1) {
            c += keys[keyIndex];
         }
         if (mode == -1) {
         c -= keys[keyIndex];
         }

         if (c> '9'){
            c = c-10;
         }
         if (c< '0'){
            c = c+ 10;
         }
      } 
      keyIndex++;

      if (keyIndex == keysNum) {
         keyIndex = 0;
      }          
      fputc(c,outFile); 
         
   }
   free(keys);


   
   return 0;

}

     

   

