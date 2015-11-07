/* Programme de résolution de problèmes SAT par Davis-Putnam
   Copyright (C) 2002 Olivier Serve, Mickaël Sibelle & Philippe Strelezki

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sat.h"
#include "davis_putnam.h"

#define TESTSAT_NAME "TestSAT"
#define TESTSAT_VERS "2.1"
#define TESTSAT_AUTH "Olivier Serve & Mickael Sibelle & Philippe Strelezki"

int main(int argC, char *argV[]) {
  tGraphe *graph;
  char *nom_fic;

  // Afficher les versions et auteurs des librairies utilisées
  printf("\n\n           ________________________________________________________________\n");
  printf("    ^ ^   |                                                                |\n");
  printf("   (o o) <  Bienvenue dans %s v%s !                                  |\n", TESTSAT_NAME, TESTSAT_VERS);
  printf("  ()\\#/() | Ecrit par %s |\n", TESTSAT_AUTH);
  printf("  (\")'(\") |________________________________________________________________|\n");

  nom_fic = malloc(32);
  if (argC == 1) strcpy(nom_fic, "cls.dat");
  else strcpy(nom_fic, argV[1]);

  printf("\n\n> Chargement du fichier :\n");
  graph = sat_load_file(nom_fic);

  printf("\n\n> Affichage :\n");
  sat_see(graph);

  printf("\n\n> Résolution:\n");
  intr_see( dp_sat(&graph) );

  printf("\n\n> Libération de la mémoire:\n");
  sat_free(&graph);

  printf("\n\n> Terminé\n\n");

  fprintf(stderr, "              ________________________________________________________________\n");
  fprintf(stderr, "             /                                                                \\\n");
  fprintf(stderr, "     o o    |  Pour ne plus voir tous ces messages, il suffit de taper         |\n");
  fprintf(stderr, "    (. .)  <  ' 2> /dev/null' à la fin de la ligne de commande. Mais vous pou- |\n");
  fprintf(stderr, "   () \" ()  | vez aussi rediriger la sortie vers un fichier de log...          |\n");
  fprintf(stderr, "   (\")'(\")   \\________________________________________________________________/\n\n"); 

  return 0;
}
