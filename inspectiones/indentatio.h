/*
 * indentatio.h — inspectio indentationis
 */

#ifndef INSPECTIONES_INDENTATIO_H
#define INSPECTIONES_INDENTATIO_H

#include "../insinulint.h"
#include "auxiliaria.h"

typedef enum {
    CONTINUATIO_CONGRUENS = 0,  /* ad columnam post '(' alignata */
    CONTINUATIO_MASSA,          /* indentata per unum gradum */
    CONTINUATIO_PENDENS,        /* '(' in fine, contentum +1 gradus */
    CONTINUATIO_PATENS           /* ut pendens, ')' primum in linea */
} continuatio_t;

void inspice_indentationem(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus,
    const speculum_t *spec
);

#endif /* INSPECTIONES_INDENTATIO_H */
