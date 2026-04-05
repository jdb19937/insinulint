/*
 * tabulae_mixtae.h — inspectio tabularum mixtarum
 */

#ifndef INSPECTIONES_TABULAE_MIXTAE_H
#define INSPECTIONES_TABULAE_MIXTAE_H

#include "../insinulint.h"
#include "auxiliaria.h"

void inspice_tabulas_mixtas(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus
);

#endif /* INSPECTIONES_TABULAE_MIXTAE_H */
