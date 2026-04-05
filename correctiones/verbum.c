/*
 * correctiones/verbum.c — correctio spatiorum post verba clavis
 *
 * Inserit spatium post verba clavis (if, for, while, switch, return, case)
 * ubi sequitur '(' vel (pro return) expressio sine spatio.
 */

#include "../correctio.h"
#include "verbum.h"

#include <string.h>

/* verba clavis quae spatium ante '(' requirunt */
static const char *verba[] = {
    "if", "for", "while", "switch", "return", "case", "do"
};
static const int num_verba = 7;

/*
 * est_verbum_hic — 1 si corpus[pos..] incipit verbo et character
 * post verbum non est alphanumericus (ergo verbum integrum est).
 */
static int est_verbum_hic(
    const char *corpus, int corp_lon, int pos,
    const char *verbum, int verb_lon
) {
    if (pos + verb_lon > corp_lon)
        return 0;
    if (memcmp(corpus + pos, verbum, (size_t)verb_lon) != 0)
        return 0;
    /* confirma finem verbi — sequens character non alphanumericum */
    if (pos + verb_lon < corp_lon) {
        char c = corpus[pos + verb_lon];
        if (
            (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '_'
        )
            return 0;
    }
    /* confirma initium verbi — praecedens non alphanumericum */
    if (pos > 0) {
        char p = corpus[pos - 1];
        if (
            (p >= 'a' && p <= 'z') || (p >= 'A' && p <= 'Z') ||
            (p >= '0' && p <= '9') || p == '_'
        )
            return 0;
    }
    return 1;
}

char *corrige_spatium_verbum(
    char *wp, const char *corpus, int corp_lon
) {
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

        /* transili commentaria */
        if (c == '/' && i + 1 < corp_lon) {
            if (corpus[i + 1] == '/') {
                /* '//' — copia usque ad finem */
                memcpy(wp, corpus + i, corp_lon - i);
                wp += corp_lon - i;
                i = corp_lon;
                break;
            }
            if (corpus[i + 1] == '*') {
                *wp++ = corpus[i];
                continue;
            }
        }

        /* inspice verba clavis */
        int inventum = 0;
        for (int v = 0; v < num_verba; v++) {
            int vlon = (int)strlen(verba[v]);
            if (est_verbum_hic(corpus, corp_lon, i, verba[v], vlon)) {
                /* copia verbum */
                memcpy(wp, corpus + i, vlon);
                wp += vlon;
                i += vlon;
                /* si sequitur '(' vel (pro return) non-spatium,
                 * adde spatium */
                if (i < corp_lon && corpus[i] != ' ' && corpus[i] != '\t') {
                    *wp++ = ' ';
                }
                i--; /* compensatio pro i++ in for */
                inventum = 1;
                break;
            }
        }
        if (inventum)
            continue;

        *wp++ = corpus[i];
    }
    *wp++ = '\n';
    return wp;
}
