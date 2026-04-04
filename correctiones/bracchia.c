/*
 * correctiones/bracchia.c — correctio stili bracchiorum
 *
 * K&R: iunge '{' ex linea nova ad finem lineae praecedentis.
 * Allman: scinde '{' ex eadem linea ad lineam novam.
 */

#include "../correctio.h"

#include <string.h>

/*
 * corrige_bracchia_kr — iunge '{' ad finem lineae currentis
 *
 * Linea currens (i) est linea ante '{'. Linea i+1 continet '{' solam.
 * Scribit contentum lineae currentis, addit ' {', transit lineam i+1.
 */
char *corrige_bracchia_kr(
    char *wp, const char *corpus, int corp_lon,
    const linea_t *lineae, int i, int nlin,
    int *transili, const speculum_t *spec
) {
    (void)spec;

    /* scribe contentum lineae currentis sine spatiis terminalibus */
    int lon = corp_lon;
    while (
        lon > 0 &&
        (corpus[lon - 1] == ' ' || corpus[lon - 1] == '\t')
    )
        lon--;
    memcpy(wp, corpus, lon);
    wp += lon;

    /* adde ' {' */
    *wp++ = ' ';
    *wp++ = '{';
    *wp++ = '\n';

    /* emitte contentum post '{' in linea proxima */
    if (i + 1 < nlin) {
        const linea_t *ln = &lineae[i + 1];
        int k = 0;
        while (
            k < ln->lon &&
            (ln->initium[k] == ' ' || ln->initium[k] == '\t')
        )
            k++;
        /* transili '{' */
        if (k < ln->lon && ln->initium[k] == '{')
            k++;
        /* transili spatia post '{' */
        while (
            k < ln->lon &&
            (ln->initium[k] == ' ' || ln->initium[k] == '\t')
        )
            k++;
        /* si restat contentum post '{', scribe in nova linea */
        if (k < ln->lon) {
            /* computa indentationem lineae currentis */
            int ind_cur = 0;
            for (int j = 0; j < lon; j++) {
                /* iam habemus corpus post indentationem — non possumus;
                 * utimur indentationem originalem lineae praecedentis */
                (void)j;
                break;
            }
            /* indentatio lineae praecedentis + unus gradus */
            const linea_t *lc = &lineae[i];
            for (int j = 0; j < lc->lon; j++) {
                if (lc->initium[j] == ' ')
                    ind_cur++;
                else if (lc->initium[j] == '\t')
                    ind_cur += 8;
                else
                    break;
            }
            int lat = spec->ind_latitudo;
            if (spec->ind_tabulis)
                lat = spec->ind_latitudo * 8;
            if (lat <= 0)
                lat = 4;
            wp = scribe_indentationem(wp, ind_cur + lat, spec);
            int rest_lon = ln->lon - k;
            while (
                rest_lon > 0 &&
                (ln->initium[k + rest_lon - 1] == ' ' ||
                 ln->initium[k + rest_lon - 1] == '\t')
            )
                rest_lon--;
            memcpy(wp, ln->initium + k, rest_lon);
            wp += rest_lon;
            *wp++ = '\n';
        }
        *transili = 1;
    }

    return wp;
}

/*
 * corrige_bracchia_allman — scinde '{' ad lineam novam
 *
 * Linea currens continet '...) {' vel '...else {'.
 * Scindimus ante '{': contentum ante + '\n' + indentatio + '{' + '\n'.
 */
char *corrige_bracchia_allman(
    char *wp, const char *corpus, int corp_lon,
    int col, int ind, const speculum_t *spec
) {
    /* inveni '{' in corpore */
    int bra_byte = -1;
    int cur_col = 0;
    for (int j = 0; j < corp_lon; j++) {
        if (cur_col >= col && corpus[j] == '{') {
            bra_byte = j;
            break;
        }
        if (corpus[j] == '\t')
            cur_col += 8 - (cur_col % 8);
        else
            cur_col++;
    }
    if (bra_byte < 0) {
        memcpy(wp, corpus, corp_lon);
        wp += corp_lon;
        *wp++ = '\n';
        return wp;
    }

    /* pars ante '{' — sine spatiis terminalibus */
    int ante = bra_byte;
    while (
        ante > 0 &&
        (corpus[ante - 1] == ' ' || corpus[ante - 1] == '\t')
    )
        ante--;
    memcpy(wp, corpus, ante);
    wp += ante;
    *wp++ = '\n';

    /* '{' cum indentione in nova linea */
    wp = scribe_indentationem(wp, ind, spec);
    *wp++ = '{';

    /* contentum post '{' */
    int post = bra_byte + 1;
    while (
        post < corp_lon &&
        (corpus[post] == ' ' || corpus[post] == '\t')
    )
        post++;
    if (post < corp_lon) {
        /* restat contentum — scribe in eadem linea post '{' */
        *wp++ = '\n';
        int rest_lon = corp_lon - post;
        while (
            rest_lon > 0 &&
            (corpus[post + rest_lon - 1] == ' ' ||
             corpus[post + rest_lon - 1] == '\t')
        )
            rest_lon--;
        wp = scribe_indentationem(wp, ind + (spec->ind_latitudo > 0 ? spec->ind_latitudo : 4), spec);
        memcpy(wp, corpus + post, rest_lon);
        wp += rest_lon;
    }
    *wp++ = '\n';

    return wp;
}
