#
# Makefile pour le projet de Logique - Licence Info 2001/2002
# Université d'Angers
#
# Ecrit par Olivier SERVE
# 17/01/2002

CC = gcc
CFLAGS = -ggdb

testSat : testSat.o libSat.o libIntr.o libDP.o libHist.o
	$(CC) $(CFLAGS) -o testSat testSat.o libSat.o libIntr.o libDP.o libHist.o

testSat.o : testSat.c
	$(CC) -Wall -c $(CFLAGS) testSat.c

libSat.o : libSat.c
	$(CC) -Wall -c $(CFLAGS) libSat.c
libIntr.o : libIntr.c
	$(CC) -Wall -c $(CFLAGS) libIntr.c
libDP.o : libDP.c
	$(CC) -Wall -c $(CFLAGS) libDP.c
libHist.o : libHist.c
	$(CC) -Wall -c $(CFLAGS) libHist.c

clean :
	rm *.o testSat
