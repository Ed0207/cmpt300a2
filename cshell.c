 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <string.h>
 #include <time.h>
 #include <stdbool.h>
 #include <sys/wait.h>
 
 
typedef struct {
 	char *name;
 	char *value;
} EnvVar;



typedef struct Command{
	char *name;
	struct tm time;
	int value;
	struct Command *preLog;
} Command;
 
int user_input_len;
 
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



// use this function to print out log
// cmd pointer required
void printLog(Command *myLogs){
	// recursive call
	if(myLogs != NULL){
        	printLog(myLogs -> preLog);
        	printf("%s", asctime(&(myLogs->time)));
        	printf(" %s %d\n", myLogs->name, myLogs->value);
	// base case
	}
	else{
        	return;
	}
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
	}
	else{
        	cmdLog -> preLog = NULL;
	}

 	return cmdLog;
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
  
  	line_copy = malloc(sizeof(char) * strlen(line)* 32);
  
  	strcpy(line_copy, line);

  	token = strtok(line, delim);
  
	// calculate number of tokens
  	while (token != NULL) {
    		numtokens++;    
    		token = strtok(NULL, delim);  
  	}
	// change : incorrect amount of token
  	numtokens++;
	user_input_len = numtokens - 1;


	// allocate memory for the array of strings
  	tokens = malloc(sizeof(char *) * numtokens);
	// store each token into the array
  	token1 = strtok(line_copy, delim);

	for (i = 0; token1 != NULL; i++){
    		tokens[i] = malloc(sizeof(char) * strlen(token1));
    		strcpy(tokens[i], token1);
    		token1 = strtok(NULL, delim); 
	}
  
  	tokens[i] = NULL;
	  
  	return(tokens);
}



