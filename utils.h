#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

void vider_buffer_stdin(void);
bool lire_chaine_securisee(char *buffer, int taille_max);
bool lire_long_securise(long *valeur);
bool lire_double_securise(double *valeur);

#endif