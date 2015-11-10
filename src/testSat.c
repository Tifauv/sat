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
#include "solver.h"

int main(int argC, char *argV[]) {
	char* nom_fic = malloc(32);
	if (argC == 1)
		strcpy(nom_fic, "cls.dat");
	else
		strcpy(nom_fic, argV[1]);

	printf("\n\n> Chargement du fichier :\n");
	tGraphe* formula = sat_load_file(nom_fic);
	free(nom_fic);
	sat_see(formula);

	printf("\n\n> Résolution:\n");
	tIntr* interpretation = alg_solve(&formula);
	intr_see(interpretation);

	printf("\n\n> Libération de la mémoire:\n");
	intr_free(&interpretation);
	sat_free(&formula);

	printf("\n\n> Terminé\n\n");

	return 0;
}
