#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <string.h>
#include <pthread.h>

#include "sqlite3.h"
#include "databasecomm.h"
#include "types.h"
#include "constants.h"

void login();
void create_account();
void create_char();
void select_char();
void delete_char();
void logout();
void char_exp_up();

int select_callback(void *, int, char **, char **);
int delete_callback(void *, int, char **, char **);
int login_callback(void *, int, char **, char **);
int create_account_callback(void *, int, char **, char **);

char * getaddress(char *);

void initialize_table();
void process_data();

DBListener * db_comm_listen(char *);
Data * db_receiveData(DBConnection *);

sqlite3 *db;
char *err_msg = 0;
char query[200];
int rc;
int semaphore_id;
DBListener * db_listener;
DBConnection * db_connection;
Data * data;

pthread_mutex_t mutex;

int main(void) {

    printf("[database] initializing\n");
    
    initialize_table();

    db_listener = db_comm_listen(getaddress("DBSV"));

    pthread_mutex_init(&mutex, NULL);

<<<<<<< HEAD
=======
    //semaphore_id = binary_semaphore_allocation (666, IPC_RMID);

    //binary_semaphore_initialize (semaphore_id);

>>>>>>> de2be990841ce48ed0c92a66771a3aea4109efe4
    printf("[database] awaiting connections\n");

    while(1) {

        db_connection = db_comm_accept(db_listener);

        data = db_receiveData(db_connection);

        process_data();

        db_sendData(db_connection, data);

        db_comm_disconnect(db_connection);

    }
<<<<<<< HEAD
=======

    //binary_semaphore_deallocate(semaphore_id);
>>>>>>> de2be990841ce48ed0c92a66771a3aea4109efe4
    
    sqlite3_close(db);
    
    return 0;

}

void initialize_table() {

    unlink("/tmp/database.db");

    int rc = sqlite3_open("/tmp/database.db", &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return;
    }

    char sql[200];

    strcpy(sql, "CREATE TABLE User(Name TEXT PRIMARY KEY, Password TEXT, CantChar INTEGER, Inuse INTEGER)");
    
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to create table\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    strcpy(sql, "CREATE TABLE Chars(Id TEXT, Level INTEGER, TotalExp INTEGER, CurrentExp INTEGER, Inuse INTEGER, Name TEXT, PRIMARY KEY(Id), FOREIGN KEY(Name) REFERENCES User(Name) ON DELETE CASCADE);");

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to create table\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

}

void process_data() {

    pthread_mutex_lock(&mutex);

    if(data->opcode == LOGIN) {

        printf("[database] processing LOGIN request from server session %d\n", data->sender_pid);

        login();

    } 

    else if(data->opcode == CREATE_ACCOUNT){

        printf("[database] processing CREATE_ACCOUNT request from server session %d\n", data->sender_pid);

        create_account();

    } 

    else if(data->opcode == SELECT_CHARACTER) {

        printf("[database] processing SELECT_CHARACTER request from server session %d\n", data->sender_pid);

        select_char();

    } 

    else if(data->opcode == CREATE_CHARACTER) {

        printf("[database] processing CREATE_CHARACTER request from server session %d\n", data->sender_pid);

        create_char();

    }

    else if(data->opcode == DELETE_CHARACTER) {

        printf("[database] processing DELETE_CHARACTER request from server session %d\n", data->sender_pid);

        delete_char();
        
    }
    
    else if(data->opcode == SHOW_CHARACTER) {

        printf("[database] processing SHOW_CHARACTER request from server session %d\n", data->sender_pid);

        //show_character();
        
    }

    else if(data->opcode == EXP_UP) {

        printf("[database] processing EXP_UP request from server session %d\n", data->sender_pid);

        char_exp_up();

    }

    else if(data->opcode == EXIT_AND_LOGOUT) {

        printf("[database] processing EXIT_AND_LOGOUT request from server session %d\n", data->sender_pid);

        logout();

    } 

    else if(data->opcode == LOGOUT) {

        printf("[database] processing LOGOUT request from server session %d\n", data->sender_pid);

        logout();
    }

    pthread_mutex_unlock(&mutex);
}

void login() {
    printf("%s, %s\n", data->user.username, data->user.password);
    sprintf(query, "SELECT * FROM User WHERE name = '%s' AND password = '%s'", data->user.username, data->user.password);

    rc = sqlite3_exec(db, query, login_callback, 0, &err_msg);

    if(data->opcode == LOGIN) {

        data->opcode = USER_DOESNT_EXIST;

    }

}

