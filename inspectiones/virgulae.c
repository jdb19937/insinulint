/*
 * inspectiones/virgulae.c — spatia post virgulam
 */

#include "../inspectio.h"
#include "virgulae.h"

void inspice_virgulas(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus
) {
    const signum_t *signa = lex->signa;

    for (int vi = 0; vi < num_versus; vi++) {
        const versus_t *v = &versus[vi];
        for (int j = v->tok_initium; j < v->tok_finis; j++) {
            if (!est_operator(&signa[j], ","))
                continue;

            /* spatium ante virgulam non debet esse */
            if (
                j > v->tok_initium &&
                signa[j - 1].genus == SIGNUM_SPATIUM
            ) {
                inspector_adde(
                    ins, GRAVITAS_MONITUM,
                    signa[j].linea, signa[j].columna,
                    "spatium_virgula",
                    "spatium non debet esse ante virgulam"
                );
            }

            /* spatium post virgulam debet esse */
            int seq = j + 1;
            if (
                seq < v->tok_finis &&
                signa[seq].genus != SIGNUM_SPATIUM &&
                signa[seq].genus != SIGNUM_LINEA_NOVA
            ) {
                inspector_adde(
                    ins, GRAVITAS_MONITUM,
                    signa[j].linea, signa[j].columna,
                    "spatium_virgula",
                    "spatium requiritur post virgulam"
                );
            }
        }
    }
}
