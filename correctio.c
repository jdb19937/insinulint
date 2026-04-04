/*
 * correctio.c — correctio plicarum C in loco
 *
 * Orchestrator: legit monita, populat tabulas, vocat correctiones
 * singulas ex correctiones/.
 */

#include "correctio.h"
#include "insinulint.h"
#include "ison.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        if (fons[i] == '\n')
            nlin_max++;

    linea_t *lineae = malloc((size_t)nlin_max * sizeof(linea_t));
    if (!lineae) {
        free(fons);
        return -1;
    }
    int nlin = scinde_in_lineas(fons, fons_lon, lineae, nlin_max);

    /* construi tabulam correctionum (per lineam, 0-basis) */
    int *ind_exp   = malloc((size_t)(nlin + 1) * sizeof(int));
    int *trim_fin  = calloc((size_t)(nlin + 1), sizeof(int));
    int *split_col = malloc((size_t)(nlin + 1) * sizeof(int));
    int *split_ind = malloc((size_t)(nlin + 1) * sizeof(int));
    int *apert_col = malloc((size_t)(nlin + 1) * sizeof(int));
    int *apert_ind = malloc((size_t)(nlin + 1) * sizeof(int));
    int *equ_col   = malloc((size_t)(nlin + 1) * sizeof(int));
    int *equ_spa   = malloc((size_t)(nlin + 1) * sizeof(int));
    int *corp_col  = malloc((size_t)(nlin + 1) * sizeof(int));
    int *corp_ind  = malloc((size_t)(nlin + 1) * sizeof(int));
    if (
        !ind_exp || !trim_fin || !split_col || !split_ind ||
        !apert_col || !apert_ind || !equ_col || !equ_spa ||
        !corp_col || !corp_ind
    ) {
        free(fons); free(lineae); free(ind_exp); free(trim_fin);
        free(split_col); free(split_ind); free(apert_col); free(apert_ind);
        free(equ_col); free(equ_spa); free(corp_col); free(corp_ind);
        return -1;
    }
    for (int i = 0; i <= nlin; i++) {
        ind_exp[i]   = -1;
        split_col[i] = -1;
        split_ind[i] = 0;
        apert_col[i] = -1;
        apert_ind[i] = 0;
        equ_col[i]   = -1;
        equ_spa[i]   = 0;
        corp_col[i]  = -1;
        corp_ind[i]  = 0;
    }

    /* popula tabulam ex monitis */
    for (int i = 0; i < ins.num_monita; i++) {
        const monitum_t *m = &ins.monita[i];
        int li = m->linea - 1;
        if (li < 0 || li >= nlin)
            continue;

        if (strcmp(m->regula, "indentatio") == 0) {
            if (m->apert_columna >= 0) {
                if (apert_col[li] < 0) {
                    apert_col[li] = m->apert_columna;
                    apert_ind[li] = (m->fix_valor >= 0) ? m->fix_valor : 0;
                }
            } else if (m->split_columna >= 0) {
                if (split_col[li] < 0) {
                    split_col[li] = m->split_columna;
                    split_ind[li] = (m->fix_valor >= 0) ? m->fix_valor : 0;
                }
            } else if (m->fix_valor >= 0 && ind_exp[li] < 0) {
                ind_exp[li] = m->fix_valor;
            }
        } else if (strcmp(m->regula, "spatia_terminalia") == 0) {
            trim_fin[li] = 1;
        } else if (strcmp(m->regula, "adtributio_colineata") == 0) {
            if (m->fix_valor >= 0 && equ_col[li] < 0) {
                equ_col[li] = m->columna;
                equ_spa[li] = m->fix_valor - m->columna;
            }
        } else if (strcmp(m->regula, "corpus_separatum") == 0) {
            if (m->fix_valor >= 0 && corp_col[li] < 0) {
                corp_col[li] = m->columna;
                corp_ind[li] = m->fix_valor;
            }
        }
    }

    /* alloca buffer exitus */
    size_t outsz = fons_lon * 2 + (size_t)nlin * 64 + 4;
    char *out    = malloc(outsz);
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
        while (
            sp_init < l->lon &&
            (l->initium[sp_init] == ' ' || l->initium[sp_init] == '\t')
        )
            sp_init++;

        /* scribe indentationem (correctionis vel originalem) */
        int ind = ind_exp[i];
        if (ind >= 0) {
            wp = scribe_indentationem(wp, ind, spec);
        } else {
            memcpy(wp, l->initium, sp_init);
            wp += sp_init;
        }

        /* contentum post indentationem */
        const char *corpus   = l->initium + sp_init;
        int         corp_lon = l->lon - sp_init;

        /* corpus separatum */
        if (corp_col[i] >= 0) {
            int cb = columna_ad_byte(
                l->initium, l->lon, corp_col[i]
            ) - sp_init;
            wp = corrige_corpus(
                wp, corpus, corp_lon, cb, corp_ind[i], spec
            );
            continue;
        }

        /* scissio post '(' */
        if (apert_col[i] >= 0) {
            int ab = columna_ad_byte(
                l->initium, l->lon, apert_col[i]
            );
            wp = corrige_apertionem(
                wp, corpus, corp_lon, ab, apert_ind[i], sp_init, spec
            );
            continue;
        }

        /* scissio ')' ad lineam novam */
        if (split_col[i] >= 0) {
            int sb = columna_ad_byte(
                l->initium, l->lon, split_col[i]
            );
            int transili = 0;
            wp = corrige_scissionem(
                wp, corpus, corp_lon,
                sb, split_ind[i], sp_init,
                l, lineae, i, nlin, &transili, spec
            );
            if (transili)
                i++;
            continue;
        }

        /* remove spatia terminalia */
        if (trim_fin[i]) {
            while (
                corp_lon > 0 &&
                (
                    corpus[corp_lon - 1] == ' ' ||
                    corpus[corp_lon - 1] == '\t'
                )
            )
                corp_lon--;
        }

        /* colineatio adtributionum */
        if (equ_col[i] >= 0) {
            int eb = columna_ad_byte(
                l->initium, l->lon, equ_col[i]
            );
            wp = corrige_colineationem(
                wp, corpus, corp_lon,
                eb, equ_spa[i], sp_init
            );
            continue;
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
    free(split_col);
    free(split_ind);
    free(apert_col);
    free(apert_ind);
    free(equ_col);
    free(equ_spa);
    free(corp_col);
    free(corp_ind);
    free(out);
    return res;
}
