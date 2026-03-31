/*
 * correctio.c — correctio plicarum C in loco
 *
 * Regulas fixabiles corrigit:
 *   - indentatio          (ex valore correctionis in monitis)
 *   - spatia_terminalia   (spatiis terminalibus deletis)
 *   - lineae_vacuae       (lineis extra contractis)
 *   - finis_lineae        (linea nova addita si deest)
 */

#include "correctio.h"
#include "insinulint.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================================================================
 * linea — portio fontis inter '\n'
 * ================================================================ */

typedef struct {
    const char *initium;
    int         lon;
} linea_t;

static int scinde_in_lineas(const char *fons, size_t fons_lon,
    linea_t *lineae, int max_lineae)
{
    int n = 0;
    const char *p   = fons;
    const char *fin = fons + fons_lon;

    while (p < fin && n < max_lineae) {
        const char *q = p;
        while (q < fin && *q != '\n') q++;
        lineae[n].initium = p;
        lineae[n].lon     = (int)(q - p);
        n++;
        p = (q < fin) ? q + 1 : q;
    }
    return n;
}

/* est linea vacua (sola spatia/tabulae vel nihil)? */
static int linea_vacua(const linea_t *l)
{
    for (int i = 0; i < l->lon; i++) {
        char c = l->initium[i];
        if (c != ' ' && c != '\t') return 0;
    }
    return 1;
}

/* ================================================================
 * correctio_age — correctio integra plicae
 * ================================================================ */

int correctio_age(const char *via, const speculum_t *spec)
{
    /* lege, lexa, inspice — per functionem communem */
    inspector_t ins;
    char *fons;
    if (insinulint_lege_inspice(via, spec, &ins, &fons) < 0)
        return -1;
    size_t fons_lon = strlen(fons);

    /* scinde in lineas */
    int nlin_max = 2;
    for (size_t i = 0; i < fons_lon; i++)
        if (fons[i] == '\n') nlin_max++;

    linea_t *lineae = malloc((size_t)nlin_max * sizeof(linea_t));
    if (!lineae) {
        free(fons);
        return -1;
    }
    int nlin = scinde_in_lineas(fons, fons_lon, lineae, nlin_max);

    /* construi tabulam correctionum (per lineam, 0-basis) */
    int *ind_exp  = malloc((size_t)(nlin + 1) * sizeof(int));
    int *trim_fin = calloc((size_t)(nlin + 1), sizeof(int));
    if (!ind_exp || !trim_fin) {
        free(fons); free(lineae); free(ind_exp); free(trim_fin);
        return -1;
    }
    for (int i = 0; i <= nlin; i++) ind_exp[i] = -1;

    /* popula tabulam ex monitis */
    for (int i = 0; i < ins.num_monita; i++) {
        const monitum_t *m = &ins.monita[i];
        int li = m->linea - 1;  /* 0-basis */
        if (li < 0 || li >= nlin) continue;

        if (strcmp(m->regula, "indentatio") == 0 && m->fix_valor >= 0) {
            /* si plura monita in eadem linea, primum vincit */
            if (ind_exp[li] < 0)
                ind_exp[li] = m->fix_valor;
        } else if (strcmp(m->regula, "spatia_terminalia") == 0) {
            trim_fin[li] = 1;
        }
    }

    /* alloca buffer exitus */
    size_t outsz = fons_lon * 2 + (size_t)nlin * 32 + 4;
    char *out = malloc(outsz);
    if (!out) {
        free(fons); free(lineae); free(ind_exp); free(trim_fin);
        return -1;
    }

    char *wp       = out;
    int vacuae_seq = 0;
    int vmax       = spec->lin_vacuae_max;

    for (int i = 0; i < nlin; i++) {
        const linea_t *l = &lineae[i];

        /* linea vacua: contrahe si opus est */
        if (linea_vacua(l)) {
            vacuae_seq++;
            if (vmax <= 0 || vacuae_seq <= vmax)
                *wp++ = '\n';
            continue;
        }
        vacuae_seq = 0;

        /* mensura spatia initialia originalia */
        int sp_init = 0;
        while (sp_init < l->lon &&
            (l->initium[sp_init] == ' ' || l->initium[sp_init] == '\t'))
            sp_init++;

        /* scribe indentationem (correctionis vel originalem) */
        int ind = ind_exp[i];
        if (ind >= 0) {
            if (spec->ind_tabulis) {
                int tabs = ind / spec->ind_latitudo;
                for (int j = 0; j < tabs; j++) *wp++ = '\t';
            } else {
                for (int j = 0; j < ind; j++) *wp++ = ' ';
            }
        } else {
            memcpy(wp, l->initium, sp_init);
            wp += sp_init;
        }

        /* contentum post indentationem */
        const char *corpus  = l->initium + sp_init;
        int         corp_lon = l->lon - sp_init;

        /* remove spatia terminalia si opus est */
        if (trim_fin[i]) {
            while (corp_lon > 0 &&
                (corpus[corp_lon - 1] == ' ' ||
                       corpus[corp_lon - 1] == '\t'))
                corp_lon--;
        }

        memcpy(wp, corpus, corp_lon);
        wp += corp_lon;
        *wp++ = '\n';
    }

    /* finis: adde lineam novam si deest */
    if (spec->lin_finis_nova && wp > out && *(wp - 1) != '\n')
        *wp++ = '\n';

    size_t outlon = (size_t)(wp - out);

    /* scribe in plicam */
    int res = 0;
    FILE *f = fopen(via, "wb");
    if (!f) {
        fprintf(stderr, "correctio: non possum scribere '%s'\n", via);
        res = -1;
    } else {
        if (fwrite(out, 1, outlon, f) != outlon) {
            fprintf(stderr, "correctio: erratum scribendi '%s'\n", via);
            res = -1;
        }
        fclose(f);
    }

    free(fons);
    free(lineae);
    free(ind_exp);
    free(trim_fin);
    free(out);
    return res;
}
