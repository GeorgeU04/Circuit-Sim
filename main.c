#include "circuit.h"
#include "extras.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

// eventually i will add a gui component so ill probabaly change it so it will
// no longer accept a file unless i have two versions one with a gui and one
// without idk yet
int32_t main(int32_t argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "[ERROR]: Must Provide File Path as an Argument\n");
    exit(EXIT_FAILURE);
  }

  FILE *file = fopen(argv[1], "r");
  if (!file) {
    fprintf(stderr, "[ERROR]: File Could not be Opened\n");
    exit(EXIT_FAILURE);
  }
  circuit *circuit = file_parse(file);
  if (!circuit) {
    fprintf(stderr, "[ERROR]: Circuit Initialization Error\n");
    exit(EXIT_FAILURE);
  }
  calc_circuit_info(circuit);
  print_comps(circuit);
  print_circuit_info(circuit);
  free_circuit(circuit);
  exit(EXIT_SUCCESS);
}
