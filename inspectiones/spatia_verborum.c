/*
 * inspectiones/spatia_verborum.c — spatia post verba clavis
 */

#include "../inspectio.h"

#include <stdio.h>

void inspice_spatia_verborum(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus
) {
    const signum_t *signa = lex->signa;

    for (int vi = 0; vi < num_versus; vi++) {
        const versus_t *v = &versus[vi];
        for (int j = v->tok_initium; j < v->tok_finis; j++) {
            if (!est_verbum_cum_spatio(&signa[j]))
                continue;
            /* inspice sequens signum */
            int seq = j + 1;
            if (seq >= v->tok_finis)
                continue;
            /* si sequens est '(' directe sine spatio */
            if (signa[seq].genus == SIGNUM_APERTIO_PAR) {
                char nuntius[NUNTIUS_MAX];
                snprintf(
                    nuntius, sizeof(nuntius),
                    "spatium requiritur post '%.*s'",
                    signa[j].longitudo, signa[j].initium
                );
                inspector_adde(
                    ins, GRAVITAS_MONITUM,
                    signa[j].linea, signa[j].columna,
                    "spatium_verbum", nuntius
                );
            }
            /* si 'return' sequitur expressio sine spatio (non '(') */
            else if (
                est_verbum(&signa[j], "return") &&
                signa[seq].genus != SIGNUM_SPATIUM &&
                signa[seq].genus != SIGNUM_SEMICOLON &&
                signa[seq].genus != SIGNUM_LINEA_NOVA
            ) {
                inspector_adde(
                    ins, GRAVITAS_MONITUM,
                    signa[j].linea, signa[j].columna,
                    "spatium_verbum",
                    "spatium requiritur post 'return'"
                );
            }
        }
    }
}
