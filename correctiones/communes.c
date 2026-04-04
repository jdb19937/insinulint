/*
 * correctiones/communes.c — auxiliaria communia correctionum
 */

#include "../correctio.h"

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

int linea_vacua(const linea_t *l)
{
    for (int i = 0; i < l->lon; i++) {
        char c = l->initium[i];
        if (c != ' ' && c != '\t')
            return 0;
    }
    return 1;
}
