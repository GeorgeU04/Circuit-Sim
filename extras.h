#ifndef EXTRAS_H
#define EXTRAS_H

#include "circuit.h"
#include <stdio.h>

/*
 *
 * File Format:
 * <VOLTAGE_VALUE>:<AC/DC>
 * <COMPONENT>:<NAME>:<VALUE>:<UNIT>
 *
 * EX:
 * 5:DC
 * resistor:resistor1:200:O
 *
 */

char *str_to_lower(const char *str);
circuit *file_parse(FILE *file);
#endif // !EXTRAS_H
