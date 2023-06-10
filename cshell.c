#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

// 
const char *PROMPT = "cshell$ ";
const char *DELIM= " \n";

typedef struct{
    char *name;
    char *value;
}EnvVar;


void freeSentence(char **oldSent){

    int i = 0;

    while(oldSent[i] != NULL){       
        free(oldSent[i]);
        i++;
    }


    return;
}


char *get_line(){

    char *line = NULL;

    return line;
}


char **parser(char *inputLine){

    printf("parser\n");
    printf("input: %s\n", inputLine);

    char **parsed;
    char *token, *line_copy;
    int num_token = 0;

    line_copy = malloc(sizeof(char)*sizeof(inputLine));
    parsed = malloc(sizeof(char)*sizeof(line_copy));
    strcpy(line_copy, inputLine);
    token = strtok(line_copy, DELIM);

    printf("before while loop in parser\n");
    while(token != NULL){

        num_token++;

        printf("token: %s\n", token);
        printf("token number: %d\n", num_token);

        parsed[num_token] = token;
        token = strtok(NULL, DELIM);
    }


    free(token);
    free(line_copy);

    return parsed;
}


int main(int ac, char **temp){

    // input mode
    if(ac == 1){

        bool exit = false;
        // upper bound to be changed!
        char *input = malloc(sizeof(char)*999);
        char **tokens;    

        // main loop
        while(!exit){
            
            
            printf("%s", PROMPT);
            // scanf("%s", input);
            fgets(input, 1000,stdin);

            if(input[0] == '$'){
                printf("using var mode");
                // store input
                // check variable

            }else if(!strcmp(input, "exit\n")){
                printf("bye\n");
                exit = true;
            }else{
                tokens = parser(input);
            }

            freeSentence(tokens);
        }
        

        free(input);
        // free the pointers within pointer

    // script mode
    }else{
        
    }

    return 0;
}   