#include "circuit.h"
#include <inttypes.h>
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

circuit *init_circuit(uint32_t capacity, uint32_t voltage) {
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
  circuit->voltage = voltage;
  circuit->total_resistance = 0;
  circuit->total_inductance = 0;
  circuit->total_capacitance = 0;
  circuit->power = 0;
  circuit->current = 0;
  return circuit;
}

void free_circuit(circuit *circuit) {
  for (uint32_t i = 0; i < circuit->size; ++i) {
    free(circuit->array[i].name);
  }
  free(circuit->array);
  free(circuit);
}

void calc_circuit_info(circuit *circuit) {
  for (size_t i = 0; i < circuit->size; ++i) {
    switch (circuit->array[i].type) {
    case RESISTOR:
      circuit->total_resistance += circuit->array[i].value;
      break;
    case CAPACITOR:
      circuit->total_capacitance += circuit->array[i].value;
      break;
    case INDUCTOR:
      circuit->total_inductance += circuit->array[i].value;
      break;
    }
    if (circuit->total_resistance == 0)
      circuit->total_resistance = 1;
    circuit->current = (double)circuit->voltage / circuit->total_resistance;
    circuit->power = ((double)(circuit->voltage * circuit->voltage)) /
                     circuit->total_resistance;
  }
}

void print_comps(circuit *circuit) {
  for (size_t i = 0; i < circuit->size; ++i) {
    printf("Name: %s, Type: %d, Value: %" PRIu32 "\n", circuit->array[i].name,
           circuit->array[i].type, circuit->array[i].value);
  }
}

void print_circuit_info(circuit *circuit) {
  printf("Total Resitance: %" PRIu32 " Total Capacitance: %" PRIu32
         " Total Inductance: %" PRIu32 " Current: %f Power: %f\n",
         circuit->total_resistance, circuit->total_capacitance,
         circuit->total_inductance, circuit->current, circuit->power);
}
