#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

#define DB_NAME "wowdb.db"
#define USER_ALREADY_EXISTS 19
#define USER_ALREADY_EXISTS_MSG "That username has already been taken"


// USER

// Create User Table 
#define DB_CREATE_TABLE_USER "CREATE TABLE Users(Username varchar[25] PRIMARY KEY, Password varchar[25]) ;"
// Delete User Table // 
#define DB_DROP_TABLE_USER ("DROP TABLE 'Users'")
// Create User // Send parameters in order
#define DB_CREATE_USER ("insert into Users values('%s','%s');")
// Change Password // Ser New Pass and Username
#define DB_CHANGE_PASSWORD ("UPDATE Users SET Password='%s' WHERE Username='%s'")
// Delete User // Send Username
#define DB_DELETE_USER ("DELETE FROM Users where Userame='%s'")

// CHARACTER 

// Create Character Table
#define DB_CREATE_TABLE_CHARACTER "CREATE TABLE Characters(Owner varchar[25],Name varchar[25] PRIMARY KEY, Lvl int DEFAULT 1, Totalexp int DEFAULT 0,
Currentexp int DEFAULT 0, FOREIGN KEY(Owner) REFERENCES Users(Username) ) ;"
// Delete Character Table // 
#define DB_DROP_TABLE_CHARACTER ("DROP TABLE 'Characters'")
// Create Character // Send parameters in order
#define DB_CREATE_CHARACTER ("insert into Characters values('%s','%s');")
// Add Experience // Send Name as param
#define DB_UPDATE_CHARACTER_EXP ("UPDATE Characters SET,Totalexp=Totalexp+1, Currentexp=Currentexp+1 WHERE Name='%s'")
// Add Level // Send Name as Param -> Exp will reset
#define DB_UPDATE_CHARACTER_LEVEL ("UPDATE Characters SET Level=Level+1, Currentexp='0' WHERE Name='%s'")
// Get Current Exp // Send Name as param
#define DB_GET_CHARACTER_EXP ("SELECT Currentexp FROM Characters WHERE Name='%s'")



// Get a single value // (Test if can send '*') - Var 1 = Field, Var 2 = Table
#define DB_SELECT_FIELD_FROM_TABLE ("SELECT '%s' FROM '%s'") 
// Get a single value with condition // - Var 1 = Field, Var 2 = Table, Var 3 = Field to Compare, Var 4 = Condition
#define DB_SELECT_FIELD_FROM_TABLE_WHERE ("SELECT '%s' FROM '%s' WHERE '%s'='%s'")



#define DB_OPEN_ERROR NULL

#define ADD_USER_PASSED 0
#define ADD_USER_NAME_EXISTS 1
#define CHANGE_PASSWORD_PASSED 2
#define DELETE_USER_PASSED 3
#define TABLE_CREATED 4

#define DB_ERROR 7

#define ADD_EXP_PASSED 8
#define ADD_LEVEL_PASSED 9
#define GET_EXP_PASSED 10


sqlite3* DbOpen(void);
int DbCeckTableExistance(sqlite3* db);
int DbCreateTable();
int DbAddStudent(char name[25], char average[5]);
int DbDeleteStudent (char name[25]);
int DbReadStudents ();
int printRow (void *NotUsed, int argc, char **argv, char **azColName);
int DbDropTable ();
int DbDropDatabase (char databaseName[25]);
int DbUpdateStudent (char currentName[25], char newName[25], char average[5]);
void processRequestDatabase (Request * request);

#endif