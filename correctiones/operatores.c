/*
 * correctiones/operatores.c — correctio spatiorum circa operatores
 *
 * Inserit spatia circa operatores binarios ubi desunt.
 * Tractat: =, ==, !=, <=, >=, &&, ||,
 *          +=, -=, *=, /=, %=, &=, |=, ^=, <<=, >>=
 */

#include "../correctio.h"

#include <string.h>

/* operatores binarii duo-characterum */
static const char *ops2[] = {
    "==", "!=", "<=", ">=", "&&", "||",
    "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^="
};
static const int num_ops2 = 14;

/* operatores binarii trium characterum */
static const char *ops3[] = {"<<=", ">>="};
static const int num_ops3 = 2;

/*
 * est_in_chorda — transili litteras chordarum.
 * Reddit novam positionem post chordae finem, vel -1 si non chorda.
 */
static int transili_chordam(
    char *wp, const char *corpus, int corp_lon, int i, char **wp_out
) {
    char c = corpus[i];
    if (c != '"' && c != '\'')
        return -1;
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
    *wp_out = wp;
    return i;
}

char *corrige_operatores(
    char *wp, const char *corpus, int corp_lon
) {
    char *initium_wp = wp;

    for (int i = 0; i < corp_lon; i++) {
        char c = corpus[i];

        /* transili litteras chordarum */
        if (c == '"' || c == '\'') {
            int ni = transili_chordam(wp, corpus, corp_lon, i, &wp);
            if (ni >= 0) {
                i = ni;
                continue;
            }
        }

        /* transili commentaria '//' */
        if (c == '/' && i + 1 < corp_lon && corpus[i + 1] == '/') {
            memcpy(wp, corpus + i, corp_lon - i);
            wp += corp_lon - i;
            i = corp_lon;
            break;
        }

        /* operatores tres-characterum */
        if (i + 2 < corp_lon) {
            int inventum = 0;
            for (int k = 0; k < num_ops3; k++) {
                if (
                    corpus[i] == ops3[k][0] &&
                    corpus[i + 1] == ops3[k][1] &&
                    corpus[i + 2] == ops3[k][2]
                ) {
                    if (
                        wp > initium_wp &&
                        *(wp - 1) != ' ' && *(wp - 1) != '\t'
                    )
                        *wp++ = ' ';
                    *wp++ = corpus[i];
                    *wp++ = corpus[i + 1];
                    *wp++ = corpus[i + 2];
                    if (
                        i + 3 < corp_lon &&
                        corpus[i + 3] != ' ' &&
                        corpus[i + 3] != '\t'
                    )
                        *wp++ = ' ';
                    i += 2;
                    inventum = 1;
                    break;
                }
            }
            if (inventum)
                continue;
        }

        /* operatores duo-characterum */
        if (i + 1 < corp_lon) {
            int inventum = 0;
            for (int k = 0; k < num_ops2; k++) {
                if (
                    corpus[i] == ops2[k][0] &&
                    corpus[i + 1] == ops2[k][1]
                ) {
                    /* evita '<<' vel '>>' nuda (non <<=/>>=) */
                    if (
                        (corpus[i] == '<' && corpus[i + 1] == '<') ||
                        (corpus[i] == '>' && corpus[i + 1] == '>')
                    ) {
                        /* solum <<= et >>= tractantur (iam supra) */
                        break;
                    }
                    if (
                        wp > initium_wp &&
                        *(wp - 1) != ' ' && *(wp - 1) != '\t'
                    )
                        *wp++ = ' ';
                    *wp++ = corpus[i];
                    *wp++ = corpus[i + 1];
                    if (
                        i + 2 < corp_lon &&
                        corpus[i + 2] != ' ' &&
                        corpus[i + 2] != '\t'
                    )
                        *wp++ = ' ';
                    i++;
                    inventum = 1;
                    break;
                }
            }
            if (inventum)
                continue;
        }

        /* '=' simplex (adtributio) — non pars compositi */
        if (c == '=') {
            /* confirma non pars compositi: praecedens non est op char */
            int compositus = 0;
            if (wp > initium_wp) {
                char p = *(wp - 1);
                /* si praecedens est spatium, inspice ante spatium */
                if (p == ' ' || p == '\t') {
                    /* iam spatiatum — non insere iterum */
                } else if (
                    p == '!' || p == '<' || p == '>' ||
                    p == '+' || p == '-' || p == '*' ||
                    p == '/' || p == '%' || p == '&' ||
                    p == '|' || p == '^' || p == '='
                ) {
                    compositus = 1;
                }
            }
            /* sequens '=' => '==' (iam tractatum supra) */
            if (i + 1 < corp_lon && corpus[i + 1] == '=')
                compositus = 1;

            if (!compositus) {
                if (
                    wp > initium_wp &&
                    *(wp - 1) != ' ' && *(wp - 1) != '\t'
                )
                    *wp++ = ' ';
                *wp++ = '=';
                if (
                    i + 1 < corp_lon &&
                    corpus[i + 1] != ' ' && corpus[i + 1] != '\t'
                )
                    *wp++ = ' ';
                continue;
            }
        }

        *wp++ = corpus[i];
    }
    *wp++ = '\n';
    return wp;
}
