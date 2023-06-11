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



int LOGSIZE;

typedef struct Command{
  char *name;
  struct tm time;
  int returnVal;
}Command;

// takes in an array of Command struct
void printLog(Command myLogs[]){

  for(int i = 0; i < LOGSIZE; i++){
    printf("%s", asctime(&(myLogs[i].time)));
    printf(" %s %d\n", myLogs[i].name, myLogs[i].returnVal);
  }

  return;
}


Command *createLog(Command *oldLogs, char *cmdName, int cmdVal){

  LOGSIZE++;
  Command *newLogs = malloc(sizeof(Command)*LOGSIZE);
  
  // copy older log into new log if old log existed
  for(int i = 0; i < LOGSIZE-1; i++){
    newLogs[i] = oldLogs[i];
  }

  // creating new single log entry, and copy into the new logs array
  Command temp = {.name = cmdName, .returnVal = cmdVal, .time = NULL };

  // init time 
  time_t rawtime;
  time(&rawtime);
  temp.time = *localtime(&rawtime);


  // problem
  newLogs[LOGSIZE-1] = temp;
  // memcpy(newLogs[LOGSIZE-1], temp, sizeof(temp));


  return newLogs;
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


  
  printf("tokens %s\n", tokens[1]);
  return(tokens);
}



// man execve
int execute_command(char **tokens, EnvVar variables[], Command myLogs[]){
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
 
  
  switch (switchcomm) {
  case 1: //ls
    if(fork() == 0){
    	execvp(tokens[0], tokens);
    }
    else{
    	wait(NULL);
    }    
    myLogs = createLog(myLogs, "ls", 0);
    return (1);

  case 2: //pwd
    if(fork() == 0){
    	execvp(tokens[0], tokens);
    }
    else{
    	wait(NULL);
    }   
    myLogs = createLog(myLogs, "pwd", 0);
    return (1);

  case 3: //whoami
    if(fork() == 0){
    	execvp(tokens[0], tokens);
    }
    else{
    	wait(NULL);
    } 
    myLogs = createLog(myLogs, "whoami", 0);
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
    return(1);
  
  case 5: //exit
    printf("exit");
    printf("Bye!");
    //exit(0);
    return 0;
    
  case 6:
    printf("testing log rework\n");
    printLog(myLogs);
    return (1);


  case 7:
    if(strcmp(tokens[1], "red") == 0){
    	printf("%c[%dm\n", 0x1B, 31);
    }
    else if(strcmp(tokens[1], "blue") == 0){
    	printf("%c[%dm\n", 0x1B, 34);
    }
    else if(strcmp(tokens[1], "green") == 0){
    	printf("%c[%dm\n", 0x1B, 32);
    }
    else{
    	printf("Unsupported theme\n");
    }
    
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

  LOGSIZE = 0;
 	int max_array_size = 20;
 	int current_array_size = 0;
 	EnvVar variables[max_array_size];
  Command myLogs[max_array_size];

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
 					
 				if(execute_command(tokens, variables, &myLogs) == 0){
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
