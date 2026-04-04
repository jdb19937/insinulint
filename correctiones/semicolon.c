/*
 * correctiones/semicolon.c — correctio spatiorum ante semicolon
 *
 * Removet spatia ante ';' extra chordas et litteras.
 */

#include "../correctio.h"

#include <string.h>

char *corrige_spatium_semicolon(
    char *wp, const char *corpus, int corp_lon
) {
    char *wp_initium = wp;

    for (int i = 0; i < corp_lon; i++) {
        char c = corpus[i];

        /* transili litteras chordarum */
        if (c == '"' || c == '\'') {
            char delim = c;
            *wp++      = corpus[i];
            for (i++; i < corp_lon; i++) {
                *wp++ = corpus[i];
                if (corpus[i] == '\\' && i + 1 < corp_lon) {
                    i++;
                    *wp++ = corpus[i];
                } else if (corpus[i] == delim) {
                    break;
                }
            }
            continue;
        }

        /* transili commentaria '//' */
        if (c == '/' && i + 1 < corp_lon && corpus[i + 1] == '/') {
            memcpy(wp, corpus + i, corp_lon - i);
            wp += corp_lon - i;
            i = corp_lon;
            break;
        }

        /* semicolon: remove spatia ante */
        if (c == ';') {
            while (wp > wp_initium && *(wp - 1) == ' ')
                wp--;
            *wp++ = ';';
            continue;
        }

        *wp++ = c;
    }
    *wp++ = '\n';
    return wp;
}
