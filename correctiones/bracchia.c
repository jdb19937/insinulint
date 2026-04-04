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
 * Linea currens (i) est linea ante '{'. Linea i+1 continet '{'.
 * Scribit contentum lineae currentis, addit ' {', transit lineam i+1.
 * Si contentum post '{' adest et una_ind_prox >= 0, scindit sententias.
 */
char *corrige_bracchia_kr(
    char *wp, const char *corpus, int corp_lon,
    const linea_t *lineae, int i, int nlin,
    int *transili, int una_ind_prox, const speculum_t *spec
) {
    /* scribe contentum lineae currentis sine spatiis terminalibus */
    int lon = corp_lon;
    while (
        lon > 0 &&
        (corpus[lon - 1] == ' ' || corpus[lon - 1] == '\t')
    )
        lon--;

    /* inveni '//' commentarium (extra chordas) */
    int comm = -1;
    for (int j = 0; j < lon; j++) {
        if (corpus[j] == '"' || corpus[j] == '\'') {
            char d = corpus[j];
            for (j++; j < lon; j++) {
                if (corpus[j] == '\\' && j + 1 < lon)
                    j++;
                else if (corpus[j] == d)
                    break;
            }
            continue;
        }
        if (
            corpus[j] == '/' && j + 1 < lon &&
            corpus[j + 1] == '/'
        ) {
            comm = j;
            break;
        }
    }

    if (comm >= 0) {
        /* insere '{' ante '//' commentarium */
        int ante = comm;
        while (
            ante > 0 &&
            (corpus[ante - 1] == ' ' || corpus[ante - 1] == '\t')
        )
            ante--;
        memcpy(wp, corpus, ante);
        wp += ante;
        *wp++ = ' ';
        *wp++ = '{';
        *wp++ = ' ';
        memcpy(wp, corpus + comm, lon - comm);
        wp += lon - comm;
    } else {
        memcpy(wp, corpus, lon);
        wp += lon;
        *wp++ = ' ';
        *wp++ = '{';
    }

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

        if (k < ln->lon) {
            /* restat contentum post '{' */
            int rest_lon = ln->lon - k;
            while (
                rest_lon > 0 &&
                (
                    ln->initium[k + rest_lon - 1] == ' ' ||
                    ln->initium[k + rest_lon - 1] == '\t'
                )
            )
                rest_lon--;

            /* computa indentationem: linea praecedentis + unus gradus */
            const linea_t *lc = &lineae[i];
            int ind_cur       = 0;
            for (int j = 0; j < lc->lon; j++) {
                if (lc->initium[j] == ' ')
                    ind_cur++;
                else if (lc->initium[j] == '\t')
                    ind_cur += 8;
                else
                    break;
            }
            int lat = spec->ind_tabulis
            ? spec->ind_latitudo * 8
                : spec->ind_latitudo;
            if (lat <= 0)
                lat = 4;
            int ind_rest = ind_cur + lat;

            *wp++ = '\n';
            if (una_ind_prox >= 0) {
                /* sententias scinde */
                wp = scribe_indentationem(wp, ind_rest, spec);
                wp = corrige_unam_sententiam(
                    wp, ln->initium + k, rest_lon,
                    ind_rest, spec
                );
                /* corrige_unam_sententiam iam '\n' addidit */
            } else {
                wp = scribe_indentationem(wp, ind_rest, spec);
                memcpy(wp, ln->initium + k, rest_lon);
                wp += rest_lon;
                *wp++ = '\n';
            }
        } else {
            *wp++ = '\n';
        }
        *transili = 1;
    } else {
        *wp++ = '\n';
    }

    return wp;
}

/*
 * corrige_bracchia_allman — scinde '{' ad lineam novam
 */
char *corrige_bracchia_allman(
    char *wp, const char *corpus, int corp_lon,
    int col, int ind, const speculum_t *spec
) {
    /* inveni '{' in corpore */
    int bra_byte = -1;
    int cur_col  = 0;
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
    wp    = scribe_indentationem(wp, ind, spec);
    *wp++ = '{';

    /* contentum post '{' */
    int post = bra_byte + 1;
    while (
        post < corp_lon &&
        (corpus[post] == ' ' || corpus[post] == '\t')
    )
        post++;
    if (post < corp_lon) {
        int lat = spec->ind_latitudo;
        if (lat <= 0)
            lat      = 4;
        *wp++        = '\n';
        int rest_lon = corp_lon - post;
        while (
            rest_lon > 0 &&
            (
                corpus[post + rest_lon - 1] == ' ' ||
                corpus[post + rest_lon - 1] == '\t'
            )
        )
            rest_lon--;
        wp = scribe_indentationem(wp, ind + lat, spec);
        memcpy(wp, corpus + post, rest_lon);
        wp += rest_lon;
    }
    *wp++ = '\n';

    return wp;
}
