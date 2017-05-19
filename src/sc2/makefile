CFLAGS=-Wall $(INCLUDE) -lpthread -Wno-unused-variable -O3
CC=gcc
INCLUDE=-I./ -I../common
BIN=../../bin
COMMON=../common

all: serveur2-SemiCroustillants

serveur2-SemiCroustillants: main.o connect.o files.o
	$(CC) $^ -o $(BIN)/serveur2-SemiCroustillants $(CFLAGS)

%.o: %.c %.h
	$(CC) -c $< -o $@ $(CFLAGS)

%.o: $(COMMON)/%.c $(COMMON)/%.h
	$(CC) -c $< -o $@ $(CFLAGS)

%.o: $(COMMON1_3)/%.c $(COMMON1_3)/%.h
		$(CC) -c $< -o $@ $(CFLAGS)

#opens server on public port 2000
run: serveur2-SemiCroustillants
	$(BIN)/serveur2-SemiCroustillants 2000

clean:
	rm *.o
