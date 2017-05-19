all:
	-make -C src/
clean:
	-make -C src/ clean
	-rm bin/serveur1-SemiCroustillants
	-rm bin/serveur2-SemiCroustillants
	-rm bin/serveur3-SemiCroustillants
	-rm bin/copy_*
