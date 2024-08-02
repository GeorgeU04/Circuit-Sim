#include "circuit.h"
#include <stdint.h>

const uint32_t voltage = 5;

int main(void) {
  circuit *circuit = init_circuit(5, voltage);
  add_comp(circuit, "resistor1", RESISTOR, 220);
  add_comp(circuit, "resistor2", RESISTOR, 200);
  add_comp(circuit, "resistor3", RESISTOR, 300);
  // add_comp(circuit, "capacitor1", CAPACITOR, 10);
  // add_comp(circuit, "indcuctor1", INDUCTOR, 5);
  print_comps(circuit);
  calc_circuit_info(circuit);
  print_circuit_info(circuit);
  return 0;
}
