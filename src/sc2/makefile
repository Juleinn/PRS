CFLAGS=-Wall $(INCLUDE) -lpthread -Wno-unused-variable -O3
CC=gcc
INCLUDE=-I./ -I../common
BIN=../../bin
COMMON=../common

all: serveur2-Renotte_Claes

serveur2-Renotte_Claes: main.o connect.o files.o
	$(CC) $^ -o $(BIN)/serveur2-Renotte_Claes $(CFLAGS)

%.o: %.c %.h
	$(CC) -c $< -o $@ $(CFLAGS)

%.o: $(COMMON)/%.c $(COMMON)/%.h
	$(CC) -c $< -o $@ $(CFLAGS)

%.o: $(COMMON1_3)/%.c $(COMMON1_3)/%.h
		$(CC) -c $< -o $@ $(CFLAGS)

#opens server on public port 2000
run: serveur2-Renotte_Claes
	$(BIN)/serveur2-Renotte_Claes 2000

clean:
	rm *.o
