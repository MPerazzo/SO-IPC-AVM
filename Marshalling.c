#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "data.h"

void Marshall(Datagram * datagram, Data * data) {
  datagram->arguments = calloc(sizeof(Data), 1); 
  memcpy(datagram->arguments, data, sizeof(Data));
}

void unMarshall(Datagram * datagram, Data * data) {
  memcpy(data, datagram->arguments, sizeof(Data));
}
