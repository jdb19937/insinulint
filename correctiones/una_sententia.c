/*
 * correctiones/una_sententia.c — scinde plures sententias in lineas proprias
 *
 * "{ d++; q++; continue; }" → lineae separatae cum indentione.
 */

#include "../correctio.h"
#include "una_sententia.h"

#include <string.h>

/*
 * est_labellum_char — character validus in verbo (identificator) */
static int est_labellum_char(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') || c == '_';
}

/*
 * quaere_labellum — si corpus incipit cum labello (case X:, default:,
 * foo:), reddit indicem byti post ':'; aliter -1.
 */
static int quaere_labellum(const char *corpus, int corp_lon)
{
    int i = 0;
    while (i < corp_lon && (corpus[i] == ' ' || corpus[i] == '\t'))
        i++;
    if (i >= corp_lon)
        return -1;

    int incipit_case = (
        i + 5 <= corp_lon && strncmp(corpus + i, "case ", 5) == 0
    );
    int incipit_default = (
        i + 7 <= corp_lon &&
        strncmp(corpus + i, "default", 7) == 0 &&
        (i + 7 == corp_lon || !est_labellum_char(corpus[i + 7]))
    );

    if (incipit_case || incipit_default) {
        /* quaere primum ':' ad profunditatem 0 */
        int prof = 0;
        for (int j = i; j < corp_lon; j++) {
            char c = corpus[j];
            if (c == '(' || c == '[')
                prof++;
            else if (c == ')' || c == ']') {
                if (prof > 0)
                    prof--;
            } else if (c == ':' && prof == 0) {
                /* exclude '::' (C++, non C, sed ad defensionem) */
                if (j + 1 < corp_lon && corpus[j + 1] == ':')
                    return -1;
                return j + 1;
            }
        }
        return -1;
    }

    /* labellum generale: identificator + ':' */
    if (
        !est_labellum_char(corpus[i]) ||
        (corpus[i] >= '0' && corpus[i] <= '9')
    )
        return -1;
    int j = i;
    while (j < corp_lon && est_labellum_char(corpus[j]))
        j++;
    while (j < corp_lon && (corpus[j] == ' ' || corpus[j] == '\t'))
        j++;
    if (j < corp_lon && corpus[j] == ':') {
        if (j + 1 < corp_lon && corpus[j + 1] == ':')
            return -1;
        return j + 1;
    }
    return -1;
}

char *corrige_unam_sententiam(
    char *wp, const char *corpus, int corp_lon,
    int ind, const speculum_t *spec
) {
    int lat = spec->ind_tabulis ? 8 : spec->ind_latitudo;
    if (lat <= 0)
        lat = 4;

    /* si linea incipit cum labello, scribe labellum in linea propria
     * cum indentatione sua (ind), dein indentatione auctata sequentibus
     * sententiis (ind + lat). Corpus ad recursionem transmittitur. */
    if (spec->lin_schedae_propria) {
        int post_lab = quaere_labellum(corpus, corp_lon);
        if (post_lab > 0) {
            int lab_init = 0;
            while (
                lab_init < corp_lon &&
                (
                    corpus[lab_init] == ' ' ||
                    corpus[lab_init] == '\t'
                )
            )
                lab_init++;
            /* scribe labellum ipsum (sine spatiis initialibus —
             * indentatio iam emissa est ante invocationem) */
            int lab_lon = post_lab - lab_init;
            memcpy(wp, corpus + lab_init, lab_lon);
            wp += lab_lon;
            *wp++ = '\n';
            /* transili spatia post ':' */
            int rest = post_lab;
            while (
                rest < corp_lon &&
                (corpus[rest] == ' ' || corpus[rest] == '\t')
            )
                rest++;
            if (rest >= corp_lon)
                return wp;
            /* scribe indentationem et corpus restans per recursionem */
            wp = scribe_indentationem(wp, ind + lat, spec);
            return corrige_unam_sententiam(
                wp, corpus + rest, corp_lon - rest,
                ind + lat, spec
            );
        }
    }

    int ind_scindi = ind;

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
