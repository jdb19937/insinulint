/*
 * una_sententia.h — inspectio sententiarum multiplicium
 */

#ifndef INSPECTIONES_UNA_SENTENTIA_H
#define INSPECTIONES_UNA_SENTENTIA_H

#include "../insinulint.h"
#include "auxiliaria.h"

void inspice_unam_sententiam(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus
);

#endif /* INSPECTIONES_UNA_SENTENTIA_H */
