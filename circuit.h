#ifndef COMPONENT_H
#define COMPONENT_H

#include <stdint.h>

typedef enum {
  RESISTOR,
  CAPACITOR,
  INDUCTOR,
} comp_type;

typedef struct component {
  char *name;
  comp_type type;
  uint32_t value;
} component;

typedef struct circuit {
  uint32_t size;
  uint32_t capacity;
  uint32_t voltage;
  uint32_t total_resistance;
  uint32_t total_capacitance;
  uint32_t total_inductance;
  float current;
  float power;
  component *array;
} circuit;

circuit *add_comp(circuit *circuit, const char *name, comp_type type,
                  uint32_t value);
circuit *init_circuit(uint32_t capacity, uint32_t voltage);
void free_circuit(circuit *circuit);
void calc_circuit_info(circuit *circuit);
void print_comps(circuit *circuit);
void print_circuit_info(circuit *circuit);
#endif
