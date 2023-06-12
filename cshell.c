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

typedef struct{
    char *name;
    struct tm time;
    // original line: "return value"
    int value;
    struct Command *preLog;
 }Command;
 
 
 
 // free up log struct
// cmd pointer required
void freeLog(Command *cmdLog){
    struct Command *temp = cmdLog;
    while(cmdLog != NULL){
        temp = cmdLog->preLog;
        free(cmdLog);
        cmdLog = temp;
    }
    return;
}

// this function creates log, called everytime a input command is being processed
// cmd pointer required
struct Command *createLog(char *cmdName, int successFail, struct Command *preCmd){

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
    //printf("while . %s\n", token);
    token = strtok(NULL, delim);  
  }

  numtokens++;
  //printf(". %d\n", numtokens);
// allocate memory for the array of strings

  tokens = malloc(sizeof(char *) * numtokens);
// store each token into the array

  token1 = strtok(line_copy, delim);

  for (i = 0; token1 != NULL; i++){
    tokens[i] = malloc(sizeof(char) * strlen(token1));
    strcpy(tokens[i], token1);
    token1 = strtok(NULL, delim); 
    //printf("inside for %s\n", tokens[i]);
  }
  
  tokens[i] = NULL;
  /*
  if(strcmp(tokens[0], "print") == 0){
  	for(int k = 2; tokens[k] != NULL; k++){
  		strcat(tokens[1], " ");
  		strcat(tokens[1], tokens[k]);
  	}
  	printf("cat tokens %s\n", tokens[1]);
  }*/

  
  //printf("tokens %s\n", tokens[1]);
  return(tokens);
}



// man execve
int execute_command(char **tokens, EnvVar variables[]){
  int numofcomm = 7, switchcomm = 0;
  char *listofcomm[numofcomm];

  listofcomm[0] = "ls";
  listofcomm[1] = "pwd";
  listofcomm[2] = "whoami";
  listofcomm[3] = "print";
  listofcomm[4] = "exit"; 
  listofcomm[5] = "log";
  listofcomm[6] = "theme";
  //printf("!as%d", switchcomm);
  
  for (int i = 0; i < numofcomm; i++){
    if (strcmp(tokens[0], listofcomm[i]) == 0){
      switchcomm = i + 1;
      break;
    }   
  }
  /*script -a myscript.log
  logger -s tokens[0] >> log
  tokens[0] >> fp;
  fputs(tokens[0], fp);
  putc('\n', fp);
  
  fputs(tokens[0], fp);
  putc('\n', fp);*/
  
  switch (switchcomm) {
  case 1: //ls
        int temp = 1;
  	
  	if(tokens[temp] == NULL){
  		if(fork() == 0){
    			execvp(tokens[0], tokens);
		}
		else{
		    	wait(NULL);
		}    
  	}
  	/*else{  	
	  	printf("test print: %s\n", tokens[1]);
	  	char* print_line;	
		print_line = malloc(sizeof(char) * strlen(tokens[0])*1500);
		print_line[0] = '\0';
		printf("test print: %s\n", print_line);
	  	//EnvVar *current = &List;
	  	while(tokens[i] != NULL){
	  		
	  	}
  	}*/
    
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
  	
  	//printf("test print: %s\n", tokens[1]);
  	char* print_line;	
	print_line = malloc(sizeof(char) * strlen(tokens[0])*1500);
	print_line[0] = '\0';
	//printf("test print: %s\n", print_line);
  	//EnvVar *current = &List;
  	while(tokens[i] != NULL){
  		//printf("Inside while loop Tokens %s\n", tokens[i]);
  		//printf("Inside while loop Tokens[0] %c\n", tokens[i][0]);
  		if(tokens[i][0] == '$'){
  			
  			char *compare = NULL;
  			compare = malloc(sizeof(char *) * strlen(tokens[i])*150);
  			//printf("print from before compare list %s\n", compare);
  			
  			strncpy(compare, tokens[i]+1, strlen(tokens[i])-1);
  			compare[strlen(tokens[i])-1] = '\0';
  			
  			//printf("print from compare list %s\n", compare);
  			//printf("print from before while state value list %s\n", variables[j].value);
		  	while(variables[j].name != NULL){
		  		//printf("print from while state value list %s\n", variables[j].value);
		  		if(strcmp(variables[j].name, compare) == 0){
		  			//printf("print from if state value list %s\n", variables[j].value);
		  			//current = current->next;
		  			variables[j].value[strcspn(variables[j].value, "\n")] = 0;
		  			//printf("print from exec list %s\n", variables[j].value);
		  			strcat(print_line, variables[j].value);
		  			//printf("print from print line %s\n", print_line);
		  			strcat(print_line, " ");
		  			//printf("print blank %s\n", print_line);
		  		}
		  		j++;
		  		//printf("j goes up %d\n", j);
		  	}
		  	j = 0;
		  	//printf("j reset %d\n", j);
		  	free(compare);
		  	//printf("compare bye bye %s\n", compare);
		  	/*
		  	for(j = 0; j < 1; j++){
		  		printf("print from exec list %s\n", variables[j].value);
		  	}*/
		}
		else{
			strcat(print_line, tokens[i]);
			strcat(print_line, " ");
		}
		i++;
	}
	printf("Final print: %s\n", print_line);
	free(print_line);
    return(1);
  
  case 5: //exit
    printf("Bye!\n");
    //exit(0);
    return 0;
    
  case 6://log
    printf("testing log\n");
    
    struct Command *newLog = createLog("print", 0, NULL);
    struct Command *newLog2 = createLog("log", 0, newLog);
    
    printLog(newLog2);
    freeLog(newLog2);
    return (1);

  case 7://theme
    if(strcmp(tokens[1], "red") == 0){
    	printf("%c[%dm", 0x1B, 31);
    }
    else if(strcmp(tokens[1], "blue") == 0){
    	printf("%c[%dm", 0x1B, 34);
    }
    else if(strcmp(tokens[1], "green") == 0){
    	printf("%c[%dm", 0x1B, 32);
    }
    else{
    	printf("Unsupported theme\n");
    }
    return 1;
    
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
    return 0;
  }

  else if (pid == 0){
    if (execvp(tokens[0], tokens) < 0){
      printf("\nCould not execute command");
    }
    exit(0);
  }
  else{
    wait(NULL);
    return 0;
  }
}



