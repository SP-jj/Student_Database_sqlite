// Name: Stef Pollo
// Date: 16/June/2021
// Program Name: main.cpp
// Purpose: This program uses sqlite to manage a database of students, "StudentData.db". 
//          If it cannot locate the database when it is run, it creates a new one.

#include "sqlite3.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// int tutorial()
// {
// 	// declare database
// 	sqlite3* db;

// 	// Think of each SQL statement as a separate computer program. The original SQL text is source code.
// 	// A prepared statement object is the compiled object code. 
// 	// All SQL must be converted into a prepared statement before it can be run.
// 	sqlite3_stmt* statement;

// 	// create database
// 	sqlite3_open("tutorialDatabase.db", &db);

// 	char* err;
// 	int rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS table1(x1 INT, x2 INT, name varchar(100));", NULL, NULL, &err);

// 	if(rc != SQLITE_OK)
// 	{
// 		cout << "error: " << err;
// 	}

// 	for (int i = 0; i < 10; i++)
// 	{
// 		string query = "insert into table1 VALUES ("+to_string(i) +','+ to_string(i+5)+", 'stef');";
// 		cout << query << '\n';
// 		rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
// 		if(rc != SQLITE_OK)
// 		{
// 			cout << "insert error error: " << err;
// 		}
// 	}

// 	//prepares statement to get rows from the table
// 	sqlite3_prepare_v2(db, "select x1, x2, name from table1", -1, &statement, 0);
// 	int a1, a2;
// 	const unsigned char* myName;

// 	// sqlite3_step *steps* trought each row in the select query
// 	while(sqlite3_step(statement) != SQLITE_DONE)
// 	{	
// 		// get value in first column(index 0)
// 		a1 =  sqlite3_column_int(statement, 0);
// 		// value in second column
// 		a2 =  sqlite3_column_int(statement, 1);
// 		//value in third column
// 		myName = sqlite3_column_text(statement, 2);
// 		cout << "a1 = "<< a1 << "   a2 = "<< a2 <<'\n';
// 	}

// 	sqlite3_close(db);


// 	return 0;
// }

int getValidId(string prompt)
{
	cout << prompt;
	int input{};
	cin >> input;
	while(cin.fail() || to_string(input).length() != 9)
	{	
		cout << "\nPlease enter a 9-digit integer id.\n" << prompt; //the prompt is expected to have its own \n(if it needs one)
		cin.clear(); 
		cin.ignore(10000,'\n');
		cin >> input;
	}
	return input;
}

vector<int> getAllIds()
{
	sqlite3* db;
	sqlite3_stmt *statement;
	sqlite3_open("StudentData.db", &db);

	// selects ids of all students and puts them in vector array
	sqlite3_prepare_v2(db, "SELECT id FROM students", -1, &statement, 0);

	vector<int> ids;
	while(sqlite3_step(statement) == SQLITE_ROW)
	{
		ids.push_back( sqlite3_column_int(statement, 0) );
	}

	sqlite3_finalize(statement);
	sqlite3_close(db);

	return ids;
}


void addStudent()
{
	sqlite3* db;
	sqlite3_open("StudentData.db", &db);
	char* err;

	string fName, lName, major;
	double id;
	cout << "\nFirst Name: ";
	cin >> fName;
	cout << "\nLast Name: ";
	cin >> lName;
	cout << "\nMajor: ";
	cin >> major;
enterID:
	id = getValidId("ID: ");

	// check if id has already been assigned to a student
	// if id is repeated, user gets asked to enter another id
	vector<int> allStudentIDs = getAllIds();
	for(int i=0; i < allStudentIDs.size();i++)
	{
		if (id == allStudentIDs[i])
		{
			cout << "That id has already been assigned to a student.\nEnter another ID.\n\n";
			goto enterID;
		}
	}

	string query = "INSERT INTO students VALUES ('"+to_string(id)+"','"+fName+"','"+lName+"','"+major+"');";
	//cout << "query: " << query << '\n';
	
	int rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
	if(rc != SQLITE_OK)
	{
		cout << "error: " << err << '\n';
	}

	cout << "Student Added to Database Successfully\n";
	sqlite3_close(db);
}

