#include "circuit.h"
#include "extras.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *comp_names[] = {"Resistor", "Capacitor", "Inductor"};

unit_t units[] = {{"o", "Ohms", 1.0},           {"ko", "Kiloohms", 1.0e3},
                  {"mo", "Megaohms", 1.0e6},    {"f", "Farads", 1.0e12},
                  {"uf", "Microfarads", 1.0e3}, {"nf", "Nanofarads", 1.0},
                  {"pf", "Picofarads", 1.0e-3}, {"h", "Henries", 1.0e3},
                  {"mh", "Millihenries", 1.0},  {"uh", "Microhenries", 1.0e-3}};

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

  switch (type) {
  case RESISTOR:
    comp->comp.resistor.voltage_drop = 0;
    comp->comp.resistor.power_dissipation = 0;
    break;
  case CAPACITOR:
    comp->comp.capacitor.charge = 0;
    comp->comp.capacitor.voltage_across = 0;
    break;
  case INDUCTOR:
    comp->comp.inductor.voltage_across = 0;
    comp->comp.inductor.energy_stored = 0;
    break;
  }
  ++circuit->size;
  return circuit;
}

circuit *init_circuit(uint32_t capacity, const double voltage,
                      const uint8_t voltage_type) {
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
  circuit->voltage_type = voltage_type;
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
  // DC will be a 1
  if (circuit->voltage_type) {
    double reciprocal_sum = 0;
    for (size_t i = 0; i < circuit->size; ++i) {
      switch (circuit->array[i].type) {
      case RESISTOR:
        circuit->total_resistance +=
            convert_units(circuit->array[i].value, circuit->array[i].unit);
        break;
      case CAPACITOR:
        reciprocal_sum += 1.0 / convert_units(circuit->array[i].value,
                                              circuit->array[i].unit);
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
    circuit->current = circuit->voltage / circuit->total_resistance;
    circuit->power =
        (circuit->voltage * circuit->voltage) / circuit->total_resistance;
    double resistance = 0;
    double capacitance = 0;
    double inductance = 0;
    for (size_t i = 0; i < circuit->size; ++i) {
      switch (circuit->array[i].type) {
      case RESISTOR:
        resistance =
            convert_units(circuit->array[i].value, circuit->array[i].unit);
        circuit->array[i].comp.resistor.voltage_drop =
            circuit->current * resistance;
        circuit->array[i].comp.resistor.power_dissipation =
            (circuit->voltage * circuit->voltage) / resistance;
        break;
      case CAPACITOR:
        capacitance =
            convert_units(circuit->array[i].value, circuit->array[i].unit);
        circuit->array[i].comp.capacitor.charge =
            capacitance * circuit->voltage;
        circuit->array[i].comp.capacitor.voltage_across =
            circuit->array[i].comp.capacitor.charge / capacitance;
        circuit->array[i].comp.capacitor.energy_stored =
            (.5 * capacitance * (circuit->voltage * circuit->voltage) *
             .000001);
        break;
      case INDUCTOR:
        inductance =
            convert_units(circuit->array[i].value, circuit->array[i].unit) *
            1.0e-3;
        circuit->array[i].comp.inductor.energy_stored =
            (0.5 * inductance * (circuit->current * circuit->current)) * 1000.0;
        circuit->array[i].comp.inductor.voltage_across = 0;
        break;
      }
    }
  }
  // AC will be a 0
  else {
  }
}

void print_comps(circuit *circuit) {
  for (size_t i = 0; i < circuit->size; ++i) {
    switch (circuit->array[i].type) {
    case RESISTOR:
      printf(
          "Name: %s, Type: %s, Value: %.2f %s, Voltage Drop: %.2f Volts, Power "
          "Dissipation: %.2f Watts\n",
          circuit->array[i].name, comp_names[circuit->array[i].type],
          circuit->array[i].value, get_full_unit_name(circuit->array[i].unit),
          circuit->array[i].comp.resistor.voltage_drop,
          circuit->array[i].comp.resistor.power_dissipation);
      break;
    case CAPACITOR:
      printf(
          "Name: %s, Type: %s, Value: %.2f %s, Energy Stored: %.6f Milijoules, "
          "Charge: %.2f Nanocoulombs, "
          "Voltage Across: %.2f Volts\n",
          circuit->array[i].name, comp_names[circuit->array[i].type],
          circuit->array[i].value, get_full_unit_name(circuit->array[i].unit),
          circuit->array[i].comp.capacitor.energy_stored,
          circuit->array[i].comp.capacitor.charge,
          circuit->array[i].comp.capacitor.voltage_across);
      break;
    case INDUCTOR:
      printf(
          "Name: %s, Type: %s, Value: %.2f %s, Energy Stored: %.6f Milijoules, "
          "Voltage Across: %.2f Volts\n",
          circuit->array[i].name, comp_names[circuit->array[i].type],
          circuit->array[i].value, get_full_unit_name(circuit->array[i].unit),
          circuit->array[i].comp.inductor.energy_stored,
          circuit->array[i].comp.inductor.voltage_across);
      break;
    }
  }
}

void print_circuit_info(circuit *circuit) {
  printf(
      "Total Resistance: %.2f Ohms\nTotal Capacitance: %.4f Nanofarads\nTotal "
      "Inductance: %.2f Milihenrys\nCurrent: %.4f Amps\nPower: %.4f Watts\n",
      circuit->total_resistance, circuit->total_capacitance,
      circuit->total_inductance, circuit->current, circuit->power);
}
