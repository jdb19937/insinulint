/*
 * corpus.h — correctio corporis
 */

#ifndef CORRECTIONES_CORPUS_H
#define CORRECTIONES_CORPUS_H

#include "../insinulint.h"

char *corrige_corpus(
    char *wp, const char *corpus, int corp_lon,
    int col, int ind, const speculum_t *spec
);

#endif /* CORRECTIONES_CORPUS_H */
