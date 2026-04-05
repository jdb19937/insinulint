/*
 * longitudo.h — inspectio longitudinis linearum
 */

#ifndef INSPECTIONES_LONGITUDO_H
#define INSPECTIONES_LONGITUDO_H

#include "../insinulint.h"
#include "auxiliaria.h"

void inspice_longitudinem(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus,
    int maxima, int lat
);

#endif /* INSPECTIONES_LONGITUDO_H */