void searchStudents()
{
	sqlite3* db;
	sqlite3_stmt *statement;
	sqlite3_open("StudentData.db", &db);
	int printCmd{};
	string printMenu = R"(
---------------------SEARCH STUDENTS BY...---------------------
        1 - ID
        2 - First Name
        3 - Last Name
        4 - Major
        5 - Back to Main Menu
--------------------------------------------------------------
)";
	while(printCmd != 5)
	{
		cout << printMenu;
		cin >> printCmd;
		while(cin.fail() || printCmd < 1 || printCmd > 5)
		{	
			cout << "\nPlease select one of the specified commands.\n" << printMenu; //the prompt is expected to have its own \n(if it needs one)
			cin.clear(); 
			cin.ignore(10000,'\n');
			cin >> printCmd;
		}

		string query{};
		if(printCmd == 1)
		{
			double idInput = getValidId("Enter the student's id:  ");
			query = "SELECT * FROM students WHERE id = "+to_string(idInput);
		} 
		else if(printCmd == 2)
		{
			string fName{};
			cout << "Enter the student's first name:  ";
			cin >> fName;
			query = "SELECT * FROM students WHERE firstName = '"+fName+"'";
		}
		else if(printCmd == 3)
		{
			string lName{};
			cout << "Enter the student's last name:  ";
			cin >> lName;
			query = "SELECT * FROM students WHERE lastName = '"+lName+"'";
		}
		else if(printCmd == 4)
		{
			string major{};
			cout << "Enter the major:  ";
			cin >> major;
			query = "SELECT * FROM students WHERE major = '"+major+"'";
		}
		else if(printCmd == 5)
		{
			continue;
		}

		//cout << "query: " << query << '\n';
		sqlite3_prepare_v2(db, query.c_str(), -1, &statement, 0);

		cout << "\n\nAll found matches:\n\n";
	    cout << "\n- - - - - - - - - - - - - - - - - - - - - - - -\n";

		int count{};
		int step;
		while((step = sqlite3_step(statement)) == SQLITE_ROW)
		{
			count++;
			printf("Name:\t%s %s\nMajor:\t%s\nID:\t%i",
			sqlite3_column_text(statement, 1),
			sqlite3_column_text(statement, 2),
			sqlite3_column_text(statement, 3),
			sqlite3_column_int(statement, 0));
			cout << "\n- - - - - - - - - - - - - - - - - - - - - - - -\n";
		}
		// if there is an error
		if(step != SQLITE_DONE)
		{
			cout << "There seems to be an issue with the connection to the database.\nGo back to the main menu and then try searching for the student(s) again.\n";
			if(step == SQLITE_BUSY)
				cout << "The connection has been overloaded with too many tasks.\n";
		}
		else if(count==0)
			cout << "\nThere are no students in the database that match your search.";
		cout << "\n\n\n";
		sqlite3_finalize(statement);
		sqlite3_close(db);
	}
}

void printAllStudents()
{
	sqlite3_stmt* statement;
	char* err;

	sqlite3* db;
	sqlite3_open("StudentData.db", &db);

	string query = "SELECT * FROM students";

	sqlite3_prepare_v2(db, query.c_str(), -1, &statement, 0);

	cout << "\n- - - - - - - - - - - - - - - - - - - - - - - -\n";

	int count{};
	while(sqlite3_step(statement) == SQLITE_ROW)
	{
		count++;
		printf("Name:\t%s %s\nMajor:\t%s\nID:\t%i",
		sqlite3_column_text(statement, 1),
		sqlite3_column_text(statement, 2),
		sqlite3_column_text(statement, 3),
		sqlite3_column_int(statement, 0));
		cout << "\n- - - - - - - - - - - - - - - - - - - - - - - -\n";
	}
	if(count==0)
		cout << "\nThere are currently no students in the database";
	cout << "\n\n\n";
	sqlite3_finalize(statement);
	sqlite3_close(db);
}

