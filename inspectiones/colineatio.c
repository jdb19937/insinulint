/*
 * inspectiones/colineatio.c — colineatio signorum (e.g. '=' in adtributionibus)
 */

#include "../inspectio.h"
#include "colineatio.h"

#include <stdio.h>
#include <string.h>

#define COLINEATIO_TURBA_MAX 8

typedef struct {
    signum_genus_t genus;       /* genus signi allineandi */
    char           valor[8];    /* valor signi (e.g. "=", "//") */
    int            spatium_min; /* spatia minima ante signum */
    const char    *regula;
} colineatio_t;

typedef int (*colineatio_praedicatum_t)(const signum_t *, const versus_t *);

/*
 * est_adtributio — 1 si linea est adtributio simplex cum '=' et ';'.
 */
static int est_adtributio(const signum_t *signa, const versus_t *v)
{
    if (versus_vacuus(v))
        return 0;
    int primus = v->tok_primus;
    if (
        signa[primus].genus == SIGNUM_PRAEPROCESSOR ||
        signa[primus].genus == SIGNUM_COMMENTARIUM
    )
        return 0;
    static const char *prohibita[] = {
        "if", "else", "for", "while", "switch", "do",
        "return", "break", "continue", "goto", "case", "default",
        "struct", "union", "enum", "typedef", NULL
    };
    for (int k = 0; prohibita[k]; k++)
        if (est_verbum(&signa[primus], prohibita[k]))
            return 0;
    int aequale   = 0;
    int semicolon = 0;
    for (int j = v->tok_initium; j < v->tok_finis; j++) {
        if (
            signa[j].genus == SIGNUM_APERTIO ||
            signa[j].genus == SIGNUM_CLAUSIO
        )
            return 0;
        if (signa[j].genus == SIGNUM_SEMICOLON)
            semicolon = 1;
        if (
            !aequale &&
            signa[j].genus == SIGNUM_OPERATOR &&
            signa[j].longitudo == 1 &&
            signa[j].initium[0] == '='
        )
            aequale = 1;
    }
    return aequale && semicolon;
}

static int inveni_signum_col(
    const signum_t *signa, const versus_t *v, const colineatio_t *col
) {
    int lon = (int)strlen(col->valor);
    for (int j = v->tok_initium; j < v->tok_finis; j++) {
        if (signa[j].genus != col->genus)
            continue;
        if (
            lon > 0 && (
                signa[j].longitudo != lon ||
                strncmp(signa[j].initium, col->valor, (size_t)lon) != 0
            )
        )
            continue;
        return j;
    }
    return -1;
}

static int columna_naturalis(
    const signum_t *signa, const versus_t *v, int idx, int spatium_min
) {
    int prev = idx - 1;
    while (prev >= v->tok_initium && signa[prev].genus == SIGNUM_SPATIUM)
        prev--;
    if (prev < v->tok_initium)
        return -1;
    return signa[prev].columna + signa[prev].longitudo + spatium_min;
}

static void inspice_colineationes(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus,
    colineatio_praedicatum_t praedicatum,
    const colineatio_t *col
) {
    const signum_t *signa = lex->signa;

    int vi = 0;
    while (vi < num_versus) {
        while (vi < num_versus && !praedicatum(signa, &versus[vi]))
            vi++;
        if (vi >= num_versus)
            break;

        int gi         = vi;
        int ind_gregis = versus[vi].spatia;
        while (
            vi < num_versus &&
            praedicatum(signa, &versus[vi]) &&
            versus[vi].spatia == ind_gregis
        )
            vi++;
        int gf = vi;

        if (gf - gi < 2)
            continue;

        int nat_max = 0;
        int nat_min = -1;
        for (int k = gi; k < gf; k++) {
            int idx = inveni_signum_col(signa, &versus[k], col);
            if (idx < 0)
                continue;
            int nc = columna_naturalis(
                signa, &versus[k], idx, col->spatium_min
            );
            if (nc < 0)
                continue;
            if (nat_min < 0 || nc < nat_min)
                nat_min = nc;
            if (nc > nat_max)
                nat_max = nc;
        }

        if (nat_min < 0)
            continue;

        if (nat_max - nat_min > COLINEATIO_TURBA_MAX)
            continue;

        for (int k = gi; k < gf; k++) {
            const versus_t *v = &versus[k];
            int idx = inveni_signum_col(signa, v, col);
            if (idx < 0)
                continue;
            int c = signa[idx].columna;
            if (c == nat_max)
                continue;
            char nuntius[NUNTIUS_MAX];
            snprintf(
                nuntius, sizeof(nuntius),
                "'%s' in columna %d, expectatur %d",
                col->valor, c, nat_max
            );
            adde_fix(
                ins, GRAVITAS_MONITUM,
                v->numero, c,
                col->regula, nuntius,
                nat_max
            );
        }
    }
}

void inspice_adtributiones_colineatas(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus
) {
    static const colineatio_t col = {
        SIGNUM_OPERATOR, "=", 1, "adtributio_colineata"
    };
    inspice_colineationes(ins, lex, versus, num_versus, est_adtributio, &col);
}
