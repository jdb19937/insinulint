/*
 * cubitus.h — correctio cubiti
 */

#ifndef CORRECTIONES_CUBITUS_H
#define CORRECTIONES_CUBITUS_H

#include "communes.h"

char *corrige_cubitum(
    char *wp, const char *corpus, int corp_lon,
    const linea_t *lineae, int i, int nlin,
    int *transili
);

#endif /* CORRECTIONES_CUBITUS_H */
