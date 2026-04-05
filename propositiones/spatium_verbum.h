/*
 * propositiones/spatium_verbum.h — propositio spatii post verba clavis
 */

#ifndef PROPOSITIONES_SPATIUM_VERBUM_H
#define PROPOSITIONES_SPATIUM_VERBUM_H

#include "../propositio.h"

int propone_spatium_verbum(
    const prop_linea_t *centrum,
    char *dest, int dest_max
);

#endif /* PROPOSITIONES_SPATIUM_VERBUM_H */
