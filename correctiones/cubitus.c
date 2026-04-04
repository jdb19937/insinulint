/*
 * correctiones/cubitus.c — correctio cubiti (else coniuncti)
 *
 * Iunge '}' cum 'else' in linea sequente: "}\nelse" → "} else"
 */

#include "../correctio.h"

#include <string.h>

char *corrige_cubitum(
    char *wp, const char *corpus, int corp_lon,
    const linea_t *lineae, int i, int nlin,
    int *transili
) {
    /* scribe contentum lineae currentis sine spatiis terminalibus */
    int lon = corp_lon;
    while (
        lon > 0 &&
        (corpus[lon - 1] == ' ' || corpus[lon - 1] == '\t')
    )
        lon--;
    memcpy(wp, corpus, lon);
    wp += lon;

    /* adde spatium et contentum lineae sequentis (ab 'else') */
    if (i + 1 < nlin) {
        const linea_t *ln = &lineae[i + 1];
        int k = 0;
        while (
            k < ln->lon &&
            (ln->initium[k] == ' ' || ln->initium[k] == '\t')
        )
            k++;
        int rest_lon = ln->lon - k;
        while (
            rest_lon > 0 &&
            (
                ln->initium[k + rest_lon - 1] == ' ' ||
                ln->initium[k + rest_lon - 1] == '\t'
            )
        )
            rest_lon--;
        if (rest_lon > 0) {
            *wp++ = ' ';
            memcpy(wp, ln->initium + k, rest_lon);
            wp += rest_lon;
        }
        *transili = 1;
    }

    *wp++ = '\n';
    return wp;
}
