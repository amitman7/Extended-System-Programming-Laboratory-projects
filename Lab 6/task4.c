
int count(char* x){
    int count = 0;
    while (*x != '\0'){
        if (*x >= 48 && *x <= 57){
            count++;
        }

        x++;
    }
    return count;

    
}

int main(int argc, char **argv){ 
    return 0 ;

}