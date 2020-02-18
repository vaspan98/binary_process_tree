#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <math.h>
#include <signal.h>

#include "Employee.h"
#include "List.h"

#define _GNU_SOURCE

char *myfifo = "/tmp/myfifo";	//string included in every fifo

volatile sig_atomic_t signal_received = 0;	

void sig_handler(int signo) {	//function to count received signals
  
  	if(signo == SIGUSR2) {

  		signal_received++;
  	}
}

int main(int argc, char* argv[]) {
   	
   	int a, i, j, height, maxh, skewed = 0, l, numOfl, f, n;
	long filelen, src, dest;
	char *datafile, *myf;
	char cmd[12], strmaxh[12], strsrc[12], strdest[12], strpid[12];
 	char *rargv[argc + 5];
 	pid_t current, process;
	FILE *fp;			

    fp = NULL;
	
	if(argc < 7) {		
		
		printf("Not enough arguments given\n");
		return EXIT_FAILURE;
	}
	
	for(a = 1; a < argc; a++) {			//give arguments in any priority
	
        if (strcmp(argv[a],"-h") == 0) {

			height = atoi(argv[a + 1]);    
		}
		    	
        if(strcmp(argv[a],"-d") == 0) {

			datafile = argv[a + 1];    
		}

        if(strcmp(argv[a],"-s") == 0) {	//if "-s" flag is given by the user execute skew mode                
			
			skewed = 1;
		} 
	}
	
	if(height < 0) {
    	
		fprintf(stderr, "Height must be non-negative\n");
    	return EXIT_FAILURE;
  	}

	if(height == 0) {
	    
		fprintf(stderr, "Please enter a positive height\n");
	    return EXIT_FAILURE;
	}
	
   if(height > 5) {
      
	  	printf("Depth should be up to 5\n");
      	return EXIT_FAILURE;
    }
	
	maxh = height;	//store actual height of tree to use later
	
	fp = fopen(datafile,"rb");
   	
   	if(fp == NULL) {	//if file cannot be open return
   	
      	perror("Cannot open binary file\n");
      	return EXIT_FAILURE;
	}

	fseek(fp, 0, SEEK_END);	//search all file
	filelen = ftell(fp);	//file length
	rewind(fp);	//pointer returns to the beginning of the file
	
	fclose(fp);
	
	current = getpid();	//get root's pid
	
	snprintf(strpid, sizeof(strpid), "%d", current);

	l = strlen(myfifo) + strlen(strpid) + 1;
	
	myf = (char*) malloc(l * sizeof(char));	//allocate memory for fifo's name
	
	strcpy(myf, myfifo);
	
	myf = strcat(myf, strpid);	//fifo's name created
	
	if(mkfifo(myf, 0666) == -1) {	//create fifo
		
		if(errno != EEXIST) { 
			
			perror("Couldn't create fifo"); 
			return EXIT_FAILURE; 
		}
	}
	
	process = fork();	//root forks first splitter
	
	if(process == -1) {

    	fprintf(stderr, "Problem in process %lu: fork: %s\n", (long) getpid(), strerror(errno));
	}
	
	else if(process == 0) {	//execute first splitter 
		
		if(skewed == 0) {	//if flag -s is not given, divide based on file range in bytes
			
			src = 0;
			dest = filelen;
		}

		else if(skewed == 1) {	//if flag -s is given, firstly pass the number of leaf nodes
			
			numOfl = pow(2.0, height);	//number of leaf nodes
			src = 0;	//leaf 0
			dest = numOfl;	//in scew mode dest just helps in the division of nodes
		}
	
		for(i = 0; i < argc + 4; i++) {

			rargv[i] = (char *) malloc(12 * sizeof(char*));	// allocate memory for ./Internal arguments
		}
		
		strcpy(cmd, "./Internal");
		strcpy(rargv[0], "./Internal");
		
		for(i = 1; i < argc; i++) {	//copy all arguments to the array used is execvp()
			
			strcpy(rargv[i], argv[i]);
		}
		//convert everything to string so it can be given as array argument
		snprintf(strmaxh, sizeof(strmaxh), "%d", maxh);
		snprintf(strsrc, sizeof(strsrc), "%ld", src);
		snprintf(strdest, sizeof(strdest), "%ld", dest);
		
		//copy extra arguments to the array used is execvp()
		strcpy(rargv[argc], strmaxh);				
		strcpy(rargv[argc + 1], strpid);	
		strcpy(rargv[argc + 2], strsrc);	
		strcpy(rargv[argc + 3], strdest);
		
		rargv[argc + 4] = (char*) NULL;
		
		execvp(cmd, rargv);		//run ./Internal executable
		perror("Execvp failed");
		
		for(j = 0; j < argc + 4; j++) {

			free(rargv[j]);			//free memory of arguments 
		}
	}

	else {	//root mode
			
        struct sigaction sa;	//struct defined on <signals.h>

        sa.sa_handler = sig_handler;	//specify the function to be executed when signal is caught
		sa.sa_flags = SA_RESTART;	//if a function is interrupted by this signal, the function will restart and will not fail 
        sigfillset(&sa.sa_mask);	//block every signal when running the sig_handler function

        if(sigaction(SIGUSR2, &sa, NULL) == -1) {	//function that examines and changes signal action
                
			perror("Problem in sigaction");
            return EXIT_FAILURE;
        }
		
		Employee em;
		int numOfsm, c = 0, counter = 0;
		char *output;
		FILE *wfp;
		
		wfp = NULL;
				
		numOfsm = pow(2.0, height - 1);	//number of splitters

		f = open(myf , O_RDONLY , 0);	//open root fifo to read
		
		if((wfp = fopen("output.txt","w")) == NULL) {	//if output file cannot be opened return 
	
			perror("Could not open file.");
	        return EXIT_FAILURE;
		}
				
		for(c=0; c<numOfsm; c++) {	//loop for every splitter
		
			for(;;) {
			
				n = read(f, &em, sizeof(em));	//read employee records
			
				if(n > 0) {	//if there are still records
					
					counter++;	//count employee records
					
					if(wfp != NULL) {	//write employee records to file
						
					   	fprintf(wfp, "%ld %s %s %s %d %s %s %f\n", \
								em.custid, em.LastName, em.FirstName, \
								em.Street, em.HouseID, em.City, em.postcode, \
								em.amount);
					}
				}

				else if(n == 0) {	//if there is no other record
					
					break;
				}

				else {		//if an error occured
					
					perror("Problem in reading"); 
					return EXIT_FAILURE;
				}
			}

			wait(NULL);
		}

		fclose(wfp);	//close file pointer
		close(f);	//close file descriptor
		
		char command[50];
		
		strcpy(command, "sort -g -k 1 output.txt");	//call system() function to execute 'sort' shell command
		system(command);
		
		printf("\nTotal results: %d\n", counter);				//print total employee records
		printf("Number of Signals: %d\n", signal_received);	//print number of signals received from leaf nodes
	}
	
	unlink(myf);	//unlink fifo
	free(myf);	//free allocated memory
	
	return EXIT_SUCCESS;
}
