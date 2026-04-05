/*
 * longitudo.h — correctio longitudinis linearum
 */

#ifndef CORRECTIONES_LONGITUDO_H
#define CORRECTIONES_LONGITUDO_H

#include "../insinulint.h"

char *corrige_longitudinem(
    char *wp, const char *corpus, int corp_lon,
    int sp_init, int cont_ind, int maxima,
    const speculum_t *spec
);

#endif /* CORRECTIONES_LONGITUDO_H */
