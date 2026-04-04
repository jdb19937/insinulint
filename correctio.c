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
    int *bra_col   = malloc((size_t)(nlin + 1) * sizeof(int));
    int *bra_ind   = malloc((size_t)(nlin + 1) * sizeof(int));
    int *una_ind   = malloc((size_t)(nlin + 1) * sizeof(int));
    int *virg_fix  = calloc((size_t)(nlin + 1), sizeof(int));
    int *cub_fix   = calloc((size_t)(nlin + 1), sizeof(int));
    int *op_fix    = calloc((size_t)(nlin + 1), sizeof(int));
    int *verb_fix  = calloc((size_t)(nlin + 1), sizeof(int));
    int *semi_fix  = calloc((size_t)(nlin + 1), sizeof(int));
    int *long_fix  = calloc((size_t)(nlin + 1), sizeof(int));
    int *long_ind  = calloc((size_t)(nlin + 1), sizeof(int));
    if (
        !ind_exp || !trim_fin || !split_col || !split_ind ||
        !apert_col || !apert_ind || !equ_col || !equ_spa ||
        !corp_col || !corp_ind || !bra_col || !bra_ind || !una_ind ||
        !virg_fix || !cub_fix || !op_fix || !verb_fix || !semi_fix ||
        !long_fix || !long_ind
    ) {
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
        free(bra_col);
        free(bra_ind);
        free(una_ind);
        free(virg_fix);
        free(cub_fix);
        free(op_fix);
        free(verb_fix);
        free(semi_fix);
        free(long_fix);
        free(long_ind);
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
        bra_col[i]   = -1;
        bra_ind[i]   = 0;
        una_ind[i]   = -1;
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
        } else if (strcmp(m->regula, "tabulae_mixtae") == 0) {
            if (m->fix_valor >= 0 && ind_exp[li] < 0)
                ind_exp[li] = m->fix_valor;
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
        } else if (strcmp(m->regula, "bracchia_stilus") == 0) {
            if (m->bra_columna >= 0 && bra_col[li] < 0) {
                bra_col[li] = m->bra_columna;
                bra_ind[li] = m->fix_valor;
            }
        } else if (strcmp(m->regula, "una_sententia") == 0) {
            if (m->fix_valor >= 0 && una_ind[li] < 0)
                una_ind[li] = m->fix_valor;
        } else if (strcmp(m->regula, "spatium_virgula") == 0) {
            virg_fix[li] = 1;
        } else if (strcmp(m->regula, "cubitus") == 0) {
            cub_fix[li] = 1;
        } else if (strcmp(m->regula, "spatium_operator") == 0) {
            op_fix[li] = 1;
        } else if (strcmp(m->regula, "spatium_verbum") == 0) {
            verb_fix[li] = 1;
        } else if (strcmp(m->regula, "spatium_semicolon") == 0) {
            semi_fix[li] = 1;
        } else if (strcmp(m->regula, "longitudo_lineae") == 0) {
            if (!long_fix[li]) {
                long_fix[li] = 1;
                long_ind[li] = (m->fix_valor >= 0) ? m->fix_valor : 0;
            }
        }
    }

    /* alloca buffer exitus */
    size_t outsz = fons_lon * 2 + (size_t)nlin * 64 + 4;
    char *out    = malloc(outsz);
    if (!out) {
        free(fons);
        free(lineae);
        free(ind_exp);
        free(trim_fin);
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

        /* si linea est solum ';' vel ',' cum correctione, iunge ad
         * lineam praecedentem */
        {
            const char *cont = l->initium + sp_init;
            int clon         = l->lon - sp_init;
            if (
                clon == 1 && (cont[0] == ';' || cont[0] == ',') &&
                (semi_fix[i] || virg_fix[i]) && wp > out
            ) {
                /* retrahe ante '\n' praecedentem */
                if (*(wp - 1) == '\n')
                    wp--;
                *wp++ = cont[0];
                *wp++ = '\n';
                continue;
            }
        }

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

        /* cubitus: iunge '}' cum 'else' in linea sequente */
        if (
            spec->bra_else_coniunctum &&
            i + 1 < nlin &&
            cub_fix[i + 1]
        ) {
            int transili = 0;
            wp = corrige_cubitum(
                wp, corpus, corp_lon,
                lineae, i, nlin, &transili
            );
            if (transili)
                i++;
            continue;
        }

        /* bracchia stilus K&R: iunge '{' ex linea proxima */
        if (
            spec->bra_stilus == 0 &&
            i + 1 < nlin &&
            bra_col[i + 1] >= 0
        ) {
            int transili = 0;
            wp = corrige_bracchia_kr(
                wp, corpus, corp_lon,
                lineae, i, nlin, &transili,
                (i + 1 < nlin) ? una_ind[i + 1] : -1, spec
            );
            if (transili)
                i++;
            continue;
        }

        /* bracchia stilus Allman: scinde '{' ad lineam novam */
        if (spec->bra_stilus == 1 && bra_col[i] >= 0) {
            int cb = columna_ad_byte(
                l->initium, l->lon, bra_col[i]
            ) - sp_init;
            wp = corrige_bracchia_allman(
                wp, corpus, corp_lon, cb, bra_ind[i], spec
            );
            continue;
        }

        /* una sententia */
        if (una_ind[i] >= 0) {
            wp = corrige_unam_sententiam(
                wp, corpus, corp_lon, una_ind[i], spec
            );
            continue;
        }

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

        /* scissio linearum longarum */
        if (long_fix[i]) {
            wp = corrige_longitudinem(
                wp, corpus, corp_lon,
                sp_init, long_ind[i],
                spec->lin_longitudo_max, spec
            );
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

        /* correctiones inline catenatae: virgulae, operatores,
         * verba clavis, semicolona */
        {
            int inl = virg_fix[i] | op_fix[i] |
                verb_fix[i] | semi_fix[i];
            if (inl) {
                /* alveus temporarius pro catenatione */
                char tmp1[8192], tmp2[8192];
                const char *src = corpus;
                int slon        = corp_lon;
                char *dst;
                int dlon;

                if (virg_fix[i]) {
                    dst  = corrige_virgulas(tmp1, src, slon);
                    dlon = (int)(dst - tmp1) - 1; /* sine '\n' */
                    src  = tmp1;
                    slon = dlon;
                }
                if (op_fix[i]) {
                    dst  = corrige_operatores(tmp2, src, slon);
                    dlon = (int)(dst - tmp2) - 1;
                    src  = tmp2;
                    slon = dlon;
                }
                if (verb_fix[i]) {
                    char *d = (src == tmp2) ? tmp1 : tmp2;
                    dst     = corrige_spatium_verbum(d, src, slon);
                    dlon    = (int)(dst - d) - 1;
                    src     = d;
                    slon    = dlon;
                }
                if (semi_fix[i]) {
                    char *d = (src == tmp2) ? tmp1 : tmp2;
                    dst     = corrige_spatium_semicolon(d, src, slon);
                    dlon    = (int)(dst - d) - 1;
                    src     = d;
                    slon    = dlon;
                }
                memcpy(wp, src, slon);
                wp += slon;
                *wp++ = '\n';
                continue;
            }
        }

        memcpy(wp, corpus, corp_lon);
        wp += corp_lon;
        *wp++ = '\n';
    }

    /* finis: adde lineam novam si deest */
    if (spec->lin_finis_nova && wp > out && *(wp - 1) != '\n')
        *wp++ = '\n';

    size_t outlon = (size_t)(wp - out);

    /* si nihil mutatum est, non scribe */
    int mutatum = (outlon != fons_lon || memcmp(fons, out, outlon) != 0);

    int res = 0;
    if (mutatum) {
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
    free(bra_col);
    free(bra_ind);
    free(una_ind);
    free(virg_fix);
    free(cub_fix);
    free(op_fix);
    free(verb_fix);
    free(semi_fix);
    free(long_fix);
    free(long_ind);
    free(out);
    return (res < 0) ? -1 : mutatum;
}