void deleteStudent()
{
	sqlite3* db;
	sqlite3_stmt *statement;
	char* err;
	sqlite3_open("StudentData.db", &db);
	int delCmd{};
	string delMenu = R"(
----------------SELECT STUDENT TO DELETE BY...----------------
        1 - ID
        2 - First Name
        3 - Last Name
        4 - Back to Main Menu
--------------------------------------------------------------
)";
	while(delCmd != 4)
	{
		cout << delMenu;
		cin >> delCmd;
		while(cin.fail() || delCmd < 1 || delCmd > 4)
		{	
			cout << "\nPlease select one of the specified commands.\n" << delMenu; //the prompt is expected to have its own \n(if it needs one)
			cin.clear(); 
			cin.ignore(10000,'\n');
			cin >> delCmd;
		}

		string searchCondition{};

		if(delCmd == 1)
		{
			double idInput = getValidId("Enter the student's id:  ");
			searchCondition = "id = "+to_string(idInput);
		} 
		else if(delCmd == 2)
		{
			string fName{};
			cout << "Enter the student's first name:  ";
			cin >> fName;
			searchCondition = "firstName = '"+fName+"'";
		}
		else if(delCmd == 3)
		{
			string lName{};
			cout << "Enter the student's last name:  ";
			cin >> lName;
			searchCondition = "lastName = '"+lName+"'";
		}
		else if(delCmd == 4)
		{
			//exits out of this iteration of the while loop, and since delCmd is 4, the while loop's 
			// condition is false
			continue;
		}

		string selectQuery = "SELECT * FROM students WHERE "+searchCondition;
		string delQuery = "DELETE FROM students WHERE "+searchCondition+" LIMIT 1";

	searchForStudent:
		sqlite3_prepare_v2(db, selectQuery.c_str() , -1, &statement, 0);

		cout << "\n- - - - - - - - - - - - - - - - - - - - - - - -\n";
		int possibleMatches{}, step;
		while((step = sqlite3_step(statement)) == SQLITE_ROW)
		{
			possibleMatches++;
			printf("Name:\t%s %s\nMajor:\t%s\nID:\t%i",
			sqlite3_column_text(statement, 1),
			sqlite3_column_text(statement, 2),
			sqlite3_column_text(statement, 3),
			sqlite3_column_int(statement, 0));
			cout << "\n- - - - - - - - - - - - - - - - - - - - - - - -\n";
		}
			// if there is an error
		if(step != SQLITE_DONE)
		{
			cout << "There seems to be an issue with the connection to the database.\nGo back to the main menu and then try searching for the student(s) again.\n";
			if(step == SQLITE_BUSY)
				cout << "The connection has been overloaded with too many tasks.\n";
			continue;
		}
		else if(possibleMatches > 1)
		{
			cout << "\nMore than 1 possible matches found.\nPlease specify the id of the student whom you wish to delete:\n";
			int id = getValidId("ID:  ");
			string searchCondition2 = "id = "+to_string(id);
			selectQuery = "SELECT * FROM students WHERE "+searchCondition+ " AND "+searchCondition2;
			delQuery = "DELETE FROM students WHERE "+searchCondition+" AND "+searchCondition2+" LIMIT 1";
			sqlite3_finalize(statement);
			goto searchForStudent;
		}
		else if (possibleMatches == 0)
		{
			// go back to delete menu
			cout << "\nNo matches found\n";
			continue;
		}
		// only possible possibleMatches past this point is 1
		//cout << "\nonly one match found!\n";

		// cout << "delquery: " << delQuery << '\n';

		int rc = sqlite3_exec(db, delQuery.c_str(), NULL, NULL, &err);
		if(rc != SQLITE_OK)
		{
			cout << "Delete Error: " << err;
		}
		else
		{
			cout << "Student Deleted Successfully\n";
		}
		
	}

	sqlite3_close(db);
}



int main()
{
	// setup database if it hasnt been created yet
	sqlite3* db;
	char* err;
	sqlite3_open("StudentData.db", &db);
	int rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS students(id INT,firstName TEXT,lastName TEXT,major TEXT);", NULL, NULL, &err);
	sqlite3_close(db);


	int cmd{};
	string mainMenu = R"(
---------------------MAIN MENU---------------------
        1 - Add students to file
        2 - Print all students
        3 - Search students
        4 - Delete student
        5 - Exit program
---------------------------------------------------
Please enter the corresponding number of your command.

)";
	while( cmd != 5)
	{
		// get one of the valid commands(1-6) from user
		cout << mainMenu;
		cin >> cmd;
		while(cin.fail() || cmd < 1 || cmd > 5)
		{	
			cout << "\nPlease select one of the specified commands.\n" << mainMenu; //the prompt is expected to have its own \n(if it needs one)
			cin.clear(); 
			cin.ignore(10000,'\n');
			cin >> cmd;
		}

		// handle case for each command
		switch (cmd)
		{
		case 1:
			addStudent();
			break;
		case 2:
			printAllStudents();
			break;
		case 3:
			searchStudents();
			break;
		case 4:
			deleteStudent();
			break;
		default:
			break;
		}
	}

	cout << "Program Terminated\n";

	return 0;
}


