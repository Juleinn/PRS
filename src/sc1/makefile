CFLAGS=-Wall $(INCLUDE) -lpthread -Wno-unused-variable
CC=gcc
INCLUDE=-I./ -I../common
BIN=../../bin
COMMON=../common

all: serveur1-Renotte_Claes

serveur1-Renotte_Claes: main.o connect.o files.o data_load.o data_send.o
	$(CC) $^ -o $(BIN)/serveur1-Renotte_Claes $(CFLAGS)

%.o: %.c %.h
	$(CC) -c $< -o $@ $(CFLAGS)

%.o: $(COMMON)/%.c $(COMMON)/%.h
	$(CC) -c $< -o $@ $(CFLAGS)

#opens server on public port 2000
run: serveur1-Renotte_Claes
	$(BIN)/serveur1-Renotte_Claes 2000

clean:
	rm *.o
