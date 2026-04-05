/*
 * commendatio.h — exitus structuratus monitorum cum contextu
 *
 * Pro quoque monito, lineas fontis circumiacentes ad stdout scribit,
 * in forma simili grep -n vel diff.
 */

#ifndef COMMENDATIO_H
#define COMMENDATIO_H

#include "insinulint.h"

/*
 * commendatio_scribe — scribe monita cum contextu ad stdout.
 * ala = lineae ante et post centrum (totum = 2*ala+1).
 * spec potest esse NULL (metadata non scribentur).
 * reddit numerum monitorum scriptorum.
 */
int commendatio_scribe(
    const inspector_t *ins,
    const char *fons,
    const speculum_t *spec,
    int ala
);

#endif /* COMMENDATIO_H */
