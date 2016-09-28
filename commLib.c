#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

char * getadress() {
	
	char buffer[BUFFERSIZE];
	char * string;

	FILE * file;

	file = fopen("commconfig.txt", "r");

	if (file) {

		rewind(file);

		fscanf(file, "Address:%s", buffer);
		string = malloc(strlen(buffer));
		strcpy(string, buffer);

		fclose(file);

		return string;
	}
	else
		return NULL;
}