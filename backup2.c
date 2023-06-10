 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <string.h>
 #include <time.h>
 
 typedef struct {
 	char *name;
 	char *value;
 	//struct EnvVar * next;
 } EnvVar;

typedef struct Command{
    char *name;
    struct tm time;
    // original line: "return value"
    int value;
    struct Command *preLog;
 }Command;
 
 
 
 // free up log struct
// cmd pointer required
void freeLog(Command *cmdLog){
    Command *temp = cmdLog;
    while(cmdLog != NULL){
        temp = cmdLog->preLog;
        free(cmdLog);
        cmdLog = temp;
    }
    return;
}

// this function creates log, called everytime a input command is being processed
// cmd pointer required
Command *createLog(char *cmdName, int successFail, Command *preCmd){

    Command *cmdLog;
    cmdLog = malloc(sizeof(Command));

    time_t rawtime;
    time(&rawtime);
    cmdLog -> time = *localtime(&rawtime);
    cmdLog -> name = cmdName;
    cmdLog -> value = successFail;

    if(preCmd != NULL){
        cmdLog -> preLog = preCmd;
    }else{
        cmdLog -> preLog = NULL;
    }

    return cmdLog;
}

// use this function to print out log
// cmd pointer required
void printLog(Command *myLog){

    // recursive call
    if(myLog != NULL){
        printLog(myLog -> preLog);
        // asctime return string appended with \n
        printf("%s", asctime(&(myLog->time)));
        printf(" %s %d\n", myLog->name, myLog->value);
    // base case
    }else{
        return;
    }
}
 
 
 
// man getline -> ssize_t getline(char **lineptr, size_t *n, FILE *stream)

char *get_line(void){
  char *line = NULL;
  size_t bufsize = 0;

  if (getline(&line, &bufsize, stdin) == -1) {
    if (feof(stdin)){
      free(line);
      exit(EXIT_SUCCESS);
    }
    else{
      free(line);
      perror("Error to read line from stdin");
      exit(EXIT_SUCCESS);
    }   
  }

  return(line);
}



// split string (line) into array of words

// man strtok -> char *strtok(char *str, const char *delim)

char **split_line(char *line){
  char *token;
  char *token1;
  char **tokens;
  int i = 0;
  int numtokens = 0;
  const char *delim = " \n"; 
  char* line_copy = NULL;
  
  line_copy = malloc(sizeof(char) * 26);
  
  strcpy(line_copy, line);

  token = strtok(line, delim);
  
// calculate number of tokens
  while (token != NULL) {
    numtokens++;    
    // printf("while . %s\n", token);
    token = strtok(NULL, delim);  
  }

  numtokens++;
  // printf(". %d\n", numtokens);
// allocate memory for the array of strings

  tokens = malloc(sizeof(char *) * numtokens);
// store each token into the array

  token1 = strtok(line_copy, delim);

  for (i = 0; token1 != NULL; i++){
    tokens[i] = malloc(sizeof(char) * strlen(token1));
    strcpy(tokens[i], token1);
    token1 = strtok(NULL, delim); 
    // printf("inside for %s\n", tokens[i]);
  }
  
  tokens[i] = NULL;
  
  // printf("tokens %s\n", tokens[1]);
  return(tokens);
}



// man execve
int execute_command(char **tokens, EnvVar variables[]){
  int numofcomm = 6, switchcomm = 0;
  char *listofcomm[numofcomm];

  listofcomm[0] = "ls";
  listofcomm[1] = "pwd";
  listofcomm[2] = "whoami";
  listofcomm[3] = "print";
  listofcomm[4] = "exit"; 
  listofcomm[5] = "log";
  
  for (int i = 0; i < numofcomm; i++){
    if (strcmp(tokens[0], listofcomm[i]) == 0){
      switchcomm = i + 1;
      break;
    }   
  }
  
  switch (switchcomm) {
  case 1: //ls
    if(fork() == 0){
    	execvp(tokens[0], tokens);
    }
    else{
    	wait(NULL);
    }    
    return (1);

  case 2: //pwd
    if(fork() == 0){
    	execvp(tokens[0], tokens);
    }
    else{
    	wait(NULL);
    }   
    return (1);

  case 3: //whoami
    if(fork() == 0){
    	execvp(tokens[0], tokens);
    }
    else{
    	wait(NULL);
    } 
    return (1);
  case 4: //printing
  	int i = 1;
  	int j = 0;
  	char* print_line;	
	print_line = malloc(sizeof(char) * strlen(tokens)*150);
  	//EnvVar *current = &List;
  	while(tokens[i] != NULL){
  		//printf("Inside while loop Tokens %s\n", tokens[i]);
  		//printf("Inside while loop Tokens[0] %c\n", tokens[i][0]);
  		if(tokens[i][0] == '$'){
  			char *compare;
  			compare = malloc(sizeof(char *) * strlen(tokens[i]));
  			
  			strncpy(compare, tokens[i]+1, strlen(tokens[i])-1);
  			
		  	while(variables[j].name != NULL){
		  		if(strcmp(variables[j].name, compare) == 0){
		  			//printf("print from exec list %s\n", variables[j].value);
		  			//current = current->next;
		  			variables[j].value[strcspn(variables[j].value, "\n")] = 0;
		  			strcat(print_line, variables[j].value);
		  			strcat(print_line, " ");
		  		}
		  		j++;
		  	}
		  	j = 0;
		  	
		}
		else{
			strcat(print_line, tokens[i]);
			strcat(print_line, " ");
		}
		i++;
	}
	// printf("Final print: %s\n", print_line);
    return(1);
  
  case 5: //exit
    printf("Bye!\n");
    //exit(0);
    return 0;
    
  case 6:
    printf("testing log (to be removed)\n");
    
    struct Command *newLog = createLog("print", 0, NULL);
    struct Command *newLog2 = createLog("log", 0, newLog);

    printLog(newLog2);
    freeLog(newLog2);
    return (1);

  default:
    break;   

 }

return (1);

}



