# To compile there are two options of comm between server and client (fifo or sock), 
# and two between server and Daemon (SharedMem or Queues).

# q is an alias for Queues.
# m is an alias for SharedMemory.

#threads with sharedMem does not work. But it works with queues.

threadsq:
	gcc -o server serverThread.c commlib.c daemonqueues.c commsockets.c marshalling.c databasecomm.c databasemarshalling.c  	  	  semaphores.c -lpthread
	gcc -o client client.c commlib.c commsockets.c marshalling.c APIclient.c
	gcc -o database database.c commlib.c sqlite3.c databasecomm.c databasemarshalling.c semaphores.c -lpthread -ldl
	gcc -o loggingdaemon commlib.c daemonqueues.c daemonserver.c semaphores.c -lpthread
	
socketsq:
	gcc -o server server.c commlib.c daemonqueues.c commsockets.c marshalling.c databasecomm.c databasemarshalling.c
	gcc -o client client.c commlib.c commsockets.c marshalling.c APIclient.c
	gcc -o database database.c commlib.c sqlite3.c databasecomm.c databasemarshalling.c semaphores.c -lpthread -ldl
	gcc -o loggingdaemon commlib.c daemonqueues.c daemonserver.c

fifosq:
	gcc -o server server.c commlib.c daemonqueues.c commfifos.c marshalling.c databasecomm.c databasemarshalling.c
	gcc -o client client.c commlib.c commfifos.c marshalling.c APIclient.c
	gcc -o database database.c commlib.c sqlite3.c databasecomm.c databasemarshalling.c semaphores.c -lpthread -ldl
	gcc -o loggingdaemon commlib.c daemonqueues.c daemonserver.c

socketsm:
	gcc -o server server.c commlib.c daemonSharedmem.c commsockets.c marshalling.c databasecomm.c databasemarshalling.c semaphores.c    		-lpthread
	gcc -o client client.c commlib.c commsockets.c marshalling.c APIclient.c
	gcc -o database database.c commlib.c sqlite3.c databasecomm.c databasemarshalling.c semaphores.c -lpthread -ldl
	gcc -o loggingdaemon commlib.c daemonSharedmem.c daemonserver.c semaphores.c -lpthread

fifosm:
	gcc -o server server.c commlib.c daemonSharedmem.c commfifos.c marshalling.c databasecomm.c databasemarshalling.c semaphores.c    		-lpthread
	gcc -o client client.c commlib.c commfifos.c marshalling.c APIclient.c
	gcc -o database database.c commlib.c sqlite3.c databasecomm.c databasemarshalling.c semaphores.c -lpthread -ldl
	gcc -o loggingdaemon commlib.c daemonSharedmem.c daemonserver.c semaphores.c -lpthread
clean: 
	rm -f client server database loggingdaemon database.db
