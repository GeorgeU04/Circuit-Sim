#include "circuit.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
circuit *add_comp(circuit *circuit, const char *name, comp_type type,
                  uint32_t value) {
  if (circuit->size >= circuit->capacity) {
    circuit->capacity *= 2;
    circuit->array =
        realloc(circuit->array, circuit->capacity * sizeof(component));
    if (!circuit->array) {
      fprintf(stderr, "[ERROR]: Memory Reallocation Failure\n");
      exit(EXIT_FAILURE);
    }
  }

  component *comp = &circuit->array[circuit->size];
  comp->name = strdup(name);
  if (!comp->name) {
    fprintf(stderr, "[ERROR]: Memory Initialization Failure\n");
    free_circuit(circuit);
    exit(EXIT_FAILURE);
  }
  comp->type = type;
  comp->value = value;
  ++circuit->size;

  return circuit;
}

circuit *init_circuit(uint32_t capacity) {
  circuit *circuit = malloc(sizeof(*circuit));
  if (!circuit) {
    fprintf(stderr, "[ERROR]: Memory Initialization Failure");
    exit(EXIT_FAILURE);
  }
  circuit->array = malloc(capacity * sizeof(component));
  if (!circuit->array) {
    fprintf(stderr, "[ERROR]: Memory Initialization Failure\n");
    free(circuit);
    exit(EXIT_FAILURE);
  }
  circuit->size = 0;
  circuit->capacity = capacity;
  return circuit;
}

void free_circuit(circuit *circuit) {
  for (uint32_t i = 0; i < circuit->size; ++i) {
    free(circuit->array[i].name);
  }
  free(circuit->array);
  free(circuit);
}
uint32_t calc_resistance(circuit *circuit);
uint32_t calc_inductance(circuit *circuit);
double calc_capacitance(circuit *comp);
