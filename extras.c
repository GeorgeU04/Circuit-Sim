#include "circuit.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *units_abr[] = {
    "o", "ko", "mo", "f", "uf", "nf", "pf", "h", "mh", "uh",
};

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

// an evil file parser
circuit *file_parse(FILE *file) {
  double voltage = 0;
  uint8_t voltage_type = 0;
  char c;
  uint32_t word_size = 16;
  uint8_t delim_num = 0;
  char *comp_name = NULL;
  uint8_t comp_type = 0;
  uint8_t first_line = 1;
  double comp_value = 0;
  char *comp_unit = NULL;
  circuit *circuit = NULL;
  char *word = calloc(word_size, sizeof(char));

  if (!word) {
    fprintf(stderr, "[ERROR]: Memory Initialization Error\n");
    fclose(file);
    exit(EXIT_FAILURE);
  }

  while ((c = fgetc(file)) != EOF) {
    if (first_line) {
      if (c == ':') {
        word[strlen(word)] = '\0';
        // Voltage Value
        if (sscanf(word, "%lf", &voltage) != 1) {
          fprintf(stderr, "[ERROR]: Invalid Voltage Value \n");
          fclose(file);
          free(word);
          exit(EXIT_FAILURE);
        }
        word[0] = '\0';
      } else if (c == '\n') {
        word[strlen(word)] = '\0';
        // Voltage Type
        if (strcmp(str_to_lower(word), "dc") == 0) {
          voltage_type = 1;
        } else if (strcmp(str_to_lower(word), "ac") == 0) {
          voltage_type = 0;
        } else {
          fprintf(stderr,
                  "[ERROR]: Invalid Voltage Type. Must be 'AC' or 'DC'\n");
          fclose(file);
          free(word);
          exit(EXIT_FAILURE);
        }
        circuit = init_circuit(5, voltage, voltage_type);
        first_line = 0;
        delim_num = 0;
        word[0] = '\0';
      } else {
        size_t len = strlen(word);
        if (len + 1 >= word_size) {
          word_size *= 2;
          word = realloc(word, word_size * sizeof(char));
          if (!word) {
            fprintf(stderr, "[ERROR]: Memory Initialization Error\n");
            fclose(file);
            exit(EXIT_FAILURE);
          }
        }
        word[len] = c;
        word[len + 1] = '\0';
      }
    } else {
      if (c == ':') {
        word[strlen(word)] = '\0';
        switch (delim_num) {
        case 0:
          // Component Type
          if (strcmp(str_to_lower(word), "resistor") == 0) {
            comp_type = 0;
          } else if (strcmp(str_to_lower(word), "capacitor") == 0) {
            comp_type = 1;
          } else if (strcmp(str_to_lower(word), "inductor") == 0) {
            comp_type = 2;
          } else {
            fprintf(stderr, "[ERROR]: Invalid component type '%s'\n", word);
            free(word);
            fclose(file);
            exit(EXIT_FAILURE);
          }
          break;
        case 1:
          // Component Name
          free(comp_name);
          comp_name = strdup(word);
          if (!comp_name) {
            fprintf(stderr, "[ERROR]: Memory Allocation Error\n");
            free(word);
            fclose(file);
            exit(EXIT_FAILURE);
          }
          break;
        case 2:
          // Component Value
          if (sscanf(word, "%lf", &comp_value) != 1) {
            fprintf(stderr, "[ERROR]: Invalid component value '%s'\n", word);
            free(word);
            fclose(file);
            exit(EXIT_FAILURE);
          }
          break;
        default:
          fprintf(stderr, "[ERROR]: Unexpected error occurred while parsing\n");
          free(word);
          fclose(file);
          exit(EXIT_FAILURE);
        }

        delim_num++;
        word[0] = '\0';
      } else if (c == '\n') {
        word[strlen(word)] = '\0';
        // Component Unit
        free(comp_unit);
        comp_unit = strdup(word);
        if (!comp_unit) {
          fprintf(stderr, "[ERROR]: Memory Allocation Error\n");
          free(word);
          fclose(file);
          exit(EXIT_FAILURE);
        }

        if (circuit) {
          add_comp(circuit, comp_name, comp_type, comp_value, comp_unit);
        } else {
          fprintf(stderr, "[ERROR]: Circuit was not initialized properly.\n");
          free(word);
          free(comp_name);
          free(comp_unit);
          fclose(file);
          exit(EXIT_FAILURE);
        }

        delim_num = 0;
        word[0] = '\0';
      } else {
        size_t len = strlen(word);
        if (len + 1 >= word_size) {
          word_size *= 2;
          word = realloc(word, word_size * sizeof(char));
          if (!word) {
            fprintf(stderr, "[ERROR]: Memory Initialization Error\n");
            fclose(file);
            exit(EXIT_FAILURE);
          }
        }
        word[len] = c;
        word[len + 1] = '\0';
      }
    }
  }
  fclose(file);
  free(comp_name);
  free(comp_unit);
  free(word);
  return circuit;
}
