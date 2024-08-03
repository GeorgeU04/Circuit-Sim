#ifndef COMPONENT_H
#define COMPONENT_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  char *name;
  char *full_name;
  double factor;
} unit_t;

typedef enum {
  RESISTOR,
  CAPACITOR,
  INDUCTOR,
} comp_type;

typedef struct component {
  char *name;
  char *unit;
  comp_type type;
  double value;
} component;

typedef struct circuit {
  uint32_t size;
  uint32_t capacity;
  uint32_t voltage;
  double total_resistance;
  double total_capacitance;
  double total_inductance;
  double current;
  double power;
  component *array;
} circuit;

circuit *add_comp(circuit *circuit, const char *name, comp_type type,
                  double value, const char *unit);
circuit *init_circuit(uint32_t capacity, uint32_t voltage);
void free_circuit(circuit *circuit);
void calc_circuit_info(circuit *circuit);
void print_comps(circuit *circuit);
void print_circuit_info(circuit *circuit);

#endif // COMPONENT_H
