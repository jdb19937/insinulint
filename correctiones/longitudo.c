/*
 * correctiones/longitudo.c — scissio linearum longarum
 *
 * Scindit lineas quae longitudinem maximam excedunt ad operatorem
 * binarium, cum continuatione indentata uno gradu extra.
 */

#include "../correctio.h"

#include <string.h>

/* operatores ubi scindere licet, per prioritatem */
static int est_op_scissionis(const char *s, int lon, int *oplon)
{
    /* duo characteres */
    if (lon >= 2) {
        if (
            (s[0] == '|' && s[1] == '|') ||
            (s[0] == '&' && s[1] == '&') ||
            (s[0] == '=' && s[1] == '=') ||
            (s[0] == '!' && s[1] == '=') ||
            (s[0] == '<' && s[1] == '=') ||
            (s[0] == '>' && s[1] == '=') ||
            (s[0] == '+' && s[1] == '=') ||
            (s[0] == '-' && s[1] == '=') ||
            (s[0] == '*' && s[1] == '=') ||
            (s[0] == '/' && s[1] == '=') ||
            (s[0] == '%' && s[1] == '=') ||
            (s[0] == '&' && s[1] == '=') ||
            (s[0] == '|' && s[1] == '=') ||
            (s[0] == '^' && s[1] == '=')
        ) {
            *oplon = 2;
            return 1;
        }
    }
    /* unus character */
    if (
        s[0] == '+' || s[0] == '-' || s[0] == '*' ||
        s[0] == '/' || s[0] == '%' || s[0] == '|' ||
        s[0] == '&' || s[0] == '^' || s[0] == '?' ||
        s[0] == ':'
    ) {
        /* evita unarios: si praecedens est '(' vel ',' vel op */
        *oplon = 1;
        return 1;
    }
    if (s[0] == '=') {
        /* simplex adtributio, non pars compositi */
        if (lon >= 2 && s[1] == '=')
            return 0;
        *oplon = 1;
        return 1;
    }
    return 0;
}

char *corrige_longitudinem(
    char *wp, const char *corpus, int corp_lon,
    int sp_init, int cont_ind, int maxima,
    const speculum_t *spec
) {
    /* inveni ultimum operatorem ante limitem */
    int col       = sp_init;
    int in_chorda = 0;
    int prof_par  = 0;
    int sciss_pos = -1;

    for (int i = 0; i < corp_lon; i++) {
        char c = corpus[i];

        /* transili chordas */
        if ((c == '"' || c == '\'') && !in_chorda) {
            in_chorda  = 1;
            char delim = c;
            for (i++; i < corp_lon; i++) {
                col++;
                if (corpus[i] == '\\' && i + 1 < corp_lon) {
                    i++;
                    col++;
                } else if (corpus[i] == delim) {
                    in_chorda = 0;
                    break;
                }
            }
            col++;
            continue;
        }

        /* transili commentaria */
        if (c == '/' && i + 1 < corp_lon && corpus[i + 1] == '/') {
            break;
        }

        /* parentheses */
        if (
            c == '(' || c == '[' ||
            c == '{' /* intra expressione */
        )
            prof_par++;
        else if (
            c == ')' || c == ']' ||
            c == '}'
        ) {
            if (prof_par > 0)
                prof_par--;
        }

        /* inspice operatores solum extra parentheses */
        if (prof_par == 0) {
            int oplon = 0;
            if (est_op_scissionis(corpus + i, corp_lon - i, &oplon)) {
                /* praeferi scissionem ante limitem */
                if (col + oplon <= maxima)
                    sciss_pos = i;
            }
        }

        col++;
    }

    if (sciss_pos < 0) {
        /* nullum punctum scissionis inventum — scribe ut est */
        memcpy(wp, corpus, corp_lon);
        wp += corp_lon;
        *wp++ = '\n';
        return wp;
    }

    /* scinde post operatorem (prae-stilus: operator in fine primae
     * lineae) */
    int oplon = 0;
    est_op_scissionis(
        corpus + sciss_pos, corp_lon - sciss_pos, &oplon
    );
    int pars1_fin = sciss_pos + oplon;

    /* remove spatia terminalia partis primae */
    int p1 = pars1_fin;
    while (
        p1 > 0 &&
        (corpus[p1 - 1] == ' ' || corpus[p1 - 1] == '\t')
    )
        p1--;

    /* si operator est ultimus non-spatium, scinde ante operatorem
     * (post-stilus: operator in initio secundae lineae) */
    int pars2_init = pars1_fin;
    while (
        pars2_init < corp_lon &&
        (corpus[pars2_init] == ' ' || corpus[pars2_init] == '\t')
    )
        pars2_init++;

    /* scribe partem primam cum operatore */
    memcpy(wp, corpus, p1);
    wp += p1;
    *wp++ = '\n';

    /* scribe continuationem */
    wp = scribe_indentationem(wp, cont_ind, spec);

    int rest_lon = corp_lon - pars2_init;
    while (
        rest_lon > 0 &&
        (
            corpus[pars2_init + rest_lon - 1] == ' ' ||
            corpus[pars2_init + rest_lon - 1] == '\t'
        )
    )
        rest_lon--;
    memcpy(wp, corpus + pars2_init, rest_lon);
    wp += rest_lon;
    *wp++ = '\n';

    return wp;
}
