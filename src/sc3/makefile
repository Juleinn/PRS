CFLAGS=-Wall $(INCLUDE) -lpthread -Wno-unused-variable -O3
CC=gcc
INCLUDE=-I./ -I../common -I../common1_3
BIN=../../bin
COMMON=../common
COMMON1_3=../common1_3

all: serveur3-SemiCroustillants

serveur3-SemiCroustillants: main.o connect.o files.o
	$(CC) $^ -o $(BIN)/serveur3-SemiCroustillants $(CFLAGS)

%.o: %.c %.h
	$(CC) -c $< -o $@ $(CFLAGS)

%.o: $(COMMON)/%.c $(COMMON)/%.h
	$(CC) -c $< -o $@ $(CFLAGS)

%.o: $(COMMON1_3)/%.c $(COMMON1_3)/%.h
		$(CC) -c $< -o $@ $(CFLAGS)

#opens server on public port 2000
run: serveur1-SemiCroustillants
	$(BIN)/serveur1-SemiCroustillants 2000

clean:
	rm *.o
