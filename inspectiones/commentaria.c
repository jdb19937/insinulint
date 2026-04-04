/*
 * inspectiones/commentaria.c — commentaria vetita
 */

#include "../inspectio.h"

void inspice_commentaria(
    inspector_t *ins, const lexator_t *lex
) {
    for (int i = 0; i < lex->num_signa; i++) {
        const signum_t *s = &lex->signa[i];
        if (s->genus == SIGNUM_COMMENTARIUM)
            inspector_adde(
                ins, GRAVITAS_MONITUM, s->linea, s->columna,
                "commentaria_vetita", "commentarium inventum"
            );
    }
}
