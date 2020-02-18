#ifndef LIST_H
#define LIST_H

#include "Employee.h"

typedef struct list List;

struct list {		//linked list to store employee records 
    
    Employee e; 
    struct list* next; 
}; 

#endif
