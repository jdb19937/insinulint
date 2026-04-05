/*
 * commendatio.c — exitus structuratus monitorum ad stdout
 *
 * Pro quoque monito, caput cum via:linea:columna et tipo,
 * deinde alas linearum fontis cum numeris linearum scribit.
 * Fenestra semper 2*ala+1. Linea violationis signo '>' notatur.
 *
 * Forma:
 *   plica.c:42:5: monitum [spatium_verbum] spatium requiritur
 *    39:static int est_verbale(int c)
 *    40:{
 *    41:    int x = 0;
 *   >42:    if(c == '\n') break;
 *    43:    return x;
 *    44:}
 *    45:
 *   --
 */

#include "commendatio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINEAE_MAX (1 << 16)

/*
 * aedifica_lineas — inveni initia omnium linearum in fonte.
 * reddit numerum linearum.
 */
static int aedifica_lineas(
    const char *fons, const char **lineae, int max
) {
    int nl       = 0;
    lineae[nl++] = fons;
    for (const char *p = fons; *p && nl < max; p++) {
        if (*p == '\n')
            lineae[nl++] = p + 1;
    }
    return nl;
}

/* longitudo lineae sine '\n' terminali */
static int lon_lineae(const char *linea)
{
    const char *p = linea;
    while (*p && *p != '\n')
        p++;
    return (int)(p - linea);
}

/* quot characteres numerus decimalis occupat */
static int lat_numeri(int n)
{
    if (n < 10)
        return 1;
    if (n < 100)
        return 2;
    if (n < 1000)
        return 3;
    if (n < 10000)
        return 4;
    if (n < 100000)
        return 5;
    return 6;
}

int commendatio_scribe(
    const inspector_t *ins,
    const char *fons,
    const speculum_t *spec,
    int ala
) {
    if (ins->num_monita == 0 || ala < 0)
        return 0;

    /* aedifica indicem linearum */
    const char **lineae = malloc(
        (size_t)LINEAE_MAX * sizeof(const char *)
    );
    if (!lineae)
        return 0;
    int num_lin = aedifica_lineas(fons, lineae, LINEAE_MAX);

    const char *via = ins->via_fasciculi
        ? ins->via_fasciculi
        : "<fons>";

    for (int i = 0; i < ins->num_monita; i++) {
        const monitum_t *m = &ins->monita[i];
        int idx = m->linea - 1;

        /* limites fenestrae */
        int ab = idx - ala;
        int ad = idx + ala;
        if (ab < 0)
            ab = 0;
        if (ad >= num_lin)
            ad = num_lin - 1;
        if (idx < 0)
            idx = 0;
        if (idx >= num_lin)
            idx = num_lin - 1;

        int lat = lat_numeri(ad + 1);

        /* caput */
        printf(
            "%s:%d:%d: %s [%s] %s\n",
            via, m->linea, m->columna,
            m->gravitas == GRAVITAS_ERRATUM ? "erratum" : "monitum",
            m->regula, m->nuntius
        );

        /* metadata structurata */
        if (spec) {
            printf("  @fix_valor=%d", m->fix_valor);
            printf(" @split_columna=%d", m->split_columna);
            printf(" @apert_columna=%d", m->apert_columna);
            printf(" @bra_columna=%d", m->bra_columna);
            printf(" @ind_latitudo=%d", spec->ind_latitudo);
            printf(" @ind_tabulis=%d", spec->ind_tabulis);
            printf(" @bra_stilus=%d", spec->bra_stilus);
            printf(" @lin_longitudo_max=%d", spec->lin_longitudo_max);
            printf("\n");
        }

        /* lineae contextus */
        for (int j = ab; j <= ad; j++) {
            int lon = lon_lineae(lineae[j]);
            printf(
                "%c%*d:%.*s\n",
                (j == idx) ? '>' : ' ',
                lat, j + 1,
                lon, lineae[j]
            );
        }

        /* separator */
        if (i + 1 < ins->num_monita)
            printf("--\n");
    }

    free(lineae);
    return ins->num_monita;
}
