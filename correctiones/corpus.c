/*
 * correctiones/corpus.c — scissio corporis separati
 *
 * "if (x) y;" → "if (x)\n    y;"
 */

#include "../correctio.h"

#include <string.h>

char *corrige_corpus(
    char *wp, const char *corpus, int corp_lon,
    int col, int ind
) {
    int cc = col;
    if (cc < 0)
        cc = 0;
    if (cc > corp_lon)
        cc = corp_lon;

    /* pars 1: controlum usque ad corpus */
    int ante_lon = cc;
    while (
        ante_lon > 0 &&
        (corpus[ante_lon - 1] == ' ' || corpus[ante_lon - 1] == '\t')
    )
        ante_lon--;
    memcpy(wp, corpus, ante_lon);
    wp += ante_lon;
    *wp++ = '\n';

    /* pars 2: corpus in linea nova cum indentatione */
    for (int j = 0; j < ind; j++)
        *wp++ = ' ';

    const char *rest     = corpus + cc;
    int         rest_lon = corp_lon - cc;
    while (
        rest_lon > 0 &&
        (rest[rest_lon - 1] == ' ' || rest[rest_lon - 1] == '\t')
    )
        rest_lon--;
    memcpy(wp, rest, rest_lon);
    wp += rest_lon;
    *wp++ = '\n';
    return wp;
}
