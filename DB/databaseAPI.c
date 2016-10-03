#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "databaseAPI.h"

char gname[25];
bool login = FALSE;

sqlite3* DBOpen(void) {
  sqlite3* db;
  int rc = sqlite3_open(DB_NAME, &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return DB_OPEN_ERROR;
  }
  return db;
}

int DBCreateUserTable() {

  sqlite3* db = DBOpen();

  if (db == DB_OPEN_ERROR) {
  	return DB_ERROR;
  }

  char *err_msg = 0;

  int rc = sqlite3_exec(db, DB_CREATE_TABLE_USER, 0, 0, &err_msg);
  if (rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", err_msg);
    //fprintf(stderr, "SQL error: %d\n", rc);
    sqlite3_free(err_msg);
    return DB_ERROR;
  }
  return TABLE_CREATED;
}

int DBNewUser(char username[25], char password[5]) {

  sqlite3* db = DBOpen();

  if (db == DB_OPEN_ERROR) {
    return DB_ERROR;
  }

	char *err_msg = 0;
  char query[200];

  sprintf (query, DB_CREATE_USER, username, password);
	int rc = sqlite3_exec(db, query, 0, 0, &err_msg);

	if (rc != SQLITE_OK ) {
    if ( rc == USER_ALREADY_EXISTS ) {
      printf("%s\n", USER_ALREADY_EXISTS_MSG);
      return ADD_USER_NAME_EXISTS;
    } else {
      fprintf(stderr, "SQL error: %s\n", err_msg);
      //fprintf(stderr, "SQL error: %d\n", rc);
      return DB_ERROR;
    }
		sqlite3_free(err_msg);
		sqlite3_close(db);
	}
	sqlite3_close(db);
	return ADD_USER_PASSED;
}

int DBDeleteUser (char username[25]) {

  sqlite3* db = DBOpen();

  if (db == DB_OPEN_ERROR) {
    return DB_ERROR;
  }

  char *err_msg = 0;
  char query[200];

  sprintf (query, DB_DELETE_USER, username);
  int rc = sqlite3_exec(db, query, 0, 0, &err_msg);

  if (rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", err_msg);
    //fprintf(stderr, "SQL error: %d\n", rc);
    sqlite3_free(err_msg);
    sqlite3_close(db);
  }

  sqlite3_close(db);
  return DELETE_USER_PASSED;
}

int DBChangeUserPassword (char username[25], char newPassword[25]) {

  sqlite3* db = DBOpen();

  if (db == DB_OPEN_ERROR) {
    return DB_ERROR;
  }

  char *err_msg = 0;
  char query[200];

  sprintf (query, DB_CHANGE_PASSWORD, newPassword, username);
  int rc = sqlite3_exec(db, query, 0, 0, &err_msg);

  if (rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", err_msg);
    //fprintf(stderr, "SQL error: %d\n", rc);
    sqlite3_free(err_msg);
    sqlite3_close(db);
  }

  sqlite3_close(db);
  return CHANGE_PASSWORD_PASSED;
}


int DBCreateCharacterTable() {

  sqlite3* db = DBOpen();

  if (db == DB_OPEN_ERROR) {
    return DB_ERROR;
  }

  char *err_msg = 0;

  int rc = sqlite3_exec(db, DB_CREATE_TABLE_CHARACTER, 0, 0, &err_msg);
  if (rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", err_msg);
    //fprintf(stderr, "SQL error: %d\n", rc);
    sqlite3_free(err_msg);
    return DB_ERROR;
  }
  return TABLE_CREATED;
}

int DBNewCharacter(char username[25], char name[5]) {

  sqlite3* db = DBOpen();

  if (db == DB_OPEN_ERROR) {
    return DB_ERROR;
  }

  char *err_msg = 0;
  char query[200];

  sprintf (query, DB_CREATE_CHARACTER, username, name);
  int rc = sqlite3_exec(db, query, 0, 0, &err_msg);

  if (rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", err_msg);
    sqlite3_free(err_msg);
    sqlite3_close(db);
    return DB_ERROR;
  }
  sqlite3_close(db);
  return ADD_EXP_PASSED;
}


