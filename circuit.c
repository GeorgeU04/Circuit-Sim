#include "circuit.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unit_t units[] = {{"o", "ohms", 1.0},           {"ko", "kiloohms", 1.0e3},
                  {"mo", "megaohms", 1.0e6},    {"f", "farads", 1.0e12},
                  {"uf", "microfarads", 1.0e6}, {"nf", "nanofarads", 1.0},
                  {"pf", "picofarads", 1.0e-3}, {"h", "henries", 1.0e6},
                  {"mh", "millihenries", 1.0},  {"uh", "microhenries", 1.0e-3}};

char *str_to_lower(const char *str) {
  size_t len = strlen(str);
  char *lower = malloc(len + 1);
  if (!lower) {
    fprintf(stderr, "[ERROR]: Memory Allocation Failure\n");
    exit(EXIT_FAILURE);
  }
  for (size_t i = 0; i < len; ++i) {
    lower[i] = (char)tolower((unsigned char)str[i]);
  }
  lower[len] = '\0';
  return lower;
}

double get_conversion_factor(const char *unit) {
  char *lower_unit = str_to_lower(unit);
  double factor = 1.0;
  for (size_t i = 0; i < sizeof(units) / sizeof(units[0]); ++i) {
    if (strcmp(lower_unit, units[i].name) == 0) {
      factor = units[i].factor;
      break;
    }
  }
  free(lower_unit);
  return factor;
}

const char *get_full_unit_name(const char *unit) {
  char *lower_unit = str_to_lower(unit);
  const char *full_name = "unknown";
  for (size_t i = 0; i < sizeof(units) / sizeof(units[0]); ++i) {
    if (strcmp(lower_unit, units[i].name) == 0) {
      full_name = units[i].full_name;
      break;
    }
  }
  free(lower_unit);
  return full_name;
}

bool check_units(const char *unit) {
  char *lower_unit = str_to_lower(unit);
  bool valid = false;
  for (size_t i = 0; i < sizeof(units) / sizeof(units[0]); ++i) {
    if (strcmp(lower_unit, units[i].name) == 0) {
      valid = true;
      break;
    }
  }
  free(lower_unit);
  return valid;
}

double convert_units(double value, const char *unit) {
  double factor = get_conversion_factor(unit);
  return value * factor;
}

circuit *add_comp(circuit *circuit, const char *name, comp_type type,
                  double value, const char *unit) {
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
  if (!check_units(unit)) {
    fprintf(stderr, "[ERROR]: Unknown Unit for %s\n", comp->name);
    free_circuit(circuit);
    exit(EXIT_FAILURE);
  }
  comp->unit = strdup(unit);
  if (!comp->unit) {
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
    free(circuit->array[i].unit);
  }
  free(circuit->array);
  free(circuit);
}

void calc_circuit_info(circuit *circuit) {
  double reciprocal_sum = 0;
  for (size_t i = 0; i < circuit->size; ++i) {
    switch (circuit->array[i].type) {
    case RESISTOR:
      circuit->total_resistance +=
          convert_units(circuit->array[i].value, circuit->array[i].unit);
      break;
    case CAPACITOR:
      reciprocal_sum +=
          1.0 / convert_units(circuit->array[i].value, circuit->array[i].unit);
      break;
    case INDUCTOR:
      circuit->total_inductance +=
          convert_units(circuit->array[i].value, circuit->array[i].unit);
      break;
    }
  }
  if (reciprocal_sum > 0) {
    circuit->total_capacitance = 1.0 / reciprocal_sum;
  } else {
    circuit->total_capacitance = 0;
  }

  if (circuit->total_resistance == 0) {
    circuit->total_resistance = 1;
  }
  circuit->current = (double)circuit->voltage / circuit->total_resistance;
  circuit->power = ((double)(circuit->voltage * circuit->voltage)) /
                   circuit->total_resistance;
}

void print_comps(circuit *circuit) {
  for (size_t i = 0; i < circuit->size; ++i) {
    printf("Name: %s, Type: %d, Value: %.2f %s\n", circuit->array[i].name,
           circuit->array[i].type, circuit->array[i].value,
           get_full_unit_name(circuit->array[i].unit)); // Use full name
  }
}

void print_circuit_info(circuit *circuit) {
  printf(
      "Total Resistance: %.2f Ohms\nTotal Capacitance: %.4f Nanofarads\nTotal "
      "Inductance: %.4f Milihenrys\nCurrent: %.4f Amps\nPower: %.4f Watts\n",
      circuit->total_resistance, circuit->total_capacitance,
      circuit->total_inductance, circuit->current, circuit->power);
}
