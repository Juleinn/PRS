#########################################################
# Makefile pour les programme du TP PRS
# Vincent Renotte, 2016, d'après le modèle de :
# Tanguy Risset, 2012
#########################################################
# Variables pour le compilateur et l'editeur de lien
BIN			  =../../bin
COMMON		  =../common
CC            =gcc 
LD			  =gcc
CFLAGS        +=-c -Wall

#cible principales: on construit l'executable TPlogo
all: serverTest 

#Création de l'exe
serverTest: serverTest.o 
	$(LD) serverTest.o -o $(BIN)/serverTest
	
#Création du .o
serverTest.o: serverTest.c
	$(CC) $(CFLAGS) serverTest.c -o serverTest.o		
	
#Pour nettoyer (enlever tous les fichiers générés)
clean:
	-rm -rf *~ *.out tri *.o *.res html
	
#opens server on public port 2000
run: serverTest
	$(BIN)/serveur1-Renotte_Claes 2000

	
