/*
 * inspectiones/spatia_operatorum.c — spatia circa operatores binarios
 */

#include "../inspectio.h"
#include "spatia_operatorum.h"

#include <stdio.h>

void inspice_spatia_operatorum(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus
) {
    const signum_t *signa = lex->signa;

    for (int vi = 0; vi < num_versus; vi++) {
        const versus_t *v = &versus[vi];
        for (int j = v->tok_initium; j < v->tok_finis; j++) {
            if (!est_operator_binarius(&signa[j]))
                continue;

            /* inspice spatium ante operatorem */
            if (
                j > v->tok_initium &&
                signa[j - 1].genus != SIGNUM_SPATIUM
            ) {
                char nuntius[NUNTIUS_MAX];
                snprintf(
                    nuntius, sizeof(nuntius),
                    "spatium requiritur ante '%.*s'",
                    signa[j].longitudo, signa[j].initium
                );
                inspector_adde(
                    ins, GRAVITAS_MONITUM,
                    signa[j].linea, signa[j].columna,
                    "spatium_operator", nuntius
                );
            }

            /* inspice spatium post operatorem */
            int seq = j + 1;
            if (
                seq < v->tok_finis &&
                signa[seq].genus != SIGNUM_SPATIUM &&
                signa[seq].genus != SIGNUM_LINEA_NOVA
            ) {
                char nuntius[NUNTIUS_MAX];
                snprintf(
                    nuntius, sizeof(nuntius),
                    "spatium requiritur post '%.*s'",
                    signa[j].longitudo, signa[j].initium
                );
                inspector_adde(
                    ins, GRAVITAS_MONITUM,
                    signa[j].linea, signa[j].columna,
                    "spatium_operator", nuntius
                );
            }
        }
    }
}
