/*
 * correctiones/una_sententia.c — scinde plures sententias in lineas proprias
 *
 * "d++; q++; continue; }" → lineae separatae cum indentione.
 */

#include "../correctio.h"

#include <string.h>

char *corrige_unam_sententiam(
    char *wp, const char *corpus, int corp_lon,
    int ind, const speculum_t *spec
) {
    int lat = spec->ind_tabulis
        ? spec->ind_latitudo * 8
        : spec->ind_latitudo;
    if (lat <= 0)
        lat = 4;

    int prof_par = 0;
    int pos = 0;

    while (pos < corp_lon) {
        char c = corpus[pos];

        if (c == '(' || c == '[')
            prof_par++;
        else if (c == ')' || c == ']') {
            if (prof_par > 0)
                prof_par--;
        }

        *wp++ = c;
        pos++;

        /* post ';' extra parentheses, scinde si restat contentum */
        if (c == ';' && prof_par == 0 && pos < corp_lon) {
            /* transili spatia */
            int rest = pos;
            while (
                rest < corp_lon &&
                (corpus[rest] == ' ' || corpus[rest] == '\t')
            )
                rest++;

            if (rest >= corp_lon)
                continue;

            /* si '}' sequitur, pone in linea propria cum indentione reducta */
            if (corpus[rest] == '}') {
                *wp++ = '\n';
                int ind_cl = (ind >= lat) ? ind - lat : 0;
                wp = scribe_indentationem(wp, ind_cl, spec);
                pos = rest;
                continue;
            }

            /* aliter scinde ad novam sententiam */
            *wp++ = '\n';
            wp = scribe_indentationem(wp, ind, spec);
            pos = rest;
        }
    }
    *wp++ = '\n';
    return wp;
}
