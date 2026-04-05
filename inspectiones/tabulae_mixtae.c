/*
 * inspectiones/tabulae_mixtae.c — tabulae et spatia mixta in indentatione
 */

#include "../inspectio.h"
#include "tabulae_mixtae.h"

void inspice_tabulas_mixtas(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus
) {
    const signum_t *signa = lex->signa;

    for (int vi = 0; vi < num_versus; vi++) {
        const versus_t *v = &versus[vi];
        if (v->tok_initium >= v->tok_finis)
            continue;
        if (signa[v->tok_initium].genus != SIGNUM_SPATIUM)
            continue;
        const signum_t *sp = &signa[v->tok_initium];
        int habet_tab      = 0, habet_sp = 0;
        for (int k = 0; k < sp->longitudo; k++) {
            if (sp->initium[k] == '\t')
                habet_tab = 1;
            if (sp->initium[k] == ' ')
                habet_sp = 1;
        }
        if (habet_tab && habet_sp) {
            adde_fix(
                ins, GRAVITAS_MONITUM,
                v->numero, 0,
                "tabulae_mixtae",
                "tabulae et spatia mixta in indentatione",
                v->spatia
            );
        }
    }
}
