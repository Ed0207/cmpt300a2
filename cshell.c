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

int user_input_len;
 

typedef struct Command{
	char *name;
	struct tm time;
	int value;
	struct Command *preLog;
} Command;
 
 
 
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
  	char* another_line_copy = NULL;
  
  	line_copy = malloc(sizeof(char) * strlen(line)* 32);
  	another_line_copy = malloc(sizeof(char) * strlen(line)* 32);
  
  	strcpy(line_copy, line);
  	strcpy(another_line_copy, line);

  	token = strtok(another_line_copy, delim);
  
	// calculate number of tokens
  	while (token != NULL) {
    		numtokens++;    
    		token = strtok(NULL, delim);  
  	}
	user_input_len = numtokens;
  	numtokens++;
  
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
  	
  	if(tokens[0][0] == '$'){
		int loc = 0;			
		char *compare = NULL;
		compare = malloc(sizeof(char *) * strlen(tokens[0])*150);	  			
		strncpy(compare, tokens[0]+1, strlen(tokens[0])-1);
		compare[strlen(tokens[0])-1] = '\0';
  		
  		while(variables[loc].name != NULL){
	  		if(strcmp(variables[loc].name, compare) == 0){	  
	  			variables[loc].value[strcspn(variables[loc].value, "\n")] = 0;			
	  			tokens[0] = variables[loc].value; 
	  		}
	  		loc++;
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
		  	char* print_line;	
			print_line = malloc(sizeof(char) * strlen(tokens[0])*1500);
			print_line[0] = '\0';
			
		  	while(tokens[temp] != NULL){
		  		if(tokens[temp][0] == '$'){  
		  			int k = 0;			
		  			char *compare = NULL;
		  			compare = malloc(sizeof(char *) * strlen(tokens[temp])*150);	  			
		  			strncpy(compare, tokens[temp]+1, strlen(tokens[temp])-1);
		  			compare[strlen(tokens[temp])-1] = '\0';
			  		
			  		while(variables[k].name != NULL){
				  		if(strcmp(variables[k].name, compare) == 0){
				  			if(strcmp(tokens[temp], "-al") == 0){
				  				tokens[temp] = "-al";
				  			}
				  			else{
				  				tokens[temp] = "-l";
				  			}
				  			
				  			if(fork() == 0){
					    			execvp(tokens[0], tokens);
							}
							else{
							    	wait(NULL);
							}  
				  		}
				  		k++;
				  	}		  		
			  	}
			  	else if(strcmp(tokens[temp], "-al") == 0){
			  		if(fork() == 0){
			    			execvp(tokens[0], tokens);
					}
					else{
					    	wait(NULL);
					}   
			  	}
			  	else if(strcmp(tokens[temp], "-l") == 0){
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
	  	char* print_line;	
		print_line = malloc(sizeof(char) * strlen(tokens[0])*1500);
		print_line[0] = '\0';
		
	  	while(tokens[i] != NULL){
	  		if(tokens[i][0] == '$'){
	  			
	  			char *compare = NULL;
	  			compare = malloc(sizeof(char *) * strlen(tokens[i])*150);
	  			
	  			strncpy(compare, tokens[i]+1, strlen(tokens[i])-1);
	  			compare[strlen(tokens[i])-1] = '\0';
	  			
			  	while(variables[j].name != NULL){
			  		if(strcmp(variables[j].name, compare) == 0){
			  			variables[j].value[strcspn(variables[j].value, "\n")] = 0;
			  			strcat(print_line, variables[j].value);
			  			strcat(print_line, " ");
			  			found = true;
			  		}
			  		j++;
			  	}
			  	if(!found){
			  		printf("Error: No Environment Variable %s found.\n", tokens[i]); 
			  		*myLogs= createLog( "print", -1, *myLogs);
			  		return (1);
			  	}
			  	j = 0;
			  	free(compare);
			  	
			}
			else{
				strcat(print_line, tokens[i]);
				strcat(print_line, " ");
			}
			i++;
		}
		printf("%s\n", print_line);
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
    		printLog(*myLogs);
    		*myLogs = createLog( "log", 0, *myLogs);
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
  			
  			strncpy(compare, tokens[word], strlen(tokens[word]));
  			compare[strlen(tokens[word])] = '\0';
  			
		  	while(compare[charac] != NULL || compare[charac] != '\0'){
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
	  	printf("%s\n", upper_line);
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
	
	char** tokens = NULL;
	
	for(int i = 0; i < strlen(lineptr); i++){
		if(lineptr[i] == ' '){
			printf("Variable value expected\n");
			return(tokens);
		}
	}
	
	int count_equal = 0;
	while(lineptr[count_equal] != '='){
		count_equal++;
	}
	
	for(int i = 0; i < count_equal; i++){
		if( lineptr[i] == '/'){
			printf("No / allowed for var\n");
			return(tokens);
		}
	}
	
	char* var_token;
	char* value_token;
		
	var_token = malloc(sizeof(char) * (count_equal-1));	
	var_token[0] = '\0';
	value_token = malloc(sizeof(char) * (strlen(lineptr)-count_equal));
	value_token[0] = '\0';
	
	strncpy(var_token, lineptr+1, count_equal-1);
	var_token[count_equal-1] = '\0';
	strncpy(value_token, lineptr+count_equal+1, strlen(lineptr)-count_equal);
	
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


 
 int main(int acm, char **argv){

 	Command *myLogs; 
	user_input_len = 0;
 		
 	//Interactive mode if acm==1 and script mode otherwise
 	if(acm == 1){
	 	int max_array_size = 20;
	 	int current_array_size = 0;
	 	EnvVar *variables;
	 	variables = malloc(sizeof(EnvVar) * max_array_size);
	 		 	
 		char *prompt = "cshell$ ";
 		char *lineptr = "";
 		char **tokens;
 		
 		while (strcmp(lineptr, "exit\n") != 0){
 			//print prompt chsell$
 			printf("%s", prompt);
 			//read line from std in
 			lineptr = get_line();
 			
 			//parse line
 			if(lineptr[0] == '$'){
 				tokens = split_line(lineptr);
 				int curr = 0;
 				int incr = 0;
 				bool saving_var = false;
 				
 				while(tokens[curr] != NULL){
	 				while(tokens[curr][incr] != NULL){
	 					if(tokens[curr][incr] == '='){
	 						saving_var = true;
	 					} 
	 					incr++;
	 				}
	 				incr = 0;
	 				curr++;
 				}
 				
				if(!saving_var){
					if(execute_command(tokens, variables, &myLogs) == 0){
						return(0);
					}
				}
 				else if(tokens[0] != NULL && saving_var){ 					
					char **temp_tokens;
					temp_tokens = malloc(sizeof(char *) * strlen(lineptr));
					int j = 0;
					
					while(tokens[j] != NULL){
						temp_tokens[j] = malloc(sizeof(char *) * strlen(lineptr));
						strcpy(temp_tokens[j], tokens[j]);
						j++;
					}
					char* var_line;	
					var_line = malloc(sizeof(char) * strlen(lineptr));
					var_line[0] = '\0';
									
 					tokens = save_var(lineptr);
 					
 					if(tokens == NULL){
					  	j = 0;
					  	while(temp_tokens[j] != NULL){
					  		strcat(var_line, temp_tokens[j]);
					  		strcat(var_line, " ");
					  		j++;
					  	}			  	
 						myLogs = createLog(var_line, -1, myLogs);
 					}
 					else{
					  	strcat(var_line, temp_tokens[0]);					  		
 						myLogs = createLog(var_line , 0, myLogs);
 						
	 					for(int i = 0; i <= current_array_size; i++){
	 						if(variables[i].name == NULL){
			 					variables[i].name = tokens[0];
			 					variables[i].value = tokens[1];								
							}
							else{
								if(strcmp(variables[i].name, tokens[0]) == 0){
		 							variables[i].value = NULL;
		 							variables[i].value = tokens[1];
		 							current_array_size = current_array_size-1; 
		 						}
	 						}						
						
						}
						current_array_size++; 
						if(current_array_size >= max_array_size){
							max_array_size *= 2;
							variables = (EnvVar*)(realloc(variables, sizeof(EnvVar) * max_array_size));
						}
					}					
 				}
 				else{
 					printf("Something went wrong!\n");
 			 	}
 			}
 			else if(lineptr != NULL){
 				tokens = split_line(lineptr);
 					
 				if(execute_command(tokens, variables, &myLogs) == 0){
					return(0);
				}
			}
			free(lineptr);
			free(tokens);				
 		}
 		free(variables);
 	}
 	else{
 		int script_max_array_size = 20;
	 	int script_current_array_size = 0;
	 	EnvVar *script_variables;
	 	char *fileName  = argv[1];
	 	
	 	script_variables = malloc(sizeof(EnvVar) * script_max_array_size);
	 	
 		FILE *fp;
 		fp = fopen(fileName, "r");
 		if(fp == NULL){
 			printf("Unable to read script file: %s\n", fileName);
 		}
 		else{
	 		const unsigned MAX_LENGTH = 256;
	 		char buffer[MAX_LENGTH];
	 		
	 		char *script_lineptr = "";
	 		char **script_tokens;
	 		
	 		printf("Able to read script file: %s\n", fileName);
	 		
	 		while(fgets(buffer, MAX_LENGTH, fp)){
	 			printf("%s", buffer);
	 			
	 			script_lineptr = buffer;
	 			
	 			if(script_lineptr[0] == '$'){
	 				script_tokens = split_line(script_lineptr);
	 				
	 				int curr = 0;
	 				int incr = 0;
	 				bool saving_var = false;
	 				while(script_tokens[curr] != NULL){
		 				while(script_tokens[curr][incr] != NULL){
		 					if(script_tokens[curr][incr] == '='){
		 						saving_var = true;
		 					} 
		 					incr++;
		 				}
		 				incr = 0;
		 				curr++;
	 				}
	 				
	 				if(!saving_var){
						if(execute_command(script_tokens, script_variables, &myLogs) == 0){
							return(0);
						}
					}
	 				else if(script_tokens[0] != NULL && saving_var){ 					
						char **temp_tokens;
						temp_tokens = malloc(sizeof(char *) * strlen(script_lineptr));
						int j = 0;
						
						while(script_tokens[j] != NULL){
							temp_tokens[j] = malloc(sizeof(char *) * strlen(script_lineptr));
							strcpy(temp_tokens[j], script_tokens[j]);
							j++;
						}
						char* var_line;	
						var_line = malloc(sizeof(char) * strlen(script_lineptr));
						var_line[0] = '\0';
										
	 					script_tokens = save_var(script_lineptr);
	 					
	 					if(script_tokens == NULL){
						  	j = 0;
						  	while(temp_tokens[j] != NULL){
						  		strcat(var_line, temp_tokens[j]);
						  		strcat(var_line, " ");
						  		j++;
						  	}			  	
	 						myLogs = createLog(var_line, -1, myLogs);
	 					}
	 					else{
						  	strcat(var_line, temp_tokens[0]);					  		
	 						myLogs = createLog(var_line , 0, myLogs);
	 						
		 					for(int i = 0; i <= script_current_array_size; i++){
		 						if(script_variables[i].name == NULL){
				 					script_variables[i].name = script_tokens[0];
				 					script_variables[i].value = script_tokens[1];								
								}
								else{
									if(strcmp(script_variables[i].name, script_tokens[0]) == 0){
			 							script_variables[i].value = NULL;
			 							script_variables[i].value = script_tokens[1];
			 							script_current_array_size = script_current_array_size-1; 
			 						}
		 						}						
							
							}
							script_current_array_size++; 
							if(script_current_array_size >= script_max_array_size){
								script_max_array_size *= 2;
								script_variables = (EnvVar*)(realloc(script_variables, sizeof(EnvVar) * script_max_array_size));
							}
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
