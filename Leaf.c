#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>

#include "Employee.h"
#include "List.h"

char *myfifo = "/temp/myfifo";


int main(int argc, char*argv[]) {

	Employee emp;
	long fppos;
   	int filelen, a, i, f, n;
   	long src, dest, numOfemp;
	char patcustid[12], patHouseID[12], patamount[12];
	char *datafile, *pattern;	
 	pid_t rpid;
	FILE *fp, *rfp;
	
	rfp = NULL;
    fp = NULL;
    
	for(a = 1; a < argc; a++) {		//give arguments in any priority
		
        if(strcmp(argv[a],"-d") == 0) {

			datafile = argv[a + 1];    
        }

        if(strcmp(argv[a],"-p") == 0) {

			pattern = argv[a + 1];
		}    
	}    
	//store needed arguments 
	rpid = atoi(argv[argc - 4]);
	src = atol(argv[argc - 3]);
	dest = atol(argv[argc - 2]);
	
	rfp = fopen (datafile,"rb");
   	
	if(rfp == NULL) {	//if file cannot be open return
   	
      	perror("Cannot open binary file\n");
      	return EXIT_FAILURE;
	}
   	
    fseek(rfp, 0, SEEK_END);
	filelen = ftell(rfp);		//length of binary file

	fclose(rfp);
		
	if(src % sizeof(emp) != 0) {	//arrange the bytes, if need, so that the file pointer doesn't miss any records
		
		while(1) {
		
			src += 1;
			
			if(dest != filelen) {	//don't read unknown bytes
				
				dest += 1;
			}

			if(src % sizeof(emp) == 0) {

				break;
			}
		}
	}	

	fp = fopen(datafile,"rb");
   	
   	if(fp == NULL) {	//if file cannot be open return
   	
      	perror("Cannot open binary file\n");
      	return EXIT_FAILURE;
	}
   	
	fseek(fp, src, SEEK_CUR);
	fppos = ftell(fp);		//position of file pointer at each leaf execution
   	numOfemp = (long) (dest - src)/sizeof(emp);	//number of employee records

	if((f=open(argv[argc - 1], O_WRONLY)) < 0) { 
	
		perror("Open error"); 
		return EXIT_FAILURE; 
	}
	
 	for(i = 0; i < numOfemp ; i++) {	//read a predetermined number of records from file
   		
    	fread(&emp, sizeof(emp), 1, fp);
    	
		//convert everything to string so it can be given as array argument
		snprintf(patcustid, sizeof(patcustid), "%ld", emp.custid);
		snprintf(patHouseID, sizeof(patHouseID), "%d", emp.HouseID);
		snprintf(patamount, sizeof(patamount), "%f", emp.amount);	
		
		//check if pattern is substring of any record
		if(strstr(patcustid, pattern) != NULL || strstr(emp.LastName, pattern) != NULL || strstr(emp.FirstName, pattern) != NULL || 
			strstr(emp.Street, pattern) != NULL || strstr(patHouseID, pattern) != NULL || strstr(emp.City, pattern) != NULL || 
			strstr(emp.postcode, pattern) != NULL || strstr(patamount, pattern) != NULL) {
    		
			if(n = write(f, &emp, sizeof(emp)) == -1) { //if pattern matches, write record to fifo
  	
				perror("Error in Writing"); 
				return EXIT_FAILURE; 
			}
		}
	}
	
	close(f);
	fclose(fp);
	
	kill(rpid, SIGUSR2);	//leaf sends a signal to root to inform him that its execution ended
}
