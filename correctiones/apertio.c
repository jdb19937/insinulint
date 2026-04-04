/*
 * correctiones/apertio.c — scissio post '(' (stilus patens)
 */

#include "../correctio.h"

#include <string.h>

char *corrige_apertionem(
    char *wp, const char *corpus, int corp_lon,
    int col, int ind, int sp_init, const speculum_t *spec
) {
    int par_byte = col - sp_init;
    if (par_byte < 0)
        par_byte = 0;
    if (par_byte >= corp_lon)
        par_byte = corp_lon - 1;

    /* pars 1: contentum usque ad '(' inclusive */
    int ante_lon = par_byte + 1;
    memcpy(wp, corpus, ante_lon);
    wp += ante_lon;
    *wp++ = '\n';

    /* pars 2: contentum post '(' in linea nova */
    wp = scribe_indentationem(wp, ind, spec);

    const char *rest     = corpus + ante_lon;
    int         rest_lon = corp_lon - ante_lon;
    /* salta spatia initialia post '(' */
    while (rest_lon > 0 && (rest[0] == ' ' || rest[0] == '\t')) {
        rest++;
        rest_lon--;
    }
    /* remove spatia terminalia */
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
