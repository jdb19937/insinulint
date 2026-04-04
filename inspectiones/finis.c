/*
 * inspectiones/finis.c — linea nova ad finem fasciculi
 */

#include "../inspectio.h"

void inspice_finem(inspector_t *ins, const lexator_t *lex)
{
    int n = lex->num_signa;
    if (n < 2)
        return;
    /* ultimum signum est FINIS; inspice penultimum */
    if (lex->signa[n - 2].genus != SIGNUM_LINEA_NOVA) {
        inspector_adde(
            ins, GRAVITAS_MONITUM,
            lex->signa[n - 1].linea, 0,
            "finis_lineae",
            "fasciculus non finit cum linea nova"
        );
    }
}
