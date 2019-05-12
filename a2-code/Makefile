CFLAGS 	= -Wall -g
CC	= gcc $(CFLAGS)

PROGRAMS = bl_server	bl_client

all : $(PROGRAMS)

bl_server : bl_server.o server_funcs.o util.o simpio.o blather.h
	$(CC) -o bl_server bl_server.o server_funcs.o util.o simpio.o

bl_client : bl_client.o util.o simpio.o blather.h
	$(CC) -o bl_client bl_client.o util.o simpio.o

simpio.o : simpio.c blather.h
	$(CC) -c simpio.c

util.o : util.c blather.h
	$(CC) -c $<

server_funcs.o : server_funcs.c blather.h
	$(CC) -c $<

bl_client.o : bl_client.c blather.h
	$(CC) -c $<

bl_server.o : bl_server.c blather.h
	$(CC) -c $<

clean : 
	rm -f blather *.o *.fifo $(PROGRAMS)


#Testing

