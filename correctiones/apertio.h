/*
 * apertio.h — correctio apertionis
 */

#ifndef CORRECTIONES_APERTIO_H
#define CORRECTIONES_APERTIO_H

#include "../insinulint.h"

char *corrige_apertionem(
    char *wp, const char *corpus, int corp_lon,
    int col, int ind, int sp_init, const speculum_t *spec
);

#endif /* CORRECTIONES_APERTIO_H */