// man execve
int execute_command(char **tokens, EnvVar *variables, Command **myLogs){
	int numofcomm = 8, switchcomm = 0;
  	char *listofcomm[numofcomm];

  	listofcomm[0] = "ls";
  	listofcomm[1] = "pwd";
  	listofcomm[2] = "whoami";
  	listofcomm[3] = "print";
  	listofcomm[4] = "exit"; 
  	listofcomm[5] = "log";
  	listofcomm[6] = "theme";
  	listofcomm[7] = "uppercase";
  	
	if(user_input_len == 0){
		return 1;
	}

  	if(tokens == NULL){
		return 1;
	}
	
  	if(tokens[0][0] == '$'){
		printf("first test print: %s\n", tokens[1]);
		int loc = 0;			
		char *compare = NULL;
		compare = malloc(sizeof(char *) * strlen(tokens[0])*150);	  			
		strncpy(compare, tokens[0]+1, strlen(tokens[0])-1);
		compare[strlen(tokens[0])-1] = '\0';
  		
  		while(variables[loc].name != NULL){
	  		//printf("print from while state value list %s\n", variables[temp].value);
	  		if(strcmp(variables[loc].name, compare) == 0){
	  			tokens[0] = variables[loc].value; 
	  		}
	  		loc++;
	  		//printf("j goes up %d\n", j);
	  	}
	}
  
	for (int i = 0; i < numofcomm; i++){
		if (strcmp(tokens[0], listofcomm[i]) == 0){
	      		switchcomm = i + 1;
	      		break;
	    	}   
	}
	
	
  
  	switch (switchcomm) {
  	case 1: //ls

			if(user_input_len > 2){
				printf("Error: too many arguments detected\n");
				*myLogs = createLog( "too many argument", -1, *myLogs);
				return 1;
			}

        int temp = 1;
  	
  		if(tokens[temp] == NULL){
  			if(fork() == 0){
    				execvp(tokens[0], tokens);
			}
			else{
		    		wait(NULL);
			}    
  		}
  		else{  	
		  	//printf("first test print: %s\n", tokens[1]);
		  	char* print_line;	
			print_line = malloc(sizeof(char) * strlen(tokens[0])*1500);
			print_line[0] = '\0';
			//printf("second test print: %s\n", print_line);
		  	//EnvVar *current = &List;
		  	//printf("test tokens[temp]: %s\n", tokens[temp]);
		  	while(tokens[temp] != NULL){
		  		//printf("while tokens[temp]: %s\n", tokens[temp]);
		  		if(tokens[temp][0] == '$'){  
		  			int k = 0;			
		  			char *compare = NULL;
		  			compare = malloc(sizeof(char *) * strlen(tokens[temp])*150);	  			
		  			strncpy(compare, tokens[temp]+1, strlen(tokens[temp])-1);
		  			compare[strlen(tokens[temp])-1] = '\0';
			  		
			  		while(variables[k].name != NULL){
				  		//printf("print from while state value list %s\n", variables[temp].value);
				  		if(strcmp(variables[k].name, compare) == 0){
				  			//printf("test print: %s\n", tokens[1]);
				  			tokens[temp] = "-al";
				  			if(fork() == 0){
					    			execvp(tokens[0], tokens);
							}
							else{
							    	wait(NULL);
							}  
				  		}
				  		k++;
				  		//printf("j goes up %d\n", j);
				  	}		  		
			  	}
			  	else if(strcmp(tokens[temp], "-al") == 0){
			  		//printf("else if tokens[temp]: %s\n", tokens[temp]);
			  		if(fork() == 0){
			    			execvp(tokens[0], tokens);
					}
					else{
					    	wait(NULL);
					}   
			  	}
			  	temp++;
		  	}
  		}
    		*myLogs = createLog( "ls", 0, *myLogs);
		return (1);

  	case 2: //pwd
			if(user_input_len > 1){
				printf("Error: too many arguments detected\n");
				*myLogs = createLog( "too many argument", -1, *myLogs);
				return 1;
			}

    		if(fork() == 0){
    			execvp(tokens[0], tokens);
    		}
    		else{
    			wait(NULL);
    		}   
    		*myLogs = createLog( "pwd", 0, *myLogs);
    		return (1);

  	case 3: //whoami
			if(user_input_len > 1){
				printf("Error: too many arguments detected\n");
				*myLogs = createLog( "too many argument", -1, *myLogs);
				return 1;
			}

    		if(fork() == 0){
    			execvp(tokens[0], tokens);
    		}
    		else{
    			wait(NULL);
    		} 
    		*myLogs = createLog( "whoami", 0, *myLogs);
    		return (1);
    		
  	case 4: //printing
	  	int i = 1;
	  	int j = 0;
	  	bool found = false;
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
			  			found = true;
			  		}
			  		j++;
			  		//printf("j goes up %d\n", j);
			  	}
			  	if(!found){
			  		printf("Error: No Environment Variable %s found.\n", tokens[i]); 
			  		*myLogs= createLog( "print", -1, *myLogs);
			  		return (1);
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
		*myLogs = createLog( "print", 0, *myLogs);
	    	return(1);
  
  	case 5: //exit
		if(user_input_len > 2){
			printf("Error: too many arguments detected\n");
			*myLogs = createLog( "too many argument", -1, *myLogs);
			return 1;
		}else{
			printf("Bye!\n");
    		freeLog(*myLogs);
		}
    		return 0;
    
  	case 6://log
		if(user_input_len > 1){
			printf("Error: too many arguments detected\n");
			*myLogs = createLog( "too many argument", -1, *myLogs);
			return 1;
		}else{
			printLog(*myLogs);
		}
		return (1);

  	case 7://theme

		if(user_input_len > 2){
			printf("Error: too many arguments detected\n");
			*myLogs = createLog( "too many argument", -1, *myLogs);
			return 1;
		}

  		if(tokens[1] == NULL){
			printf("Missing keyword or command, or permission problem\n");
			*myLogs= createLog( "theme", -1, *myLogs);			
		}
		else if(strcmp(tokens[1], "red") == 0){
			printf("%c[%dm", 0x1B, 31);
		    	*myLogs= createLog( "theme", 0, *myLogs);
		}
		else if(strcmp(tokens[1], "blue") == 0){
		    	printf("%c[%dm", 0x1B, 34);
		    	*myLogs= createLog( "theme", 0, *myLogs);
		}
		else if(strcmp(tokens[1], "green") == 0){
		    	printf("%c[%dm", 0x1B, 32);
		    	*myLogs= createLog( "theme", 0, *myLogs);
		}
		else{
		    	printf("Unsupported theme\n");
		    	*myLogs= createLog( "theme", -1, *myLogs);
		}
		return 1;
		
	case 8://uppercase
    		int charac = 0;
    		int word = 1;
    		char* upper_line;	
		upper_line = malloc(sizeof(char) * strlen(tokens[0])*1500);
		upper_line[0] = '\0';		
    		while(tokens[word] != NULL){
    			char *compare = NULL;
  			compare = malloc(sizeof(char *) * strlen(tokens[word])*150);
  			//printf("print from before compare list %s\n", compare);
  			
  			strncpy(compare, tokens[word], strlen(tokens[word]));
  			compare[strlen(tokens[word])] = '\0';
  			//printf("upper print: %s\n", upper_line);
		  	while(compare[charac] != NULL || compare[charac] != '\0'){
		  		//printf("while upper print: %s\n", upper_line);
		  		if(tokens[word][charac] >= 'a' && tokens[word][charac] <= 'z'){
		  			tokens[word][charac] = tokens[word][charac] - 32;
		  		}	 
		  		charac++; 		
		  	}
		  	strcat(upper_line, tokens[word]);
		  	strcat(upper_line, " ");
		  	free(compare);
		  	charac = 0;
		  	word++;
		}
	  	printf("Final print: %s\n", upper_line);
	  	free(upper_line);
		*myLogs = createLog( "uppercase", 0, *myLogs);
		return (1);
    
  	default:
  		*myLogs = createLog( tokens[0], -1, *myLogs);
		printf("Missing keyword or command, or permission problem\n");
    	break;   

 	}
 	
	return (1);
}



