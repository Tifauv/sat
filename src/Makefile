#
# Makefile pour le projet de Logique - Licence Info 2001/2002
# Université d'Angers
#
# Ecrit par Olivier SERVE
# 17/01/2002

CC = gcc
CFLAGS = -ggdb

## GENERAL RULES
# Builds the default targets
all: testSat

# Cleans the build environment
clean :
	rm *.o testSat


## DEPENDENCIES RULES
testSat : testSat.o sat.o interpretation.o davis_putnam.o history.o
