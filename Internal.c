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

#include "Employee.h"
#include "List.h"

char *myfifo = "/tmp/myfifo";	//string included in every fifo

void push(List **head, Employee emp);	//definition of push() function
int count_emp(List *head);	//definition of count_emp() function
void free_list(List *head);	//definition of free_list() function

int main(int argc, char* argv[]) {
   	
   	int  a, i, j, x, k, height, maxh, skewed = 0, l, l1, l2, numOfl, f, f1, f2, n, n1, n2;
	long long src1, src2, dest1, dest2, mid1, mid2, filelen, suml, sums, sumd;
	char *datafile, *myf, *myf1, *myf2;
	char cmd1[12], cmd2[12], strsrc1[12], strdest1[12], strsrc2[12], strdest2[12], strpid[12], strrpid[12];					
	char *argv1[argc + 1], *argv2[argc + 1];
	char *largv1[argc + 2], *largv2[argc + 2];
	char *strone = ".1", *strtwo = ".2";
	pid_t rpid, current, proc1, proc2;
	FILE *fp;
	
	fp = NULL;
	
	for(a = 1; a < argc; a++) {		//give arguments in any priority	

	    if(strcmp(argv[a],"-h") == 0) {

			height = atoi(argv[a + 1]);    
		}

	    if(strcmp(argv[a],"-d") == 0) {

			datafile = argv[a + 1];
		}

	    if(strcmp(argv[a],"-s") == 0) {	//if "-s" flag is given by the user execute skew mode                
			
			skewed = 1; 
		}
	}
	
	//store needed arguments    		
	maxh = atoi(argv[argc - 4]);
	numOfl = pow(2.0, maxh);	//number of leaves
	
	rpid = atoi(argv[argc - 3]);
	
	src1 = atoll(argv[argc - 2]);
	dest1 = atoll(argv[argc - 1]);
	
	src2 = atoll(argv[argc - 2]);
	dest2 = atoll(argv[argc - 1]);
		   			   
	fp = fopen(datafile,"rb");
   	
   	if (fp == NULL) {	//if file cannot be open return
   	
      	perror("Cannot open binary file\n");
      	return EXIT_FAILURE;
	}
   	
	fseek (fp, 0, SEEK_END);	//search all file
	filelen = ftell(fp);	//length of binary file
	rewind(fp);	//pointer returns to the beginning of the file
	
	fclose(fp);
					
	snprintf(strrpid, sizeof(strrpid), "%d", rpid);
	
	l = strlen(myfifo) + strlen(strrpid) + 1;
	
	myf = (char* ) malloc(l * sizeof(char));	//allocate memory for root's fifo name
	
	strcpy(myf, myfifo);
	
	myf = strcat(myf, strrpid);	//fifo's name created
	
	current = getpid();	//get current process' pid
	
	snprintf(strpid, sizeof(strpid), "%d", current);
	
	l1 = strlen(myfifo) + strlen(strpid) + strlen(strone) + 1;
	l2 = strlen(myfifo) + strlen(strpid) + strlen(strtwo) + 1;
	
	myf1 = (char* ) malloc(l1 * sizeof(char));	//allocate memory for proc1's fifo name
	myf2 = (char* ) malloc(l2 * sizeof(char));	//allocate memory for proc2's fifo name
	
	strcpy(myf1, myfifo);
	strcpy(myf2, myfifo);
	
	strcat(myf1, strpid);
	strcat(myf2, strpid);
	
	myf1 = strcat(myf1,strone);	//fifo's name created
	myf2 = strcat(myf2,strtwo);	//fifo's name created
	
	
	if(mkfifo(myf1, 0666) == -1) {	//create fifo
		
		if(errno != EEXIST) { 
			
			perror("Couldn't create fifo"); 
			return EXIT_FAILURE; 
		}
	}   
	   
	if(mkfifo(myf2, 0666) == -1) {	//create fifo
		
		if (errno != EEXIST) { 
			
			perror("Couldn't create fifo"); 
			return EXIT_FAILURE; 
		}
	}
	
	proc1 = fork();	//splitter forks his left child
				
	if(proc1 == -1) {

    	fprintf(stderr, "Problem in process %lu: fork: %s\n", (long) getpid(), strerror(errno));
	}

	else if(proc1 == 0) {
	    
		if(height == 1) {	//run leaf executable

			for(i = 0; i < argc + 1; i++) {

				largv1[i] = malloc(12 * sizeof(char*));	// allocate memory for ./Leaf arguments
			}

			strcpy(cmd1, "./Leaf");
			strcpy(largv1[0], "./Leaf");
				
			for(i = 1; i < argc - 2; i++) {	//copy all arguments to the array used is execvp(), except the ones that need to be changed
				
				strcpy(largv1[i], argv[i]);
			}

			if(skewed == 0) {	//if skew mode is off, at each call divide the range to the half of the previous range 
			
				mid1 = src1 + (dest1 - src1)/2;
				dest1 = mid1;
			}

			else if(skewed == 1) {	//if skew mode is on
				
				mid1 = src1 + (dest1 - src1)/2;	
				dest1 = mid1;
				
				//at this point src1 contains an even number which represents a leaf (0,2,4,..)
				//dest1 contains the next number after src1		
				suml = 0;	
				sums = 0;	
				sumd = 0;
				
				for(x = 1; x < numOfl + 1; x++) {	//find the sum of all leaves			
					
					suml += x;
				}

				for(j = 0; j < src1 + 1; j++) {	//find the sum of leaves in numeric order starting from leaf 0 until you reach leaf with number src1
					
					sums += j;
				}

				for(i = 0; i < dest1 + 1; i++) {	//find the sum of leaves in numeric order starting from leaf 0 until you reach leaf with number (src1 + 1)
					
					sumd += i;
				}
				//implement the given types of skewed ranges using bytes and not records
				src1 = (filelen * sums) / suml;	
				dest1 = (filelen * sumd) / suml;
			}
			
			snprintf(strsrc1, sizeof(strsrc1), "%lld", src1);
			snprintf(strdest1, sizeof(strdest1), "%lld", dest1);
			
			//copy changed arguments to the array used is execvp(). Also, pass fifo name as argument
			strcpy(largv1[argc - 2], strsrc1);	
			strcpy(largv1[argc - 1], strdest1);	
			strcpy(largv1[argc], myf1);
			largv1[argc + 1] = (char*) NULL;
												
			execvp(cmd1, largv1);	//run ./Leaf executable
			perror("Execvp failed");
				
			for(j = 0; j < argc + 1; j++) {

				free(largv1[j]);		//free memory of arguments
			}
		}

		else if(height > 1) {
		
			for(i = 0; i < argc; i++) {

				argv1[i] = malloc(12 * sizeof(char*));	// allocate memory for ./Internal arguments
			}

			strcpy(cmd1, "./Internal");
			strcpy(argv1[0], "./Internal");
			
			for(i = 1; i < argc; i++) {	//loop throught arguments
				
				if(strcmp(argv[i], "-h") == 0) {	//if "-h" flag is found
					
					height--;	//reduce height. Next process will be executed one level lower in the binary tree
					sprintf(argv[i + 1], "%d", height);		
				}
			}

			for(i = 1; i < argc - 2; i++) {	//copy all arguments to the array used is execvp(), except the ones that need to be changed
				
				strcpy(argv1[i], argv[i]);
			}
			
			mid1 = src1 + (dest1 - src1)/2;						
			dest1 = mid1;
		
			snprintf(strsrc1, sizeof(strsrc1), "%lld", src1);
			snprintf(strdest1, sizeof(strdest1), "%lld", dest1);
			
			//copy changed arguments to the array used is execvp()
			strcpy(argv1[argc - 2], strsrc1);	
			strcpy(argv1[argc - 1], strdest1);
				
			argv1[argc] = (char*) NULL;

			execvp(cmd1, argv1);
			perror("Execvp failed");
	
			for(j = 0; j < argc; j++) {

				free(argv1[j]);		//free memory of arguments
			}
		}
	} 

	else {

		proc2 = fork();
		
		if(proc1 == -1) {

    		fprintf(stderr, "Problem in process %lu: fork: %s\n", (long) getpid(), strerror(errno));
		}

		else if(proc2 == 0) {
		       
			if(height == 1) {	//run leaf executable
	
				for(i = 0; i < argc + 1; i++) {

					largv2[i] = malloc(12 * sizeof(char*));	// allocate memory for ./Leaf arguments
				}
			
				strcpy(cmd2, "./Leaf");
				strcpy(largv2[0], "./Leaf");
					
				for(i = 1; i < argc - 2; i++) {	//copy all arguments to the array used is execvp(), except the ones that need to be changed
					
					strcpy(largv2[i], argv[i]);
				}

				if(skewed == 0) {	//if skew mode is off, at each call divide the range to the half of the previous range
					
					mid2 = src2 + (dest2 - src2)/2;
					src2 = mid2;
				}

				else if(skewed == 1) {
				
					mid2 = src2 + (dest2 - src2)/2;
					src2 = mid2;
					
					//at this point src2 contains an even number which represents a leaf (0,2,4,..)
					//dest2 contains the next number after src2
					suml = 0;	
					sums = 0;	
					sumd = 0;
					
					for(x = 1; x < numOfl + 1; x++) {	//find the sum of all leaves			
						
						suml += x;
					}

					for(j = 0; j < src2 + 1; j++) {	//find the sum of leaves in numeric order starting from leaf 0 until you reach leaf with number src2
						
						sums += j;
					}

					for(i = 0; i < dest2 + 1; i++) {	//find the sum of leaves in numeric order starting from leaf 0 until you reach leaf with number (src2 + 1)
						
						sumd += i;
					}
					//implement the given types of skewed ranges using bytes and not records
					src2 = (filelen * sums) / suml;	
					dest2 = (filelen * sumd) / suml;
				}
				
				snprintf(strsrc2, sizeof(strsrc2), "%lld", src2);
				snprintf(strdest2, sizeof(strdest2), "%lld", dest2);
				
				//copy changed arguments to the array used is execvp(). Also, pass fifo name as argument
				strcpy(largv2[argc - 2], strsrc2);	
				strcpy(largv2[argc - 1], strdest2);	
				strcpy(largv2[argc], myf2);
				largv2[argc + 1] = (char*) NULL;
														
				execvp(cmd2, largv2);	//run ./Internal executable
				perror("Execvp failed");
				
				for(j = 0; j < argc + 1; j++) {

					free(largv2[j]);		//free memory of arguments
				}
			}

			else if(height > 1) {
				
				for(i = 0; i < argc; i++) {

					argv2[i] = malloc(12 * sizeof(char*));	//allocate memory for ./Internal arguments
				}
					
				strcpy(cmd2, "./Internal");
				strcpy(argv2[0], "./Internal");
					
				for(i = 1; i < argc; i++) {
					
					if (strcmp(argv[i], "-h") == 0) {	//if "-h" flag is found
						
						height--;		//reduce height. Next process will be executed one level lower in the binary tree
						sprintf(argv[i + 1], "%d", height);		
					}
				}
	
				for(i = 1; i < argc - 2; i++) {	//copy all arguments to the array used is execvp(), except the ones that need to be changed
				
					strcpy(argv2[i], argv[i]);
				}
				
				mid2 = src2 + (dest2 - src2)/2;
				src2 = mid2;
			
				snprintf(strsrc2, sizeof(strsrc2), "%lld", src2);
				snprintf(strdest2, sizeof(strdest2), "%lld", dest2);

				//copy changed arguments to the array used is execvp()
				strcpy(argv2[argc - 2], strsrc2);	
				strcpy(argv2[argc - 1], strdest2);	
				argv2[argc] = (char*) NULL;
											
				execvp(cmd2, argv2);	//run ./Internal executable
				perror("Execvp failed");
			
				for(j = 0; j < argc; j++) {

					free(argv2[j]);		//free memory of arguments			
				}
			}											 		
		}

		else {

			if(height == 1) {
				
			   	Employee em;
			   	int numOfe, inl;
			   		
			   	numOfe = filelen / sizeof(em);	//number of employees							
				numOfe /= numOfl;	//max number of employees per leaf	
								
				Employee e1, e2;	
				List *list;
				
				list = NULL;

				f1 = open(myf1 , O_RDONLY , 0);	//read employee records from left child-leaves
			
				for(k = 0; k < numOfe; k++) {	//loop for an average amount of records per leaf
		
					if(n1 = read(f1, &e1, sizeof(e1)) > 0) {	//if a record is found push it in the parent's list
						
						push(&list, e1);
					}
				}
				
				close(f1);
					
				f2 = open(myf2 , O_RDONLY , 0);	//read employee records from right child-leaves
										
				for(k = 0; k < numOfe; k++) {	//loop for an average amount of records per leaf
		
					if(n2 = read(f1, &e2, sizeof(e2)) > 0) {	//if a record is found push it in the parent's list
						
						push(&list, e2);
					}				
				}
				
				close(f2);
						
				inl = count_emp(list);	//get the number of records from both
													
				if((f = open(myf, O_WRONLY)) < 0) {	//open root fifo to send immediately the records from the last splitters to the root  

					perror("Open error"); 
					return EXIT_FAILURE;
				}

				while(list != NULL) {	//traverse throught the list and write every record to root fifo
					
					em.custid = list->e.custid;
			        strcpy(em.LastName, list->e.LastName);
					strcpy(em.FirstName, list->e.FirstName);
					strcpy(em.Street, list->e.Street);
					em.HouseID = list->e.HouseID;
					strcpy(em.City, list->e.City);
					strcpy(em.postcode, list->e.postcode);
					em.amount = list->e.amount;
					
					n = write(f, &em, sizeof(em));
					
					if(n < 0) {
			    
					    perror("Problem in writing");
						return EXIT_FAILURE;
					}
					
					list = list->next;	
				}		
				
				close(f);		
	
				free_list(list);	//free the allocated memory
			}
			
			pid_t pproc1, pproc2;
			int stat1, stat2;
			
			pproc1 = waitpid(proc1, &stat1, 0);
			pproc2 = waitpid(proc2, &stat2, 0);
			
    		if(stat1 == 1 && stat2 == 1) {	
					
				perror("The child processes terminated with an error"); 
				return EXIT_FAILURE;
			}          
    
			if(pproc1 == -1) {
					
				perror("Parent process terminated with an error"); 
				return EXIT_FAILURE;
			}
			
			if(pproc2 == -1) {
			
				perror("Parent process terminated with an error"); 
				return EXIT_FAILURE;
			}
		}									
	}
	
	unlink(myf1);
	unlink(myf2);
	free(myf1);
	free(myf2);
	free(myf);

	return EXIT_SUCCESS;
}

void push(List **head, Employee emp) {	//function to insert a node at the beginging of the list 
    
    List *l = (struct list*) malloc(sizeof(struct list)); //allocate memory for every record in the list
  
    // put in the data of each record
    l->e.custid = emp.custid; 
  	strcpy(l->e.LastName, emp.LastName);
  	strcpy(l->e.FirstName, emp.FirstName);
    strcpy(l->e.Street, emp.Street);
	l->e.HouseID = emp.HouseID;
    strcpy(l->e.City, emp.City);
    strcpy(l->e.postcode, emp.postcode);
    l->e.amount = emp.amount;
    
    l->next = *head;	//next record is now the head 
  
    *head = l;     //head now points to the new node 
} 
 
int count_emp(List *head) { //function to count number of records in list
    
	int counter = 0;  
    List *current = head;  
    
	while(current != NULL) {
	 
        counter++; 
        current = current->next; 
    }	 
    
    return counter; 
}  

void free_list(List *head) {	//function free all records from the list
	
	List *temp;
	
	while(head != NULL) {		//while there are records in the list free them 
   
	   	temp = head;
	   	head = head->next;
	   	free(temp);
	}
}
