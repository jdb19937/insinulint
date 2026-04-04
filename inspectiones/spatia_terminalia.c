/*
 * inspectiones/spatia_terminalia.c — veta spatia ad finem lineae
 */

#include "../inspectio.h"

void inspice_spatia_terminalia(
    inspector_t *ins,
    const lexator_t *lex,
    const versus_t *versus,
    int num_versus
) {
    const signum_t *signa = lex->signa;

    for (int vi = 0; vi < num_versus; vi++) {
        const versus_t *v = &versus[vi];
        if (v->tok_finis <= v->tok_initium)
            continue;
        int ultimus = v->tok_finis - 1;
        if (signa[ultimus].genus == SIGNUM_SPATIUM) {
            adde_fix(
                ins, GRAVITAS_MONITUM,
                v->numero, signa[ultimus].columna,
                "spatia_terminalia",
                "spatia terminalia inventa", 1
            );
        }
    }
}