int DBAddExp (char name[25]) {

  sqlite3* db = DBOpen();

  if (db == DB_OPEN_ERROR) {
    return DB_ERROR;
  }

  char *err_msg = 0;
  char query[200];

  sprintf (query, DB_UPDATE_CHARACTER_EXP, name);
  int rc = sqlite3_exec(db, query, 0, 0, &err_msg);

  if (rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", err_msg);
    sqlite3_free(err_msg);
    sqlite3_close(db);
    return DB_ERROR;
  }

  sqlite3_close(db);
  if( DBGetExperience(name) == GET_EXP_PASSED) {
    return ADD_EXP_PASSED;
  } else {
    return DB_ERROR;
  }
}

int DBAddLevel (char name[25]) {

  sqlite3* db = DBOpen();

  if (db == DB_OPEN_ERROR) {
    return DB_ERROR;
  }

  char *err_msg = 0;
  char query[200];

  sprintf (query, DB_UPDATE_CHARACTER_LEVEL, name);
  int rc = sqlite3_exec(db, query, 0, 0, &err_msg);

  if (rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", err_msg);
    sqlite3_free(err_msg);
    sqlite3_close(db);
    return DB_ERROR;
  }

  sqlite3_close(db);
  return ADD_LEVEL_PASSED;
}

int levelUp (void* NotUsed, int resc, char **resv, char **colName) {
  if (resv[0]) {
    if(resv[0] == 10) {
      DBAddLevel(gname);
    }
  }
  return 0;
}

int DBGetExperience (char name[25]) {

  sqlite3* db = DBOpen();
  strcpy(name, gname);

  if (db == DB_OPEN_ERROR) {
    return DB_ERROR;
  }

  char *err_msg = 0;
  char query[200];

  sprintf (query, DB_GET_CHARACTER_EXP, name);
  int rc = sqlite3_exec(db, query, levelUp, 0, &err_msg);

  if (rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", err_msg);
    sqlite3_free(err_msg);
    sqlite3_close(db);
    return DB_ERROR;
  }

  sqlite3_close(db);
  return GET_EXP_PASSED;
}


//
#define DB_GET_LOGIN ("SELECT * FROM Users WHERE Username='%s' AND Password='%s'")
// Get Character Info
#define DB_GET_SINGLE_CHARACTER ("SELECT * FROM Characters WHERE Owner='%s' AND Name='%s'")
// Get Character List Info
#define DB_GET_CHARACTER_LIST ("SELECT * FROM Characters WHERE Owner='%s'")
// Uptate Character Info
#define DB_UPDATE_CHARACTER ("UPDATE Characters SET Level='%s', Currentexp='%s', Totalexp='%s' WHERE Name='%s'")

int checkLogin (void* NotUsed, int resc, char **resv, char **colName) {
  if (resc > 0) {
    login = TRUE;
  }
  return 0;
}

int DBGetUser (char username[25], char password[25]) {

  sqlite3* db = DBOpen();

  if (db == DB_OPEN_ERROR) {
    return DB_ERROR;
  }

  char *err_msg = 0;
  char query[200];

  sprintf (query, DB_GET_LOGIN, username, password);
  int rc = sqlite3_exec(db, query, checkLogin, 0, &err_msg);

  if (rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", err_msg);
    sqlite3_free(err_msg);
    sqlite3_close(db);
    return DB_ERROR;
  }

  sqlite3_close(db);
  if (login) {
    fprintf(stderr, "SQL error: %s\n", err_msg);
    return USER_REGISTERED;
  } else {
    return USER_NOT_REGISTERED;
  }
}

int DBChangeCharacterInfo (char name[25], int lvl, int currentexp, int totalexp) {

  sqlite3* db = DBOpen();

  if (db == DB_OPEN_ERROR) {
    return DB_ERROR;
  }

  char *err_msg = 0;
  char query[200];

  sprintf (query, DB_UPDATE_CHARACTER, lvl, currentexp, totalexp, name);
  int rc = sqlite3_exec(db, query, 0, 0, &err_msg);

  if (rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", err_msg);
    //fprintf(stderr, "SQL error: %d\n", rc);
    sqlite3_free(err_msg);
    sqlite3_close(db);
  }

  sqlite3_close(db);
  return UPDATE_CHARACTER_PASSED;
}