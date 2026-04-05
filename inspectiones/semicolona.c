/*
 * inspectiones/semicolona.c — spatia ante semicolon
 */

#include "../inspectio.h"
#include "semicolona.h"

void inspice_semicolona(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus
) {
    const signum_t *signa = lex->signa;

    for (int vi = 0; vi < num_versus; vi++) {
        const versus_t *v = &versus[vi];
        for (int j = v->tok_initium; j < v->tok_finis; j++) {
            if (signa[j].genus != SIGNUM_SEMICOLON)
                continue;
            if (j <= v->tok_initium)
                continue;
            if (signa[j - 1].genus != SIGNUM_SPATIUM)
                continue;

            /* exceptio: for ( ; ; ) — praecedentia '(' vel ';' */
            int ante = j - 2;
            while (
                ante >= v->tok_initium &&
                signa[ante].genus == SIGNUM_SPATIUM
            )
                ante--;
            if (
                ante >= v->tok_initium &&
                (
                    signa[ante].genus == SIGNUM_APERTIO_PAR ||
                    signa[ante].genus == SIGNUM_SEMICOLON
                )
            )
                continue;

            inspector_adde(
                ins, GRAVITAS_MONITUM,
                signa[j].linea, signa[j].columna,
                "spatium_semicolon",
                "spatium non debet esse ante semicolon"
            );
        }
    }
}
