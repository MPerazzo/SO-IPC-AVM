#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "databaseapi.h"

char[25] gname;

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

int DBDeleteUser (char username[25]){

  sqlite3* db = DbOpen();

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

  sqlite3* db = DbOpen();

  if (db == DB_OPEN_ERROR) {
    return DB_ERROR;
  }

  char *err_msg = 0;
  char query[200];

  sprintf (query, DB_CHANGE_PASSWORD, newPassword, username);
  rc = sqlite3_exec(db, query, 0, 0, &err_msg);

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

  int rc = sqlite3_exec(db, DB_CREATE_CHARACTER_TABLE, 0, 0, &err_msg);
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
  DBGetExperience(name);
  return ADD_EXP_PASSED;
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

int DBGetExperience (char name[25]) {

  sqlite3* db = DBOpen();
  gname = name;

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
  return rc;
}

int levelUp (void* NotUsed, int resc, char **resv) {
  if (resv[0]) {
    if(resv[0] == 10) {
      DBAddLevel(gname);
    }
  }
  return 0;
}
