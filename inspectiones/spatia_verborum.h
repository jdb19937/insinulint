/*
 * spatia_verborum.h — inspectio spatiorum post verba clavis
 */

#ifndef INSPECTIONES_SPATIA_VERBORUM_H
#define INSPECTIONES_SPATIA_VERBORUM_H

#include "../insinulint.h"
#include "auxiliaria.h"

void inspice_spatia_verborum(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus
);

#endif /* INSPECTIONES_SPATIA_VERBORUM_H */
