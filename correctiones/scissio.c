/*
 * correctiones/scissio.c — scissio ')' ad lineam novam (stilus patens)
 */

#include "../correctio.h"

#include <string.h>

char *corrige_scissionem(
    char *wp, const char *corpus, int corp_lon,
    int col, int ind, int sp_init,
    const linea_t *l, const linea_t *lineae,
    int i, int nlin, int *transili
) {
    int par_byte = col;
    int ante_lon = par_byte - sp_init;
    if (ante_lon < 0)
        ante_lon = 0;
    if (ante_lon > corp_lon)
        ante_lon = corp_lon;

    /* pars 1: contentum ante ')' */
    while (
        ante_lon > 0 &&
        (corpus[ante_lon - 1] == ' ' || corpus[ante_lon - 1] == '\t')
    )
        ante_lon--;
    memcpy(wp, corpus, ante_lon);
    wp += ante_lon;
    *wp++ = '\n';

    /* pars 2: ')' et residuum in linea nova */
    for (int j = 0; j < ind; j++)
        *wp++ = ' ';

    const char *rest     = l->initium + par_byte;
    int         rest_lon = l->lon - par_byte;
    while (
        rest_lon > 0 &&
        (rest[rest_lon - 1] == ' ' || rest[rest_lon - 1] == '\t')
    )
        rest_lon--;

    /* si residuum est solum ')' et linea proxima est '{',
     * iunge in ') {' */
    int prox_apertio = 0;
    if (rest_lon == 1 && rest[0] == ')' && i + 1 < nlin) {
        const linea_t *ln = &lineae[i + 1];
        int k = 0;
        while (
            k < ln->lon &&
            (ln->initium[k] == ' ' || ln->initium[k] == '\t')
        )
            k++;
        if (k < ln->lon && ln->initium[k] == '{') {
            int m2 = k + 1;
            while (
                m2 < ln->lon &&
                (ln->initium[m2] == ' ' || ln->initium[m2] == '\t')
            )
                m2++;
            if (m2 == ln->lon)
                prox_apertio = 1;
        }
    }

    memcpy(wp, rest, rest_lon);
    wp += rest_lon;
    if (prox_apertio) {
        *wp++     = ' ';
        *wp++     = '{';
        *transili = 1;
    }
    *wp++ = '\n';
    return wp;
}
