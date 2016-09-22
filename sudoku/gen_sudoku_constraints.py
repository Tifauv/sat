#!/usr/bin/python3

size = 9
squares = 3
square_size = size // 3

def gen_line_constraints(p_value, p_line, p_size):
    #print("c Value %d  |  Line %d" % (p_value, p_line))
    for start_col in range(1, p_size):
        for target_col in range(start_col+1, p_size+1):
            print("-%d%d%d -%d%d%d 0" % (p_value, p_line, start_col, p_value, p_line, target_col))


def gen_column_constraints(p_value, p_column, p_size):
    #print("c Value %d  |  Column %d" % (p_value, p_column))
    for start_line in range(1, p_size):
        for target_line in range(start_line+1, p_size+1):
            print("-%d%d%d -%d%d%d 0" % (p_value, start_line, p_column, p_value, target_line, p_column))


def gen_square_constraints(p_value, p_square_line, p_square_column, p_square_size):
    #print("c Value %d  |  Square %d,%d" % (p_value, p_square_line, p_square_column))
    
    square_line_offset   = p_square_size * (p_square_line   - 1)
    square_column_offset = p_square_size * (p_square_column - 1)
    
    
    for line_in_square in range(1, p_square_size+1):
        for column_in_square in range(1, p_square_size+1):
            for target_line_in_square in range(1, p_square_size+1):
                if line_in_square == target_line_in_square:
                    continue
                for target_column_in_square in range(1, p_square_size+1):
                    if column_in_square == target_column_in_square:
                        continue
                    
                    value_line  =        line_in_square + square_line_offset
                    target_line = target_line_in_square + square_line_offset
                    value_column  =        column_in_square + square_column_offset
                    target_column = target_column_in_square + square_column_offset
                    print("-%d%d%d -%d%d%d 0" % (p_value, value_line, value_column, p_value, target_line, target_column))


def gen_value_constraints(p_value, p_size, p_square_size):
    #print("c Value %d" % (p_value))
    for line in range(1, p_size+1):
        gen_line_constraints(p_value, line, p_size);
        gen_column_constraints(p_value, line, p_size);
    for square_line in range(1, p_square_size+1):
        for square_column in range(1, p_square_size+1):
            gen_square_constraints(p_value, square_line, square_column, p_square_size)


def gen_values_per_cell(p_size):
    #print("c Values per cell")
    for line in range(1, p_size+1):
        for column in range(1, p_size+1):
            for value in range(1, p_size+1):
                print("%d%d%d " % (value, line, column), end="")
            print("0")


def gen_unique_value_per_cell_constraint(p_size):
    #print("c Unique value per cell")
    for value in range(1, p_size+1):
        for target_value in range(value+1, p_size+1):
            for line in range(1, p_size+1):
                for column in range(1, p_size+1):
                    print("-%d%d%d -%d%d%d 0" % (value, line, column, target_value, line, column))


# Preamble
print("c Constraints for a Sudoku of size %dx%d" % (size, size))
nb_variables = size * size * size
# Line & column constraints
nb_clauses = size * size * size * (size-1)
# Square constraints
nb_clauses += size * size * size * (square_size-1) * (square_size-1)
# Allowed values per cell
nb_clauses += size * size
# Unique value per cell
nb_clauses += size * size * size * (size-1) / 2
print("p cnf %d %d" % (nb_variables, nb_clauses))

# General line / column / square constraints
for value in range(1, size+1):
    gen_value_constraints(value, size, square_size)

# Allowed values
gen_values_per_cell(size)

# Disallow two values for the same cell
gen_unique_value_per_cell_constraint(size)

# End of file
print("%")
print("0")