int login_callback(void *NotUsed, int argc, char **argv, char **azColName) {

    if(atoi(argv[3]) == 1) {

        data->opcode = USER_BUSY;

    } else {

        sprintf(query, "UPDATE User SET Inuse=1 WHERE Name='%s'", data->user.username);

        sqlite3_exec(db, query, 0, 0, &err_msg);
        strcpy(data->user.username, argv[0]);
        strcpy(data->user.password, argv[1]);
        data->opcode = NO_ERROR;
    }

    return 0;
}

void create_account() {
    printf("%s, %s\n", data->user.username, data->user.password);
    sprintf(query, "INSERT INTO User VALUES('%s', '%s', 0, 0)", data->user.username, data->user.password);

    rc = sqlite3_exec(db, query, login_callback, 0, &err_msg);

    if (rc != SQLITE_OK ) {

        data->opcode = USER_ALREADY_EXISTS;
        
        return ;

    } 

    data->opcode = NO_ERROR;

}

int create_account_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    return atoi(argv[2]);
}

void create_char() {
    printf("%s, %s\n", data->user.username, data->character.name);
    sprintf(query, "SELECT * FROM User WHERE Name = '%s'", data->user.username);   

    rc = sqlite3_exec(db, query, create_account_callback, 0, &err_msg);

    if(rc == MAX_CHARACTERS) {

        data->opcode = TOO_MANY_CHARS;

        return;
    }

    int cant = rc + 1;

    sprintf(query, "INSERT INTO Chars VALUES('%s', 1, 10, 0, 0, '%s')", data->character.name, data->user.username);   

    rc = sqlite3_exec(db, query, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {

        data->opcode = CHAR_ALREADY_EXISTS;
        
        return ;

    }

    sprintf(query, "UPDATE User SET CantChar=%d WHERE Name='%s'", cant, data->user.username);

    rc = sqlite3_exec(db, query, create_account_callback, 0, &err_msg);

    data->opcode = NO_ERROR;

}

void select_char() {
    printf("%s, %s\n", data->user.username, data->character.name);
    sprintf(query, "SELECT * FROM Chars WHERE Id='%s' AND Name = '%s'", data->character.name, data->user.username);

    rc = sqlite3_exec(db, query, select_callback, 0, &err_msg);

    if(data->opcode == SELECT_CHARACTER) {

        data->opcode = CHAR_DOESNT_EXIST;

    }

}

int select_callback(void *NotUsed, int argc, char **argv, char **azColName) {

    if(atoi(argv[4]) == 1) {

        data->opcode = CHAR_BUSY;

    } else {

        sprintf(query, "UPDATE Chars SET Inuse=1 WHERE Id='%s'", data->character.name);

        sqlite3_exec(db, query, 0, 0, &err_msg);

        strcpy(data->character.name, argv[0]);
        data->character.lvl = atoi(argv[1]);
        data->character.totalExp = atoi(argv[2]);
        data->character.currentExp = atoi(argv[3]);

        data->opcode = NO_ERROR;
    }

    return 0;
}

void delete_char() {
    printf("%s, %s\n", data->user.username, data->character.name);
    sprintf(query, "SELECT * FROM Chars WHERE Id='%s' AND Name='%s'", data->character.name, data->user.username);

    rc = sqlite3_exec(db, query, delete_callback, 0, &err_msg);

    if(data->opcode == DELETE_CHARACTER) {

        data->opcode = CHAR_DOESNT_EXIST;

    }

}

int delete_callback(void *NotUsed, int argc, char **argv, char **azColName) {

    if(atoi(argv[4]) == 1) {

        data->opcode = CHAR_BUSY;

    } else {

        sprintf(query, "DELETE FROM Chars WHERE Id='%s'", data->character.name);

        sqlite3_exec(db, query, 0, 0, &err_msg);

        data->opcode = NO_ERROR;
    }

    return 0;
}

void logout() {
    printf("%s\n", data->user.username);
    sprintf(query, "UPDATE Chars SET Inuse=0 WHERE Name='%s'", data->user.username);

    sqlite3_exec(db, query, 0, 0, &err_msg);

    sprintf(query, "UPDATE User SET Inuse=0 WHERE Name='%s'", data->user.username);

    sqlite3_exec(db, query, 0, 0, &err_msg);

    data->opcode = NO_ERROR;

}

void char_exp_up() {
    printf("%s, %d, %d, %d\n", data->character.name, data->character.lvl, data->character.totalExp, data->character.currentExp);
    sprintf(query, "UPDATE Chars SET Level=%d, TotalExp=%d, CurrentExp=%d WHERE Id='%s'", data->character.lvl, data->character.totalExp, data->character.currentExp, data->character.name);
    sqlite3_exec(db, query, 0, 0, &err_msg);

    data->opcode = NO_ERROR;

}