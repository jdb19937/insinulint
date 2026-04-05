/*
 * una_sententia.h — correctio sententiarum multiplicium
 */

#ifndef CORRECTIONES_UNA_SENTENTIA_H
#define CORRECTIONES_UNA_SENTENTIA_H

#include "../insinulint.h"

char *corrige_unam_sententiam(
    char *wp, const char *corpus, int corp_lon,
    int ind, const speculum_t *spec
);

#endif /* CORRECTIONES_UNA_SENTENTIA_H */
