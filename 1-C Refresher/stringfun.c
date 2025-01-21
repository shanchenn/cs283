#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
void reverse_string(char *, int);
void print_words(char *, int);

int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    char *input_ptr = user_str;
    char *buffer_ptr = buff;
    int count = 0;
    int whitespace = 0;

    if (*input_ptr == '\0') {
        return 0;
    }

    while(*input_ptr != '\0' && count < len){
        if (*input_ptr == ' ' || *input_ptr == '\t') {
            if (!whitespace) {
                *buffer_ptr++ = ' ';
                count++;
                whitespace = 1;
            }
         } else {
            *buffer_ptr++ = *input_ptr;
            count++;
            whitespace = 0;
        }
        input_ptr++;
    }

    if (*input_ptr != '\0') {
        return -1;
    }

    // add dots
    while (count < len) {
        *buffer_ptr++ = '.';
        count++;
    }
    
    return count;
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    int count = 0;
    int in_word = 0;

    for (int i = 0; i < str_len && i < len; i++) {
        if (*(buff + i) != ' ') {
            if (!in_word) {
                count++;
                in_word = 1;
            }
        } else {
            in_word = 0;
        }
    }

    return count;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

void reverse_string(char *buff, int str_len) {
    char *start = buff;
    char *end = buff + str_len - 1;
    char temp;

    while (start < end) {
        temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
}

void print_words(char *buff, int str_len) {
    printf("Word Print\n");
    printf("----------\n");

    int word_start = 0;
    for (int i = 0; i <= str_len; i++) {
        if (*(buff + i) == ' ' || *(buff + i) == '.' || i == str_len) {
            if (i > word_start) {
                printf("%.*s (%d)\n", i - word_start, buff + word_start, i - word_start);
            }
            word_start = i + 1;
        }
    }
}

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    // This is safe because it makes sure that we always have more than two arguments and the second argument must contain "-" for the different options or else the program exits.
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    // The if statement below makes sure we have a string to process, if there is no string then there is no point in continuing and the program exits.
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3

    buff = (char *)malloc(BUFFER_SZ * sizeof(char));

    if(!buff){
        exit(99);
    }


    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        case 'r':
            reverse_string(buff, user_str_len);
            printf("Reversed String: ");
            for (int i = 0; i < user_str_len; i++) {
                putchar(*(buff + i));
            }
            break;

        case 'w':
            print_words(buff, user_str_len);
            break;

        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    //print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
// Providing both the pointer and length is good practice because it can protect against buffer overflows