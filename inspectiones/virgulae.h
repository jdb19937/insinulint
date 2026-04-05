/*
 * virgulae.h — inspectio spatiorum virgularum
 */

#ifndef INSPECTIONES_VIRGULAE_H
#define INSPECTIONES_VIRGULAE_H

#include "../insinulint.h"
#include "auxiliaria.h"

void inspice_virgulas(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus
);

#endif /* INSPECTIONES_VIRGULAE_H */
