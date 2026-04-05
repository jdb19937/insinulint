/*
 * inspectiones/cubitus.c — cubitus coniunctus (cuddled else)
 */

#include "../inspectio.h"
#include "cubitus.h"

void inspice_cubitum(
    inspector_t *ins, const lexator_t *lex,
    const speculum_t *spec
) {
    int n = lex->num_signa;
    const signum_t *signa = lex->signa;

    for (int i = 0; i < n; i++) {
        if (!est_verbum(&signa[i], "else"))
            continue;

        /* quaere '}' ante 'else' */
        int ante = i - 1;
        while (ante >= 0 && signa[ante].genus == SIGNUM_SPATIUM)
            ante--;

        if (ante < 0)
            continue;

        int linea_nova_inter = 0;
        if (signa[ante].genus == SIGNUM_LINEA_NOVA) {
            linea_nova_inter = 1;
            ante--;
            while (ante >= 0 && signa[ante].genus == SIGNUM_SPATIUM)
                ante--;
        }

        if (ante < 0 || signa[ante].genus != SIGNUM_CLAUSIO)
            continue;

        if (spec->bra_else_coniunctum && linea_nova_inter) {
            inspector_adde(
                ins, GRAVITAS_MONITUM,
                signa[i].linea, signa[i].columna,
                "cubitus",
                "'else' debet esse in eadem linea ac '}'"
            );
        }
        if (!spec->bra_else_coniunctum && !linea_nova_inter) {
            inspector_adde(
                ins, GRAVITAS_MONITUM,
                signa[i].linea, signa[i].columna,
                "cubitus",
                "'else' debet esse in nova linea post '}'"
            );
        }
    }
}
