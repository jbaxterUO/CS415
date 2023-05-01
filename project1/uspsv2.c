#include "p1fxns.h"
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>

#define UNUSED __attribute__((unused))
#define MAX_PROCESSES 128
#define MAX_LINE_SIZE 4096
#define MAX_ARGS 64
#define MAX_WORD_SIZE 64

typedef struct ChildProcess{
	pid_t pid;
} ChildProcess;

volatile bool not_ready = true; /* Used to prevent children from running before all are created */

/* Signal handler for SIG_START */
void signal_handler(UNUSED int sig){
	not_ready = false;
}

int main(UNUSED int argc, UNUSED char** argv) {

	/* Set up and read through first line, determine if q is given and if a file is given and act accordingly*/
	
	int opt;
	int fd = 0;
	char* filename = NULL;
	int QUANT_SECONDS = -1;
	char* QUANT_ENV;
	
	while ((opt = getopt(argc, argv, "q:")) != -1) {
		switch (opt) {
		case 'q':
			QUANT_SECONDS = p1atoi(optarg);
			break;

		default:
		;
		}
	}
	
	/* If there is an environment variable already see if one was provided, if not set it to the established one*/
	if((QUANT_ENV = getenv("USPS_QUANTUM_MSEC")) != NULL){
		if(QUANT_SECONDS < 0){
			QUANT_SECONDS = p1atoi(QUANT_ENV);
		}
	}
	else if (QUANT_SECONDS < 0){
		p1putstr(2, "Error: No environment variable set or passed");
		return EXIT_FAILURE;
	}

	/**If there are any non-optional or non "-" arguments left they will start at arvg[optind], this overrides the default stdin input above*/
	if (optind < argc) {
		filename = argv[optind];
		fd = open(filename, O_RDONLY);
	}
	
	if(fd == -1){
		p1putstr(2, "Error: opening commands file");
		return EXIT_FAILURE;
	}


/*
After establishing environment variables and input types being parsing line by line from either stin or given file

*/
	int len, num_args, status;
	int pos = 0;
	int process_count = 0;

	ChildProcess Child_Processes[MAX_PROCESSES];
	pid_t pid;

/*	Allocate space for arguments */
	char** arguments = (char**)malloc(MAX_ARGS * sizeof(char*));

	if(arguments == NULL){
		p1perror(2, "Error allocating space for arguments");
		return EXIT_FAILURE;
	}

	for(int i = 0; i < MAX_ARGS; i++){
		arguments[i] = malloc(MAX_WORD_SIZE * sizeof(char));
		if(arguments[i] == NULL){
			p1perror(2, "Error allocating space for arguments");
			return EXIT_FAILURE;
		}
	}

	char* line = (char*)malloc(MAX_LINE_SIZE * sizeof(char));

	if(line == NULL){
		p1perror(2, "Error allocating space for line");
		return EXIT_FAILURE;
	}


/* Process line by line, either from file or stdin */
	while((len = p1getline(fd, line, MAX_LINE_SIZE)) != 0){
		num_args = 0;
		int len = p1strlen(line);
		if(line[len-1] == '\n'){line[len-1] = '\0';}

		pos = p1getword(line, 0, arguments[num_args]);
		num_args += 1;

		while((pos = p1getword(line, pos, arguments[num_args]))> -1 && (num_args < MAX_ARGS)){ 
			num_args+= 1;
		}


		pid = fork();

		if(pid == -1){
			p1perror(2, "Error creating child process\n");
			return EXIT_FAILURE;
		}
		else if(pid == 0){
			free(arguments[num_args]);
			arguments[num_args] = NULL;
			
			signal(SIGUSR1, signal_handler); /* Set up signal handler for SIG_START */
			
			while(not_ready){} /* Wait for parent to finish parsing */
			execvp(arguments[0], arguments);
			p1perror(2, "Error with child process execution");
			return EXIT_FAILURE;
		}
		else if(pid > 0){
			Child_Processes[process_count].pid = pid;
			process_count += 1;
		}
		else{
			p1perror(2, "Error creating child process\n");
			return EXIT_FAILURE;
		}
	}

	/*close file*/
	close(fd);

	struct timespec delay = {0, 750000000}; /* Set up delay for nanosleep */

	if(nanosleep(&delay, NULL) == -1){
		p1perror(2, "Error with nanosleep");
		return EXIT_FAILURE;
	}
	
	for(int i = 0; i < process_count; i++){
		kill(Child_Processes[i].pid, SIGUSR1); /* Send SIGUSR1 to all children to start*/
	}
	
	for(int i = 0; i < process_count; i++){
		kill(Child_Processes[i].pid, SIGSTOP); /* Send SIGSTOP to all children to pause*/
	}
	
	for(int i = 0; i < process_count; i++){
		kill(Child_Processes[i].pid, SIGCONT); /* Send SIGCONT to finish all children */
	}

	/*Wait for all children to finish*/
	for(int i = 0; i < process_count; i++){
		waitpid(Child_Processes[i].pid, &status, 0);
	}

	free(line);
	for(int i = 0; i < MAX_ARGS; i++){
		free(arguments[i]);
	}
	free(arguments);

	return 0;
}
