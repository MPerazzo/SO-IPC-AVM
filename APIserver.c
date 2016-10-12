#include <stdlib.h>

#include "types.h"
#include "databasecomm.h"
#include "daemon.h"

char * getaddress();
Data * db_receiveData(DBConnection *);
Data * communicate_with_database();
void server_close();


Data * loginC(Data * data_from_client) {
	return communicate_with_database(data_from_client);
}

Data * createaccountC(Data * data_from_client) {
	return communicate_with_database(data_from_client);
}

Data * createcharC(Data * data_from_client) {
	return communicate_with_database(data_from_client);
}

Data * deletecharC(Data * data_from_client) {
	return communicate_with_database(data_from_client);
}

Data * showcharC(Data * data_from_client) {
	return communicate_with_database(data_from_client);
}

Data * selectcharC(Data * data_from_client) {
	return communicate_with_database(data_from_client);
}

Data * expUpC(Data * data_from_client) {
	return communicate_with_database(data_from_client);
}

Data * logoutC(Data * data_from_client) {
	return communicate_with_database(data_from_client);
}

Data * communicate_with_database(Data * data_from_client) {

	DBConnection * db_connection;

	db_connection = db_comm_connect(getaddress("DBSV"));

	if(db_connection == NULL) {

		sndMessage("couldn't connect to database", ERROR_TYPE);

		return NULL;
	}

	if (db_sendData(db_connection, data_from_client)<0) {
		
		sndMessage("couldn't send data to database", ERROR_TYPE);
		
		return NULL;
	}

	Data * data_to_client = db_receiveData(db_connection);

	if (data_to_client == NULL) {
		
		sndMessage("couldn't receive data from database", ERROR_TYPE);
		
		return NULL;
	}

	db_comm_disconnect(db_connection);

	return data_to_client;
}