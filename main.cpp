/*
Skeleton code for linear hash indexing
*/

#include <string>
#include <ios>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include "classes.h"
using namespace std;


int main(int argc, char* const argv[]) {
	bool ban=true;
	int decision;
	int emplid;
	//create and delete time every time
	char filename[ ] = "EmployeeIndex.txt";
    fstream appendFileToWorkWith;
    appendFileToWorkWith.open(filename,  fstream::in | fstream::out | fstream::trunc); // create/resets.
    appendFileToWorkWith <<"";
    appendFileToWorkWith.close();
	
    // Create the index
    LinearHashIndex emp_index("EmployeeIndex");
    emp_index.createFromFile("Employee.csv");
	while(ban){
		cout << "MENU\n";
		cout << "1. Search an employee by id \n";
		cout << "2. End program \n";
		cin >> decision;
		
		switch(decision){
			case 1:{
				cout << "Please enter the employee id" << endl;
				cin >> emplid;
				cout <<  emplid;
				emp_index.findRecordById(emplid);
				break;
			}
			case 2:{
				ban = false;
				break;
			}default:
				cout << "I don't have that option, Please try again' \n";
			}
	}
	

    return 0;
}
