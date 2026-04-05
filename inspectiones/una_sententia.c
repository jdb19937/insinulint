/*
 * inspectiones/una_sententia.c — una sententia per lineam
 *
 * Flagellat lineas cum pluribus sententiis (plura ';' extra parentheses).
 */

#include "../inspectio.h"
#include "una_sententia.h"

#include <stdio.h>

void inspice_unam_sententiam(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus
) {
    const signum_t *signa = lex->signa;

    for (int vi = 0; vi < num_versus; vi++) {
        const versus_t *v = &versus[vi];
        if (versus_vacuus(v))
            continue;

        /* transili praeprocessorem */
        if (signa[v->tok_primus].genus == SIGNUM_PRAEPROCESSOR)
            continue;

        /* case/default: permitte duas sententias (stmt; break;) */
        int limen = 1;
        if (
            est_verbum(&signa[v->tok_primus], "case") ||
            est_verbum(&signa[v->tok_primus], "default")
        )
            limen = 2;

        /* numera semicolona extra parentheses */
        int prof_par     = 0;
        int semicolona   = 0;
        int primum_extra = -1;  /* index primi ';' extra — ad nuntium */

        for (int j = v->tok_initium; j < v->tok_finis; j++) {
            if (
                signa[j].genus == SIGNUM_APERTIO_PAR ||
                signa[j].genus == SIGNUM_APERTIO_QUAD
            )
                prof_par++;
            else if (
                signa[j].genus == SIGNUM_CLAUSIO_PAR ||
                signa[j].genus == SIGNUM_CLAUSIO_QUAD
            ) {
                if (prof_par > 0)
                    prof_par--;
            } else if (
                signa[j].genus == SIGNUM_SEMICOLON && prof_par == 0
            ) {
                semicolona++;
                if (semicolona == limen + 1 && primum_extra < 0)
                    primum_extra = j;
            }
        }

        if (semicolona > limen && primum_extra >= 0) {
            char nuntius[NUNTIUS_MAX];
            snprintf(
                nuntius, sizeof(nuntius),
                "%d sententiae in eadem linea (%s expectat%s)",
                semicolona,
                limen == 1 ? "una" : "duae",
                limen == 1 ? "a" : "ae"
            );
            adde_fix(
                ins, GRAVITAS_MONITUM,
                v->numero, signa[primum_extra].columna,
                "una_sententia", nuntius,
                v->spatia
            );
        }
    }
}
