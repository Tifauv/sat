#!/bin/bash


cd $(dirname $0)

if [ $# -lt 1 ]
then
	echo "Usage: sudoku.sh <grid_data_file>"
	echo ""
	echo "  <grid_data_file>: Fichier texte contenant les données connues de la grille."
	echo "      Chaque ligne contient l'indication d'une valeur sous la forme <valeur><ligne><colonne>."
	echo "      Par exemple, '423' indique que la valeur 4 est présente à la ligne 2 colonne 3 de la grille."
	exit
fi

initial_grid=$1
if [[ ! -f "${initial_grid}" ]]
then
	echo "Le fichier '${initial_grid}' n'existe pas ou n'est pas lisible."
	exit
fi

# Génération du problème
>&2 echo "# Generating problem file..."
problem_file=$(mktemp)
./gen_sudoku_constraints.py > ${problem_file}
>&2 echo "  - general sudoku constraints generated."
sed -n -i -e "/^%$/r ${initial_grid}" -e 1x -e '2,${x;p}' -e '${x;p}' "${problem_file}"
>&2 echo "  - specific problem data added."

# Résolution et formatage du résultat
>&2 echo
>&2 echo "# Solving..."
solution_file=$(mktemp)
../build/src/solveSat "${problem_file}" > "${solution_file}"
time=$(awk '$1=="c" && $2=="Took" {print $3}' "${solution_file}")
>&2 echo "  - finished in ${time} ms."

result=$(awk '$1=="s" {print $2}' "${solution_file}")
case ${result} in
	'UNSATISFIABLE')
		>&2 echo "  - this grid has NO SOLUTION."
		;;
	'SATISFIABLE')
		# Affichage du résultat
		grep -e '^v ' "${solution_file}" | sed -e 's/^v \(.*\) 0$/\1/' | sed -E 's/(^-| -)[0-9]*//g' | tr ' ' '\n' | sort -k 1.2,1.3 | sed -e 's/^\([0-9]\).*$/\1/' | xargs -L9 echo > "${initial_grid}.solution"
		>&2 echo
		>&2 echo "# Solution"
		cat "${initial_grid}.solution"
		;;
esac

# Nettoyage du fichier temporaire
rm "${problem_file}"
rm "${solution_file}"
