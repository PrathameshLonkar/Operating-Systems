#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
//#include <sys/types.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>




//limits
#define MAX_TOKENS 100
#define MAX_STRING_LEN 100

size_t MAX_LINE_LEN = 10000;


// builtin commands
#define EXIT_STR "exit"
#define EXIT_CMD 0
#define UNKNOWN_CMD 99


FILE *fp; // file struct for stdin
char **tokens;
char *line;
char *Token;
int token_count = 0;
int size = MAX_TOKENS;
char *this_token;
int i=0;
int status;
int ret;


void initialize()
{

	// allocate space for the whole line
	assert( (line = malloc(sizeof(char) * MAX_STRING_LEN)) != NULL);

	// allocate space for individual tokens
	assert( (tokens = malloc(sizeof(char*)*MAX_TOKENS)) != NULL);

	// open stdin as a file pointer 
	assert( (fp = fdopen(STDIN_FILENO, "r")) != NULL);

}

char *tokenize (char * string)
{
	token_count=0;
	while ( (this_token = strsep( &string, " \t\v\f\n\r")) != NULL) {

		if (*this_token == '\0') continue;

		tokens[token_count] = this_token;

		printf("Token %d: %s\n", token_count, tokens[token_count]);

		token_count++;

		// if there are more tokens than space ,reallocate more space
		if(token_count >= size){
			size*=2;
		
			assert ( (tokens = realloc(tokens, sizeof(char*) * size)) != NULL);
			
		}
	}
	
	
	
	
}

void input_red(char *file,int ind){
mode_t Mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
int fdinput = open(file,O_RDONLY, 0);
	if (fdinput < 0) {
		perror("Input descriptor error.");
		return;
	}
	dup2(fdinput, 0);
	close(fdinput);
	tokens[ind]=NULL;
}


void read_command() 
{
	mode_t Mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	// getline will reallocate if input exceeds max length
	assert( getline(&line, &MAX_LINE_LEN, fp) > -1); 	

	printf("Shell read this line: %s\n", line);

	tokenize(line);
	int j=0; 
	
	int index=0;
	
	
	
	

	
	pid_t pid = fork();
	for(j=0;j<token_count;j++){
	if(strcmp(tokens[j],"<")==0){
	index=j;
	//printf("index of < is:%d\n",index);
	}else if(strcmp(tokens[j],">")==0){
	index=j;
	//printf("index of > is:%d\n",index);
	}
	}
	
	if (pid < 0) { 
		perror("Fork Failed"); 
		exit(1); 
	} 

	if (pid == 0) { 
	printf("This is the child\n");
	if(strcmp(tokens[index],"<")==0){
	//printf("%stoken 3",tokens[3]);
	input_red(tokens[token_count-1],index);
	
	}
	else if(strcmp(tokens[index],">")==0){
	printf("entered out");
	
	int fdoutput = open(tokens[token_count-1], O_WRONLY | O_TRUNC | O_CREAT, Mode);
	if (fdoutput < 0) {
		perror("Output File descriptor error.");
		return;
	}
	dup2(fdoutput, 1);
	close(fdoutput);
	tokens[index]=NULL;
	}
	
            execvp(tokens[0], tokens);
            //printf("Exit Code: %s: %s\n", tokens[0], strerror(errno));
            exit(1);
        } 
        else 
        {
             waitpid(pid, &status,0);
        }
	

	
}

int run_command() {

	if (strcmp( tokens[0], EXIT_STR ) == 0)
		return EXIT_CMD;

	return UNKNOWN_CMD;
}
/*void empty_tokens(){
int k=0;
char a="\0";
while(strcmp(tokens[k],"\0")){
strcpy(tokens[k],a);
k++;
}
}*/
int main()
{
	initialize();

	do {
		printf("sh550> ");
		read_command();
		// empty_tokens();
	} while( run_command() != EXIT_CMD );

	return 0;

}



