all:
	-make -C src/
clean:
	-make -C src/ clean
	-rm bin/serveur1-Renotte_Claes
	-rm bin/copy_*