char **save_var(char* lineptr){ 
	char** tokens;

	// char* token;
	// token = malloc(sizeof(char) * strlen(lineptr));
	
	for(int i = 0; i < strlen(lineptr); i++){
		if(lineptr[i] == ' ' || lineptr[i] == '/'){
			printf("Variable value expected\n");
			return(NULL);
		}
	}
	
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
	
	tokens = malloc(sizeof(char *) * 2);
	tokens[0] = malloc(sizeof(char *) * strlen(lineptr));
	tokens[0][0] = '\0';
	tokens[1] = malloc(sizeof(char *) * strlen(lineptr));
	tokens[1][0] = '\0';
	
	strcat(tokens[0], var_token);
	strcat(tokens[1], value_token);
	
	free(var_token);
	free(value_token);
	
	//printf("tokens[0] after free %s\n", tokens[0]);
	//printf("token[1] after free %s\n", tokens[1]);
	
	return(tokens);
}


 
 int main(int acm, char **argv){
 	Command *myLogs; 
	user_input_len = 0;
 		
 	//Interactive mode if acm==1 and script mode otherwise
 	if(acm == 1){
	 	int max_array_size = 20;
	 	int current_array_size = 0;
	 	EnvVar *variables;
	 	variables = malloc(sizeof(EnvVar) * max_array_size);
	 		 	
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
 				tokens = split_line(lineptr);
 				int incr = 0;
 				bool saving_var = false;
 				while(tokens[0][incr] != NULL){
 					if(tokens[0][incr] == '='){
 						saving_var = true;
 					} 
 					incr++;
 				}
 				
 				
 				printf("List var check if %s\n", tokens[0]);
				//printf("List value check if %s\n", tokens[1]);
				//if((strcmp(tokens[1], "pwd") == 0) || (strcmp(tokens[1], "ls") == 0)){
				if(!saving_var){
					printf("here List value check if %s\n", tokens[0]);
					if(execute_command(tokens, variables, &myLogs) == 0){
						return(0);
					}
				}
 				else if(tokens[0] != NULL && saving_var){
 					myLogs = createLog( tokens[0] , 0, myLogs);
 					tokens = save_var(lineptr);
 					
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
					
					}
					//printf("current size %d\n", current_array_size);
					current_array_size++; 
					if(current_array_size >= max_array_size){
						max_array_size *= 2;
						variables = (EnvVar*)(realloc(variables, sizeof(EnvVar) * max_array_size));
					}					
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
			free(lineptr);
			free(tokens);				
 		}
 		//fclose(fp);
 		free(variables);
 	}
 	else{
 		int script_max_array_size = 20;
	 	int script_current_array_size = 0;
	 	EnvVar *script_variables;
		char *fileName = argv[1];
	 	
	 	script_variables = malloc(sizeof(EnvVar) * script_max_array_size);
	 	
	 	
 		//printf("bye");
 		FILE *fp;
 		fp = fopen(fileName, "r");
 		if(fp == NULL){
 			printf("Unable to read script file: %s\n", fileName);
 		}else{
						
			const unsigned MAX_LENGTH = 256;
			char buffer[MAX_LENGTH];

			char *script_lineptr = "";
			char **script_tokens;
			
			while(fgets(buffer, MAX_LENGTH, fp)){
				printf("%s", buffer);
				
				script_lineptr = buffer;
				
				//printf("%c\n", script_lineptr[0]);
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
					if(script_tokens[0] != NULL && script_tokens[1] != NULL){
						
						for(int i = 0; i <= script_current_array_size; i++){
							//printf("name check %s what is here\n", variables[i].name);
							//printf("tokens check %s what is here\n", tokens[0]);
														
							if(script_variables[i].name == NULL){
								script_variables[i].name = script_tokens[0];
								script_variables[i].value = script_tokens[1];
							
								//printf("List var check %s\n", variables[i].name);
								//printf("List value check %s\n", variables[i].value);
								
							}
							else{
								if(strcmp(script_variables[i].name, script_tokens[0]) == 0){
									//printf("current size %d\n", current_array_size);
									script_variables[i].value = NULL;
									script_variables[i].value = script_tokens[1];
									script_current_array_size = script_current_array_size-1; 
								}
							}						
						
						}
						//printf("current size %d\n", current_array_size);
						script_current_array_size++; 
						if(script_current_array_size >= script_max_array_size){
							script_max_array_size *= 2;
							script_variables = (EnvVar*)(realloc(script_variables, sizeof(EnvVar) * script_max_array_size));
						}					
					}
					else{
						printf("Something went wrong!\n");
					}
					
				}
				else{
					script_tokens = split_line(script_lineptr); 					
					if(execute_command(script_tokens, script_variables, &myLogs) == 0){
						return(0);
					}
				}
			}
			free(script_variables);
			fclose(fp);
			printf("Bye!\n");
		}
 	}
 	return (0); 
 }
