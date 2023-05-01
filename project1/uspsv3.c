#include "p1fxns.h"
#include "ADTs/arrayqueue.h"
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>


#define UNUSED __attribute__((unused))
#define MAX_PROCESSES 128
#define MAX_LINE_SIZE 4096
#define MAX_ARGS 64
#define MAX_WORD_SIZE 64

typedef struct ChildProcess{
	pid_t pid;
    float totalCPUTime;
	bool running;
	bool finished;
} ChildProcess;

struct itimerval timer;
volatile bool childRunning = false;
volatile bool timeLeft = true;
volatile pid_t current_pid = -1;
int QUANT_SECONDS = -1;
float timeRuning = 0.0;
const Queue *queue;


/* Signal handler for SIGALARM */
void alarm_handler(UNUSED int sig);

void sigchld_handler(UNUSED int sig);

void start_handler(UNUSED int sig);

int main(UNUSED int argc, UNUSED char** argv) {

	/* Set up and read through first line, determine if q is given and if a file is given and act accordingly*/
	int opt;
	int fd = 0;
	char* filename = NULL;
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

	if(QUANT_SECONDS < 0){
		p1putstr(2, "Error: No quantum given");
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
	int len, num_args;
	int pos = 0;
	pid_t pid;
	queue = ArrayQueue(MAX_PROCESSES, free);

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


	/* Set up timer */
	timer.it_value.tv_sec = QUANT_SECONDS / 1000;
	timer.it_value.tv_usec = QUANT_SECONDS * 1000;
	timer.it_interval = timer.it_value;

	/* Set up signal handlers */
	signal(SIGALRM, alarm_handler);
	signal(SIGCHLD, sigchld_handler);

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

			raise(SIGSTOP);
			execvp(arguments[0], arguments);

			p1perror(2, "Error with child process execution");
			return EXIT_FAILURE;
		}
		else{
			ChildProcess* child = malloc(sizeof(ChildProcess));
			child->pid = pid;
			child->totalCPUTime = 0;
			child->finished = false;
			child->running = false;
			queue->enqueue(queue, child);
		}
	}

	/* Close file*/
	close(fd);
	while(!queue->isEmpty(queue)){
		/*Start next process, reset global flags and timer*/
		ChildProcess* child = NULL;
		queue->dequeue(queue, (void**)&child);
		kill(child->pid, SIGCONT);
		childRunning = true;
		timeLeft = true;
		setitimer(ITIMER_REAL, &timer, NULL);

		/*In version 4 there will be work occuring within the loop but here I just left it empty because nothing was specified to be done*/
		while(childRunning && timeLeft){
			sleep(1);
		}

		int status;
		pid_t pid = waitpid(child->pid, &status, WNOHANG);

		if(pid == 0){
			kill(child->pid, SIGSTOP);
			child->totalCPUTime += QUANT_SECONDS/1000;
			queue->enqueue(queue, child);
		}
		else{
			child->finished = true;
			free(child);
		}
	}

	/* Clean up*/
	queue->destroy(queue);
	free(line);
	for(int i = 0; i < MAX_ARGS; i++){
		free(arguments[i]);
	}
	free(arguments);
	return 0;
}


/* Signal handler for SIGALARM */
void alarm_handler(UNUSED int sig){
	timeRuning += QUANT_SECONDS/1000;
	timeLeft = false;
}

void sigchld_handler(UNUSED int sig){
	int status;
	pid_t pid = 0;
	while((pid = waitpid(-1, &status, WNOHANG)) > 0){
		if(WIFEXITED(status) || WIFSIGNALED(status)){
			childRunning = false;
		}
		else{
			p1perror(2, "Child exited with error\n");
		}
	}
}