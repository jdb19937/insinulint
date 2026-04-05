/*
 * communes.h — typi et functiones communes correctionum
 */

#ifndef CORRECTIONES_COMMUNES_H
#define CORRECTIONES_COMMUNES_H

#include "../insinulint.h"


#include <stddef.h>

typedef struct {
    const char *initium;
    int         lon;
} linea_t;

int scinde_in_lineas(
    const char *fons, size_t fons_lon,
    linea_t *lineae, int max_lineae
);
int linea_vacua(const linea_t *l);

int columna_ad_byte(const char *initium, int lon, int columna);

char *scribe_indentationem(char *wp, int spatia, const speculum_t *spec);

#endif /* CORRECTIONES_COMMUNES_H */
