/*
 * correctiones/una_sententia.c — scinde plures sententias in lineas proprias
 *
 * "{ d++; q++; continue; }" → lineae separatae cum indentione.
 */

#include "../correctio.h"

#include <string.h>

char *corrige_unam_sententiam(
    char *wp, const char *corpus, int corp_lon,
    int ind, const speculum_t *spec
) {
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

            /* si restat contentum (non solum spatia), scinde */
            if (rest < corp_lon) {
                *wp++ = '\n';
                wp = scribe_indentationem(wp, ind, spec);
                pos = rest;
            }
        }
    }
    *wp++ = '\n';
    return wp;
}
