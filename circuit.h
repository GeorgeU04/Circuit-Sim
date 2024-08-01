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
  component *array;
} circuit;

circuit *add_comp(circuit *circuit, const char *name, comp_type type,
                  uint32_t value);
circuit *init_circuit(uint32_t capacity);
void free_circuit(circuit *circuit);
void free_comp(component *comp);
uint32_t calc_resistance(circuit *circuit);
uint32_t calc_inductance(circuit *circuit);
double calc_capacitance(circuit *comp);
#endif
