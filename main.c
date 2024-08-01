#include "circuit.h"
#include <inttypes.h>
#include <stdio.h>
int main(void) {
  circuit *circuit = init_circuit(5);
  add_comp(circuit, "resistor1", RESISTOR, 2000);
  add_comp(circuit, "capacitor1", CAPACITOR, 10);
  add_comp(circuit, "indcuctor1", INDUCTOR, 5);

  for (size_t i = 0; i < circuit->size; ++i) {
    printf("Name: %s, Type: %d, Value: %" PRIu32 "\n", circuit->array[i].name,
           circuit->array[i].type, circuit->array[i].value);
  }
  return 0;
}
