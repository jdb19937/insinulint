/*
 * propositiones/spatium_operator.h — propositio spatiorum operatorum
 */

#ifndef PROPOSITIONES_SPATIUM_OPERATOR_H
#define PROPOSITIONES_SPATIUM_OPERATOR_H

#include "../propositio.h"

int propone_spatium_operator(
    const prop_linea_t *centrum,
    char *dest, int dest_max
);

#endif /* PROPOSITIONES_SPATIUM_OPERATOR_H */
