/*
 * correctiones/colineatio.c — colineatio adtributionum ('=')
 */

#include "../correctio.h"

#include <string.h>

char *corrige_colineationem(
    char *wp, const char *corpus, int corp_lon,
    int col, int spa, int sp_init
) {
    int eq_off = col - sp_init;
    int delta  = spa;
    if (eq_off < 0)        eq_off = 0;
    if (eq_off > corp_lon) eq_off = corp_lon;
    if (delta >= 0) {
        memcpy(wp, corpus, eq_off);
        wp += eq_off;
        for (int j = 0; j < delta; j++) *wp++ = ' ';
    } else {
        int new_off = eq_off + delta;
        if (new_off < 0) new_off = 0;
        memcpy(wp, corpus, new_off);
        wp += new_off;
    }
    memcpy(wp, corpus + eq_off, corp_lon - eq_off);
    wp += corp_lon - eq_off;
    *wp++ = '\n';
    return wp;
}
