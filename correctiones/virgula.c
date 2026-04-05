/*
 * correctiones/virgula.c — correctio spatiorum circa virgulas
 *
 * Remove spatia ante virgulam, adde spatium post virgulam.
 */

#include "../correctio.h"
#include "virgula.h"

char *corrige_virgulas(
    char *wp, const char *corpus, int corp_lon
) {
    char *wp_initium = wp;
    int in_chorda    = 0;
    int in_littera   = 0;

    for (int i = 0; i < corp_lon; i++) {
        char c = corpus[i];

        /* effugium intra chordam vel litteram */
        if (
            (in_chorda || in_littera) &&
            c == '\\' && i + 1 < corp_lon
        ) {
            *wp++ = c;
            *wp++ = corpus[++i];
            continue;
        }

        if (c == '"' && !in_littera) {
            in_chorda = !in_chorda;
            *wp++     = c;
            continue;
        }
        if (c == '\'' && !in_chorda) {
            in_littera = !in_littera;
            *wp++      = c;
            continue;
        }

        if (in_chorda || in_littera) {
            *wp++ = c;
            continue;
        }

        /* virgula extra chordas */
        if (c == ',') {
            /* remove spatia ante virgulam */
            while (wp > wp_initium && *(wp - 1) == ' ')
                wp--;

            *wp++ = ',';

            /* adde spatium post virgulam si sequitur contentum */
            if (
                i + 1 < corp_lon &&
                corpus[i + 1] != ' ' &&
                corpus[i + 1] != '\t'
            )
                *wp++ = ' ';

            continue;
        }

        *wp++ = c;
    }

    *wp++ = '\n';
    return wp;
}
