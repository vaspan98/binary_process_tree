#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#define SIZEofBUFF 20
#define SSizeofBUFF 6

typedef struct employee Employee;

struct employee {		//employee struct
	
	long custid;
	char FirstName[SIZEofBUFF];
	char LastName[SIZEofBUFF];
	char Street[SIZEofBUFF];
	int HouseID;
	char City[SIZEofBUFF];
	char postcode[SSizeofBUFF];
	float amount;
};

#endif
