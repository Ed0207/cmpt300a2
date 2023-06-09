#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

typedef struct{
    char *name;
    char *value;
}EnvVar;


char *get_line(){

    char *line = NULL;

    return line;
}


char **parser(char *inputLine){

    char **parsed;
    char *token;
    const char *delim = " \n";
    printf("testing\n");
    token = strtok(inputLine, delim);
    int num_token = 0;
    char *line_copy;
    line_copy = malloc(sizeof(char)*strlen(inputLine));
    strcpy(line_copy, inputLine);

    printf("%s\n", line_copy);
    printf("%s\n", token);

    while(token != NULL){

        token = strtok(NULL, delim);

        num_token++;
    }

    printf("%d\n", num_token);

    return **parsed;
}


int main(int ac, char **temp){


    if(ac == 1){

        char *prompt = "cshell$ ";
        bool exit = false;

        // upper bound to be changed!
        char *input = malloc(1000);
        char **tokens;    

        // main loop
        while(!exit){
            
            printf("%s", prompt);
            scanf("%s", input);

            if(input[0] == '$'){

                // store input
                // check variable

            }else if(!strcmp(input, "exit")){
                exit = true;
            }else{
                printf("%s\n", input);
                tokens = parser(input);
            }

        }
        
        free(input);

    }else{
        
    }

    return 0;
}   