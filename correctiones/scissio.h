/*
 * scissio.h — correctio scissionis linearum
 */

#ifndef CORRECTIONES_SCISSIO_H
#define CORRECTIONES_SCISSIO_H

#include "../insinulint.h"
#include "communes.h"

char *corrige_scissionem(
    char *wp, const char *corpus, int corp_lon,
    int col, int ind, int sp_init,
    const linea_t *l, const linea_t *lineae,
    int i, int nlin, int *transili,
    const speculum_t *spec
);

#endif /* CORRECTIONES_SCISSIO_H */
