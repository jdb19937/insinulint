/*
 * correctiones/communes.c — auxiliaria communia correctionum
 */

#include "../correctio.h"
#include "communes.h"

int scinde_in_lineas(
    const char *fons, size_t fons_lon,
    linea_t *lineae, int max_lineae
) {
    int n = 0;
    const char *p   = fons;
    const char *fin = fons + fons_lon;

    while (p < fin && n < max_lineae) {
        const char *q = p;
        while (q < fin && *q != '\n')
            q++;
        lineae[n].initium = p;
        lineae[n].lon     = (int)(q - p);
        n++;
        p = (q < fin) ? q + 1 : q;
    }
    return n;
}

int columna_ad_byte(const char *initium, int lon, int columna)
{
    int col = 0;
    for (int i = 0; i < lon; i++) {
        if (col >= columna)
            return i;
        if (initium[i] == '\t')
            col += 8 - (col % 8);
        else
            col++;
    }
    return lon;
}

char *scribe_indentationem(char *wp, int spatia, const speculum_t *spec)
{
    if (spec->ind_tabulis) {
        int tabs = spatia / 8;
        int rest = spatia % 8;
        for (int j = 0; j < tabs; j++)
            *wp++ = '\t';
        for (int j = 0; j < rest; j++)
            *wp++ = ' ';
    } else {
        for (int j = 0; j < spatia; j++)
            *wp++ = ' ';
    }
    return wp;
}

int linea_vacua(const linea_t *l)
{
    for (int i = 0; i < l->lon; i++) {
        char c = l->initium[i];
        if (c != ' ' && c != '\t')
            return 0;
    }
    return 1;
}