int execute_sys_comm(char **tokens){
// fork a child
  pid_t pid = fork();

  if (pid == -1){
    printf("\nFailed child");
    return;
  }

  else if (pid == 0){
    if (execvp(tokens[0], tokens) < 0){
      printf("\nCould not execute command");
    }
    exit(0);
  }
  else{
    wait(NULL);
    return;
  }
}



char **save_var(char* lineptr){
	char** tokens;
	char* token;
	
	token = malloc(sizeof(char) * strlen(lineptr));
	
	for(int i = 0; i < strlen(lineptr); i++){
		if(lineptr[i] == ' '){
			printf("Variable value expected\n");
			return(NULL);
		}
	}
	
	
	int count_equal = 0;
	while(lineptr[count_equal] != '='){
		count_equal++;
	}
	char* var_token;
	char* value_token;
		
	var_token = malloc(sizeof(char) * (count_equal-1));	
	value_token = malloc(sizeof(char) * (strlen(lineptr)-count_equal));
	
	strncpy(var_token, lineptr+1, count_equal-1);
	strncpy(value_token, lineptr+count_equal+1, strlen(lineptr)-count_equal);
	
	printf("token var check %s\n", var_token);
	printf("token value check %s\n", value_token);
	
	
	tokens = malloc(sizeof(char *) * 2);
	tokens[0] = malloc(sizeof(char *) * strlen(lineptr));
	tokens[1] = malloc(sizeof(char *) * strlen(lineptr));
	
	strcat(tokens[0], var_token);
	strcat(tokens[1], value_token);
	
	return(tokens);
}


 
 int main(int ac, char **argv){
 	
 	int max_array_size = 20;
 	int current_array_size = 0;
 	EnvVar variables[max_array_size];
 	
 	if(ac == 1){
 		char *prompt = "cshell$ ";
 		char *lineptr = "";
 		char **tokens;
 		
 		while (strcmp(lineptr, "exit\n") != 0){
 			//print prompt chsell$
 			printf("%s", prompt);
 			//read line from std in
 			lineptr = get_line();
 			
 			// printf("%c\n", lineptr[0]);
 			//parse line
 			if(lineptr[0] == '$'){
 				tokens = save_var(lineptr);
 				
 				if(tokens[0] != NULL && tokens[1] != NULL){
 					
 						if(variables[current_array_size].name == NULL){
		 					variables[current_array_size].name = tokens[0];
		 					variables[current_array_size].value = tokens[1];
	 					
		 					//printf("List var check %s\n", variables[current_array_size].name);
							//printf("List value check %s\n", variables[current_array_size].value);
						}
						current_array_size++; 
						
 					//printf("List var check %s\n", envList->name);
					//printf("List value check %s\n", envList->value);
					
 				}
 				else{
 					printf("Something went wrong!\n");
 			 	}
 			}
 			else{
 				tokens = split_line(lineptr);
 				//printf("%s\n", *tokens);
 					
 				if(execute_command(tokens, variables) == 0){
					return(0);
				}
			}
			
 		}
    
 	}
 	else{

    char *fileName = argv[1];
    FILE *fp = fopen(fileName,"r");

    if(fp == NULL){
      printf("Unable to read script file: %s\n", fileName);
      exit(1);
    }else{
      // start parsing the line
      char *line = NULL;
      size_t len = 0;
      ssize_t read;
      printf("opening file: %s\n", fileName);
      
      while((read = getline(&line, &len, fp)) != -1){
        printf("%s", line);


        // the following code is directly copied from interactive mode
        // can be edited

        
        

      }

      if(line){
        free(line);
      }
    }

    fclose(fp);
 		printf("bye\n");
 	}
 	
 	return (0); 
 }
