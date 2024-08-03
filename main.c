#include "circuit.h"
#include <stdint.h>

const uint32_t voltage = 5; // DC Voltage

int main(void) {
  circuit *circuit = init_circuit(5, voltage);
  add_comp(circuit, "resistor1", RESISTOR, 200, "O");
  add_comp(circuit, "resistor2", RESISTOR, 220, "o");
  add_comp(circuit, "resistor3", RESISTOR, 300, "o");
  add_comp(circuit, "capacitor1", CAPACITOR, 10, "nf");
  add_comp(circuit, "capacitor2", CAPACITOR, 22, "nf");
  add_comp(circuit, "capacitor2", CAPACITOR, 20, "Uf");
  add_comp(circuit, "indcuctor2", INDUCTOR, 10, "mH");
  add_comp(circuit, "indcuctor2", INDUCTOR, 500, "uH");
  // add_comp(circuit, "indcuctor3", INDUCTOR, 10, "un"); test for unit check
  calc_circuit_info(circuit);
  print_comps(circuit);
  print_circuit_info(circuit);
  free_circuit(circuit);
  return 0;
}
