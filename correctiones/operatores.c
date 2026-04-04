/*
 * correctiones/operatores.c — correctio spatiorum circa operatores
 *
 * Inserit spatia circa operatores binarios ubi desunt.
 * Solum lineas cum monitum "spatium_operator" tractat.
 */

#include "../correctio.h"

#include <string.h>

/*
 * est_pars_compositi — 1 si '=' in positione i est pars operatoris compositi.
 * Compositi: ==, !=, <=, >=, +=, -=, *=, /=, %=, &=, |=, ^=, <<=, >>=
 */
static int est_pars_compositi(const char *corpus, int corp_lon, int i)
{
    if (i > 0) {
        char p = corpus[i - 1];
        if (
            p == '!' || p == '<' || p == '>' || p == '+' ||
            p == '-' || p == '*' || p == '/' || p == '%' ||
            p == '&' || p == '|' || p == '^' || p == '='
        )
            return 1;
    }
    if (i + 1 < corp_lon && corpus[i + 1] == '=')
        return 1;
    return 0;
}

char *corrige_operatores(
    char *wp, const char *corpus, int corp_lon
) {
    char *initium_wp = wp;

    for (int i = 0; i < corp_lon; i++) {
        char c = corpus[i];

        /* transili litteras chordarum */
        if (c == '"' || c == '\'') {
            char delim = c;
            *wp++ = corpus[i];
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

        /* '=' simplex (adtributio) — non compositus */
        if (c == '=' && !est_pars_compositi(corpus, corp_lon, i)) {
            /* spatium ante */
            if (wp > initium_wp && *(wp - 1) != ' ' && *(wp - 1) != '\t')
                *wp++ = ' ';
            *wp++ = '=';
            /* spatium post (inspice sequentem, noli promovere i) */
            if (
                i + 1 < corp_lon &&
                corpus[i + 1] != ' ' && corpus[i + 1] != '\t'
            )
                *wp++ = ' ';
            continue;
        }

        *wp++ = corpus[i];
    }
    *wp++ = '\n';
    return wp;
}
