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
    int lat = spec->ind_tabulis
        ? spec->ind_latitudo * 8
        : spec->ind_latitudo;
    if (lat <= 0)
        lat = 4;

    /* case/default: corpus scindendum indentatur uno gradu profundius */
    int est_casus = 0;
    if (corp_lon >= 5 && strncmp(corpus, "case ", 5) == 0)
        est_casus = 1;
    else if (corp_lon >= 7 && strncmp(corpus, "default", 7) == 0) {
        char seq = (corp_lon > 7) ? corpus[7] : 0;
        if (seq == ':' || seq == ' ' || seq == '\t')
            est_casus = 1;
    }
    int ind_scindi = est_casus ? ind + lat : ind;

    int prof_par = 0;
    int prof_bra = 0;
    int pos      = 0;

    while (pos < corp_lon) {
        char c = corpus[pos];

        if (c == '(' || c == '[')
            prof_par++;
        else if (c == ')' || c == ']') {
            if (prof_par > 0)
                prof_par--;
        }

        if (prof_par > 0) {
            *wp++ = c;
            pos++;
            continue;
        }

        /* '{' — scribe et scinde ad novam lineam cum indentione aucta */
        if (c == '{') {
            *wp++ = '{';
            pos++;
            prof_bra++;
            /* transili spatia post '{' */
            while (
                pos < corp_lon &&
                (corpus[pos] == ' ' || corpus[pos] == '\t')
            )
                pos++;
            if (pos < corp_lon) {
                *wp++ = '\n';
                wp = scribe_indentationem(
                    wp, ind + prof_bra * lat, spec
                );
            }
            continue;
        }

        /* '}' — scinde ad novam lineam cum indentione reducta */
        if (c == '}') {
            if (prof_bra > 0)
                prof_bra--;
            *wp++ = '\n';
            wp    = scribe_indentationem(wp, ind + prof_bra * lat, spec);
            *wp++ = '}';
            pos++;
            /* transili spatia post '}' */
            while (
                pos < corp_lon &&
                (corpus[pos] == ' ' || corpus[pos] == '\t')
            )
                pos++;
            continue;
        }

        *wp++ = c;
        pos++;

        /* post ';' extra parentheses, scinde si restat contentum */
        if (c == ';' && pos < corp_lon) {
            /* transili spatia */
            int rest = pos;
            while (
                rest < corp_lon &&
                (corpus[rest] == ' ' || corpus[rest] == '\t')
            )
                rest++;

            if (rest >= corp_lon)
                continue;

            /* si '}' sequitur, permitte handler '}' tractare */
            if (corpus[rest] == '}') {
                pos = rest;
                continue;
            }

            /* scinde ad novam lineam */
            *wp++ = '\n';
            wp = scribe_indentationem(
                wp, ind_scindi + prof_bra * lat, spec
            );
            pos = rest;
        }
    }
    *wp++ = '\n';
    return wp;
}
