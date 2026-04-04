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
#include "ison.h"

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

static int scinde_in_lineas(
    const char *fons, size_t fons_lon,
    linea_t *lineae, int max_lineae
) {
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
        int li = m->linea - 1;  /* 0-basis */
        if (li < 0 || li >= nlin) continue;

        if (strcmp(m->regula, "indentatio") == 0) {
            if (m->apert_columna >= 0) {
                /* scissio: contentum post '(' ad lineam novam */
                if (apert_col[li] < 0) {
                    apert_col[li] = m->apert_columna;
                    apert_ind[li] = (m->fix_valor >= 0) ? m->fix_valor : 0;
                }
            } else if (m->split_columna >= 0) {
                /* scissio: ')' ad lineam novam */
                if (split_col[li] < 0) {
                    split_col[li] = m->split_columna;
                    split_ind[li] = (m->fix_valor >= 0) ? m->fix_valor : 0;
                }
            } else if (m->fix_valor >= 0 && ind_exp[li] < 0) {
                /* correctio indentationis normalis */
                ind_exp[li] = m->fix_valor;
            }
        } else if (strcmp(m->regula, "spatia_terminalia") == 0) {
            trim_fin[li] = 1;
        } else if (strcmp(m->regula, "adtributio_colineata") == 0) {
            if (m->fix_valor >= 0 && equ_col[li] < 0) {
                equ_col[li] = m->columna;
                equ_spa[li] = m->fix_valor - m->columna; /* delta */
            }
        } else if (strcmp(m->regula, "corpus_separatum") == 0) {
            if (m->fix_valor >= 0 && corp_col[li] < 0) {
                corp_col[li] = m->columna;
                corp_ind[li] = m->fix_valor;
            }
        }
    }

    /* alloca buffer exitus (extra spatium pro scissionibus) */
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
        const char *corpus   = l->initium + sp_init;
        int         corp_lon = l->lon - sp_init;

        /* scissio: corpus separatum (e.g. "if (x) y;" → duas lineas) */
        if (corp_col[i] >= 0) {
            int cc = corp_col[i] - sp_init;
            if (cc < 0) cc = 0;
            if (cc > corp_lon) cc = corp_lon;

            /* pars 1: controlum usque ad corpus */
            int ante_lon = cc;
            /* remove spatia terminalia ante scissionem */
            while (
                ante_lon > 0 &&
                (
                    corpus[ante_lon - 1] == ' ' ||
                    corpus[ante_lon - 1] == '\t'
                )
            )
                ante_lon--;
            memcpy(wp, corpus, ante_lon);
            wp += ante_lon;
            *wp++ = '\n';

            /* pars 2: corpus in linea nova cum indentatione */
            int ci = corp_ind[i];
            for (int j = 0; j < ci; j++) *wp++ = ' ';

            const char *rest     = corpus + cc;
            int         rest_lon = corp_lon - cc;
            /* remove spatia terminalia */
            while (
                rest_lon > 0 &&
                (
                    rest[rest_lon - 1] == ' ' ||
                    rest[rest_lon - 1] == '\t'
                )
            )
                rest_lon--;
            memcpy(wp, rest, rest_lon);
            wp += rest_lon;
            *wp++ = '\n';
            continue;
        }

        /* scissio: contentum post '(' ad lineam novam (stilus patens) */
        if (apert_col[i] >= 0) {
            int par_byte = apert_col[i] - sp_init;  /* offset '(' in corpus */
            if (par_byte < 0) par_byte = 0;
            if (par_byte >= corp_lon) par_byte = corp_lon - 1;

            /* pars 1: contentum usque ad '(' inclusive */
            int ante_lon = par_byte + 1;
            memcpy(wp, corpus, ante_lon);
            wp += ante_lon;
            *wp++ = '\n';

            /* pars 2: contentum post '(' in linea nova cum indentatio nova */
            int ai = apert_ind[i];
            for (int j = 0; j < ai; j++) *wp++ = ' ';

            const char *rest     = corpus + ante_lon;
            int         rest_lon = corp_lon - ante_lon;
            /* salta spatia initialia post '(' */
            while (
                rest_lon > 0 &&
                (rest[0] == ' ' || rest[0] == '\t')
            ) {
                rest++;
                rest_lon--;
            }
            /* remove spatia terminalia */
            while (
                rest_lon > 0 &&
                (
                    rest[rest_lon - 1] == ' ' ||
                    rest[rest_lon - 1] == '\t'
                )
            )
                rest_lon--;

            memcpy(wp, rest, rest_lon);
            wp += rest_lon;
            *wp++ = '\n';
            continue;
        }

        /* scissio: ')' ad lineam novam (stilus patens) */
        if (split_col[i] >= 0) {
            /* byte offset ')' in linea (columna == byte pro spatiis) */
            int par_byte = split_col[i];
            int ante_lon = par_byte - sp_init;  /* bytes ante ')' */
            if (ante_lon < 0) ante_lon = 0;
            if (ante_lon > corp_lon) ante_lon = corp_lon;

            /* pars 1: contentum ante ')' */
            while (
                ante_lon > 0 &&
                (
                    corpus[ante_lon - 1] == ' ' ||
                    corpus[ante_lon - 1] == '\t'
                )
            )
                ante_lon--;
            memcpy(wp, corpus, ante_lon);
            wp += ante_lon;
            *wp++ = '\n';

            /* pars 2: ')' et residuum in linea nova */
            int si = split_ind[i];
            for (int j = 0; j < si; j++) *wp++ = ' ';

            const char *rest     = l->initium + par_byte;
            int         rest_lon = l->lon - par_byte;
            while (
                rest_lon > 0 &&
                (
                    rest[rest_lon - 1] == ' ' ||
                    rest[rest_lon - 1] == '\t'
                )
            )
                rest_lon--;

            /* si residuum est solum ')' et linea proxima est '{',
             * iunge in ') {' */
            int prox_apertio = 0;
            if (rest_lon == 1 && rest[0] == ')' && i + 1 < nlin) {
                const linea_t *ln = &lineae[i + 1];
                int k = 0;
                while (
                    k < ln->lon &&
                    (
                        ln->initium[k] == ' ' ||
                        ln->initium[k] == '\t'
                    )
                )
                    k++;
                if (k < ln->lon && ln->initium[k] == '{') {
                    int m2 = k + 1;
                    while (
                        m2 < ln->lon &&
                        (
                            ln->initium[m2] == ' ' ||
                            ln->initium[m2] == '\t'
                        )
                    )
                        m2++;
                    if (m2 == ln->lon)
                        prox_apertio = 1;
                }
            }

            memcpy(wp, rest, rest_lon);
            wp += rest_lon;
            if (prox_apertio) {
                *wp++ = ' ';
                *wp++ = '{';
                i++;  /* transili lineam '{' */
            }
            *wp++ = '\n';
            continue;
        }

        /* remove spatia terminalia si opus est */
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

        /* adtributio colineata: adiusta spatia ante '=' */
        if (equ_col[i] >= 0) {
            int eq_off = equ_col[i] - sp_init;
            int delta  = equ_spa[i];
            if (eq_off < 0)        eq_off = 0;
            if (eq_off > corp_lon) eq_off = corp_lon;
            if (delta >= 0) {
                /* insere delta spatia ante '=' */
                memcpy(wp, corpus, eq_off);
                wp += eq_off;
                for (int j = 0; j < delta; j++) *wp++ = ' ';
            } else {
                /* remove -delta spatia ante '=' */
                int new_off = eq_off + delta;
                if (new_off < 0) new_off = 0;
                memcpy(wp, corpus, new_off);
                wp += new_off;
            }
            memcpy(wp, corpus + eq_off, corp_lon - eq_off);
            wp += corp_lon - eq_off;
            *wp++ = '\n';
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

/* ================================================================
 * correctio_excommenta — exstirpat omnia commentaria ex plica
 * ================================================================ */

int correctio_excommenta(const char *via)
{
    char *fons = ison_lege_plicam(via);
    if (!fons) {
        fprintf(stderr, "correctio: non possum legere '%s'\n", via);
        return -1;
    }
    size_t fons_lon = strlen(fons);

    lexator_t lex;
    if (lexator_disseca(&lex, fons, fons_lon) < 0) {
        free(fons);
        return -1;
    }

    /* alloca buffer exitus */
    char *out = malloc(fons_lon + 2);
    if (!out) {
        lexator_purgare(&lex);
        free(fons);
        return -1;
    }

    /*
     * copia fontem, praetermittens commentaria.
     * post commentarium inline, spatia praecedentia removentur.
     * lineae quae solum commentarium continebant vacuae fiunt.
     */
    char *wp = out;
    const char *src = fons;

    for (int i = 0; i < lex.num_signa; i++) {
        const signum_t *s = &lex.signa[i];
        if (s->genus != SIGNUM_COMMENTARIUM)
            continue;

        const char *com_init = s->initium;
        const char *com_fin  = s->initium + s->longitudo;

        /* copia omnia ante commentarium */
        if (com_init > src) {
            memcpy(wp, src, (size_t)(com_init - src));
            wp += com_init - src;
        }

        /* remove spatia praecedentia usque ad '\n' vel initium */
        while (wp > out && (*(wp - 1) == ' ' || *(wp - 1) == '\t'))
            wp--;

        /* si commentarium in linea sola erat, remove '\n' post */
        int sola_linea = (wp == out || (wp > out && *(wp - 1) == '\n'));
        if (sola_linea && com_fin < fons + fons_lon && *com_fin == '\n')
            com_fin++;

        src = com_fin;
    }

    /* copia residuum */
    const char *fons_fin = fons + fons_lon;
    if (src < fons_fin) {
        memcpy(wp, src, (size_t)(fons_fin - src));
        wp += fons_fin - src;
    }

    size_t outlon = (size_t)(wp - out);

    /* scribe si mutatum est */
    int res = 0;
    if (outlon != fons_lon || memcmp(out, fons, outlon) != 0) {
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

    lexator_purgare(&lex);
    free(fons);
    free(out);
    return res;
}
