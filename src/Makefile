#
# Makefile pour le projet de Logique - Licence Info 2001/2002
# Université d'Angers
#
# Ecrit par Olivier SERVE
# 17/01/2002

CC = gcc
CFLAGS = -ggdb

testSat : testSat.o sat.o interpretation.o davis_putnam.o history.o
	$(CC) $(CFLAGS) -o testSat testSat.o sat.o interpretation.o davis_putnam.o history.o

testSat.o : testSat.c
	$(CC) -Wall -c $(CFLAGS) testSat.c

sat.o : sat.c
	$(CC) -Wall -c $(CFLAGS) sat.c
interpretation.o : interpretation.c
	$(CC) -Wall -c $(CFLAGS) interpretation.c
davis_putnam.o : davis_putnam.c
	$(CC) -Wall -c $(CFLAGS) davis_putnam.c
history.o : history.c
	$(CC) -Wall -c $(CFLAGS) history.c

clean :
	rm *.o testSat
