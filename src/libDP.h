/* En-t�tes de la libDP -- Algorithme de Davis-Putnam pour probl�mes SAT
   Copyright (C) 2002 Olivier Serve, Micka�l Sibelle & Philippe Strelezki

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

#ifndef LIBDP_H
#define LIBDP_H

#include "libSat.h"
#include "libIntr.h"
#include "libDP.h"
#include "libHist.h"

// Affiche les infos de version de la librairie
void dp_vers();

// R�sout un graphe SAT
tIntr *dp_sat(tGraphe **pGraph);

// Algorithme de Davis-Putnam
tIntr *dp_davis_putnam(tGraphe **pGraph, tIntr *pIntr); 

// Renvoie l'interpr�tation 
tIntr *dp_test_sat(tGraphe **pGraph, int pLitt, tIntr *pLsLitt);

// R�duit la clause du graphe en f� du litt�ral
int dp_reduire_cls(int pLitt, tClause *pCls, tGraphe *pGraph, tHist *pHist);

// R�duit le graphe en fonction du litt�ral
void dp_reduire(int pLitt, tGraphe **pGraph, tHist *pHist);

#endif
