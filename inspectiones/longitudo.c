/*
 * inspectiones/longitudo.c — longitudo maxima lineae
 */

#include "../inspectio.h"

#include <stdio.h>

void inspice_longitudinem(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus,
    int maxima
) {
    if (maxima <= 0)
        return;
    const signum_t *signa = lex->signa;

    for (int vi = 0; vi < num_versus; vi++) {
        const versus_t *v = &versus[vi];
        if (v->tok_finis <= v->tok_initium)
            continue;
        /* columna ultimi characteris */
        int ultimus = v->tok_finis - 1;
        int eff_lon = signa[ultimus].longitudo;
        /* commentaria multiliniaria: solum primam lineam metiamur */
        if (signa[ultimus].genus == SIGNUM_COMMENTARIUM) {
            for (int k = 0; k < signa[ultimus].longitudo; k++) {
                if (signa[ultimus].initium[k] == '\n') {
                    eff_lon = k;
                    break;
                }
            }
        }
        int lon = signa[ultimus].columna + eff_lon;
        if (lon > maxima) {
            char nuntius[NUNTIUS_MAX];
            snprintf(
                nuntius, sizeof(nuntius),
                "linea %d columnarum (max %d)", lon, maxima
            );
            inspector_adde(
                ins, GRAVITAS_MONITUM,
                v->numero, maxima,
                "longitudo_lineae", nuntius
            );
        }
    }
}
