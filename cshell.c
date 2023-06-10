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
    // 
 }Command;

// man getline -> ssize_t getline(char **lineptr, size_t *n, FILE *stream)

// free up log struct
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
struct Command *createLog(char *cmdName, int successFail, struct Command *preCmd){

    Command *cmdLog;
    cmdLog = malloc(sizeof(Command));

    time_t rawtime;
    time(&rawtime);
    cmdLog -> time = *localtime(&rawtime);
    cmdLog -> name = cmdName;
    cmdLog -> value = successFail;

    if(preCmd != NULL){
        cmdLog -> preLog = NULL;
    }else{
        cmdLog -> preLog = preCmd;
    }

    return cmdLog;
}

// use this function to print out log
void printLog(Command *myLog){

    // recursive call
    if(myLog != NULL){
        printLog(myLog -> preLog);
        printf("%s\n", asctime(&(myLog->time)));
        printf(" %s\n", myLog->name);
    // base case
    }else{
        return;
    }
}


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
    printf("while . %s\n", token);
    token = strtok(NULL, delim);  
  }

  numtokens++;
  printf(". %d\n", numtokens);
// allocate memory for the array of strings

  tokens = malloc(sizeof(char *) * numtokens);
// store each token into the array

  token1 = strtok(line_copy, delim);

  for (i = 0; token1 != NULL; i++){
    tokens[i] = malloc(sizeof(char) * strlen(token1));
    strcpy(tokens[i], token1);
    token1 = strtok(NULL, delim); 
    printf("inside for %s\n", tokens[i]);
  }
  
  tokens[i] = NULL;
  
  if(strcmp(tokens[0], "print") == 0){
  	for(int k = 2; tokens[k] != NULL; k++){
  		strcat(tokens[1], " ");
  		strcat(tokens[1], tokens[k]);
  	}
  	printf("cat tokens %s\n", tokens[1]);
  }

  
  printf("tokens %s\n", tokens[0]);
  return(tokens);
}



// man execve
int execute_command(char **tokens, EnvVar variables[]){
  int numofcomm = 5, switchcomm = 0;
  char *listofcomm[numofcomm];

  listofcomm[0] = "ls";
  listofcomm[1] = "pwd";
  listofcomm[2] = "whoami";
  listofcomm[3] = "print";
  listofcomm[4] = "exit"; 
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
  	//EnvVar *current = &List;
  	while(tokens[i] != NULL){
  		if(tokens[i][0] == '$'){
		  	while(variables[j].name != NULL){
		  		printf("print from exec list %s\n", variables[j].value);
		  		//current = current->next;
		  		j++;
		  	}
		  	/*
		  	for(j = 0; j < 1; j++){
		  		printf("print from exec list %s\n", variables[j].value);
		  	}*/
		}
		i++;
	}
    return(1);
  
  case 5: //exit
    printf("exit");
    printf("Bye!");
    //exit(0);
    return 0;

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
	char* var_token;
	char* value_token;
		
	var_token = malloc(sizeof(char) * (count_equal-1));	
	value_token = malloc(sizeof(char) * (strlen(lineptr)-count_equal));
	
	strncpy(var_token, lineptr+1, count_equal-1);
	strncpy(value_token, lineptr+count_equal+1, strlen(lineptr)-count_equal);
	
	printf("token var check %s\n", var_token);
	printf("token value check %s\n", value_token);
	
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
	tokens[1] = malloc(sizeof(char *) * strlen(lineptr));
	
	strcat(tokens[0], var_token);
	strcat(tokens[1], value_token);
	
	return(tokens);
}


 
 int main(int ac, char **argv){
 	//FILE *fp;
 	//fp = fopen("mylog.txt", "wb");
 	//EnvVar *envList = (EnvVar *)malloc(sizeof(EnvVar));
 	int max_array_size = 20;
 	int current_array_size = 0;
 	EnvVar variables[max_array_size];
  struct Command *shellLog = NULL;
 	
 	if(ac == 1){
 		printf("hello");
 		char *prompt = "cshell$ ";
 		char *lineptr = "";
 		char **tokens;
 		
 		while (strcmp(lineptr, "exit\n") != 0){
 			//print prompt chsell$
 			printf("%s", prompt);
 			//read line from std in
 			lineptr = get_line();
 			
 			printf("%c\n", lineptr[0]);
 			//parse line
 			if(lineptr[0] == '$'){
 				tokens = save_var(lineptr);
 				//printf("List var check %s\n", tokens[0]);
				//printf("List value check %s\n", tokens[1]);
 				if(tokens[0] != NULL && tokens[1] != NULL){
 					//envList->name = tokens[0];
 					//envList->value = tokens[1];
 					
 						if(variables[current_array_size].name == NULL){
		 					variables[current_array_size].name = tokens[0];
		 					variables[current_array_size].value = tokens[1];
	 					
		 					printf("List var check %s\n", variables[current_array_size].name);
							printf("List value check %s\n", variables[current_array_size].value);
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
 		//fclose(fp);
 	}
 	else{
 		printf("bye");
 	}
 	(void)argv;

 	return (0); 
 }
