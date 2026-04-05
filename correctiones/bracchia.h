/*
 * bracchia.h — correctio bracchiorum
 */

#ifndef CORRECTIONES_BRACCHIA_H
#define CORRECTIONES_BRACCHIA_H

#include "../insinulint.h"
#include "communes.h"

char *corrige_bracchia_kr(
    char *wp, const char *corpus, int corp_lon,
    const linea_t *lineae, int i, int nlin,
    int *transili, int una_ind_prox, const speculum_t *spec
);

char *corrige_bracchia_allman(
    char *wp, const char *corpus, int corp_lon,
    int col, int ind, const speculum_t *spec
);

int correctio_bracchia_necessaria(
    const char *via, const speculum_t *spec
);

#endif /* CORRECTIONES_BRACCHIA_H */