char **save_var(char* lineptr){ 
	char** tokens;
	char* token;
	
	token = malloc(sizeof(char) * strlen(lineptr));
	
	for(int i = 0; i < strlen(lineptr); i++){
		if(lineptr[i] == ' ' || lineptr[i] == '/'){
			printf("Variable value expected\n");
			return(NULL);
		}
	}
	
	/*for(int i = 1; i < strlen(lineptr); i++){
		if(lineptr[i] == '='){
			tokens = malloc(sizeof(char *) * strlen(lineptr));
			strcpy(tokens, "print ");
			//int j = i;
			//for(int j = i+1; j < strlen(lineptr)-1; j++){
				//token[j-i-1] = lineptr[j];
				//*token++ = lineptr[j];
			strncpy(token, lineptr+i+1, strlen(lineptr)-i);
				//printf("line check %c\n", lineptr[j]);
			//}
			//token = '\0';
			printf("token check %s\n", token);
			strcat(tokens, token);
			printf("tokens check %s\n", tokens);
			//return(tokens);
		}		
	}*/
	int count_equal = 0;
	while(lineptr[count_equal] != '='){
		count_equal++;
	}
	//printf("Count up to = %d\n", count_equal);
	char* var_token;
	char* value_token;
		
	var_token = malloc(sizeof(char) * (count_equal-1));	
	var_token[0] = '\0';
	value_token = malloc(sizeof(char) * (strlen(lineptr)-count_equal));
	value_token[0] = '\0';
	
	//printf("test var token: %s\n", var_token);
	//printf("test value token: %s\n", value_token);
	
	strncpy(var_token, lineptr+1, count_equal-1);
	var_token[count_equal-1] = '\0';
	strncpy(value_token, lineptr+count_equal+1, strlen(lineptr)-count_equal);
	
	//printf("token var check %s\n", var_token);
	//printf("token value check %s\n", value_token);
	
	/*for(int i = 0; i < strlen(lineptr); i++){
		if(lineptr[i] == '='){
			tokens = malloc(sizeof(char *) * strlen(lineptr));
			
			strncpy(tokens, lineptr+i+1, strlen(lineptr)-i);
				
			printf("tokens check %s\n", tokens);
			return(tokens);
		}
	}*/
	//envList->name = var_token;
	tokens = malloc(sizeof(char *) * 2);
	tokens[0] = malloc(sizeof(char *) * strlen(lineptr));
	tokens[0][0] = '\0';
	tokens[1] = malloc(sizeof(char *) * strlen(lineptr));
	tokens[1][0] = '\0';
	
	strcat(tokens[0], var_token);
	strcat(tokens[1], value_token);
	
	free(var_token);
	free(value_token);
	
	return(tokens);
}


 
 int main(int ac, char **argv){
 	
 	//EnvVar *envList = (EnvVar *)malloc(sizeof(EnvVar));
 	
 	
 	if(ac == 1){
	 	int max_array_size = 20;
	 	int current_array_size = 0;
	 	EnvVar variables[max_array_size];
 		//printf("hello");
 		char *prompt = "cshell$ ";
 		char *lineptr = "";
 		char **tokens;
 		
 		while (strcmp(lineptr, "exit\n") != 0){
 			//print prompt chsell$
 			printf("%s", prompt);
 			//read line from std in
 			lineptr = get_line();
 			
 			//printf("%c\n", lineptr[0]);
 			//parse line
 			if(lineptr[0] == '$'){
 				tokens = save_var(lineptr);
 				//printf("List var check %s\n", tokens[0]);
				//printf("List value check %s\n", tokens[1]);
 				if(tokens[0] != NULL && tokens[1] != NULL){
 					//envList->name = tokens[0];
 					//envList->value = tokens[1];
 					for(int i = 0; i <= current_array_size; i++){
 						//printf("name check %s what is here\n", variables[i].name);
 						//printf("tokens check %s what is here\n", tokens[0]);
 							 						
 						if(variables[i].name == NULL){
		 					variables[i].name = tokens[0];
		 					variables[i].value = tokens[1];
	 					
		 					//printf("List var check %s\n", variables[i].name);
							//printf("List value check %s\n", variables[i].value);
							
						}
						else{
							if(strcmp(variables[i].name, tokens[0]) == 0){
	 							//printf("current size %d\n", current_array_size);
	 							variables[i].value = NULL;
	 							variables[i].value = tokens[1];
	 							current_array_size = current_array_size-1; 
	 						}
 						}						
						
 					//printf("List var check %s\n", envList->name);
					//printf("List value check %s\n", envList->value);
					}
					current_array_size++; 
					
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
 		//fclose(fp);
 	}
 	else{
 		int script_max_array_size = 20;
	 	int script_current_array_size = 0;
	 	EnvVar script_variables[script_max_array_size];
	 	
 		//printf("bye");
 		FILE *fp;
 		fp = fopen("script.txt", "r");
 		if(fp == NULL){
 			printf("No file found!\n");
 		}
 		
 		const unsigned MAX_LENGTH = 256;
 		char buffer[MAX_LENGTH];
 		
 		char *script_lineptr = "";
 		char **script_tokens;
 		
 		while(fgets(buffer, MAX_LENGTH, fp)){
 			printf("%s", buffer);
 			
 			script_lineptr = buffer;
 			
 			printf("%c\n", script_lineptr[0]);
 			//parse line
 			if(script_lineptr[0] == '$'){
 				script_tokens = save_var(script_lineptr);
 				if(script_tokens != NULL){
 					
 						if(script_variables[script_current_array_size].name == NULL){
		 					script_variables[script_current_array_size].name = script_tokens[0];
		 					script_variables[script_current_array_size].value = script_tokens[1];
						}
						script_current_array_size++; 
											
 				}
 				else{
 					printf("Something went wrong!\n");
 			 	}
 			}
 			else{
 				script_tokens = split_line(script_lineptr); 					
 				if(execute_command(script_tokens, script_variables) == 0){
					return(0);
				}
			}
 		}
 		
 		fclose(fp);
 	}
 	(void)argv;
 	
 	/*char *prompt = "cshell$ ";
 	char *lineptr = "temp";
 	size_t n = 0;
 	char *isExiting = "";
 	
 	//char cwd[1024];
 	//getcwd(cwd, sizeof(cwd));
 	//printf("\nDir: %s\n", cwd);
 	
 	(void)ac;
 	(void)argv;
 	
 	while (strcmp(isExiting, "exit\n") != 0){
	 	printf("%s", prompt);
	 	getline(&lineptr, &n, stdin);
	 	isExiting = lineptr;
	 	printf("%s\n", isExiting);
	 	int x = strcmp(isExiting, "exit\n");
	 	printf("%d", x);
	 	
	 	//free(lineptr);
 	}
 	
 	free(lineptr);
 	printf("\nBye!\n");*/
 	return (0); 
 }
