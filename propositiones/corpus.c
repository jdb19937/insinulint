/*
 * propositiones/corpus.c — scinde corpus ad lineam novam
 *
 * "if (x) y;" → "if (x)\n    y;"
 * columna indicat ubi corpus incipit.
 * fix_valor continet indentationem corporis.
 */

#include "corpus.h"

#include <string.h>

int propone_corpus(
    const propositum_t *prop,
    prop_eventus_t *ev
) {
    if (prop->idx_centri < 0 || prop->fix_valor < 0)
        return 0;

    const prop_linea_t *cl = &prop->fen[prop->idx_centri];
    const char *corpus     = cl->textus;
    int corp_lon           = cl->lon;
    int col                = prop->columna;

    /* inveni byte-offset ubi corpus incipit */
    int corp_byte = 0;
    int cur_col   = 0;
    for (int i = 0; i < corp_lon; i++) {
        if (cur_col >= col) {
            corp_byte = i;
            break;
        }
        if (corpus[i] == '\t')
            cur_col += 8 - (cur_col % 8);
        else
            cur_col++;
    }
    if (cur_col < col)
        return 0;

    /* pars 1: controlum sine spatiis terminalibus */
    int ante = corp_byte;
    while (
        ante > 0 &&
        (corpus[ante - 1] == ' ' || corpus[ante - 1] == '\t')
    )
        ante--;

    /* pars 2: corpus sine spatiis terminalibus */
    int rest_lon = corp_lon - corp_byte;
    while (
        rest_lon > 0 &&
        (corpus[corp_byte + rest_lon - 1] == ' ' ||
         corpus[corp_byte + rest_lon - 1] == '\t')
    )
        rest_lon--;

    if (rest_lon <= 0)
        return 0;

    /* linea 1: controlum */
    ev->idx_ab = prop->idx_centri;
    ev->idx_ad = prop->idx_centri;
    ev->num_novi = 2;

    ev->novi[0].numerus = cl->numerus;
    ev->novi[0].lon = ante;
    memcpy(ev->novi[0].textus, corpus, ante);
    ev->novi[0].textus[ante] = '\0';

    /* linea 2: corpus cum indentatione */
    int ind = prop->fix_valor;
    char dest2[PROP_LINEA_MAX];
    int dp2 = 0;
    if (prop->ind_tabulis) {
        int tabs = ind / 8;
        int rest = ind % 8;
        for (int i = 0; i < tabs && dp2 < PROP_LINEA_MAX - 1; i++)
            dest2[dp2++] = '\t';
        for (int i = 0; i < rest && dp2 < PROP_LINEA_MAX - 1; i++)
            dest2[dp2++] = ' ';
    } else {
        for (int i = 0; i < ind && dp2 < PROP_LINEA_MAX - 1; i++)
            dest2[dp2++] = ' ';
    }
    if (dp2 + rest_lon >= PROP_LINEA_MAX)
        return 0;
    memcpy(dest2 + dp2, corpus + corp_byte, rest_lon);
    dp2 += rest_lon;
    dest2[dp2] = '\0';

    ev->novi[1].numerus = cl->numerus + 1;
    ev->novi[1].lon = dp2;
    memcpy(ev->novi[1].textus, dest2, dp2 + 1);

    return 1;
}
