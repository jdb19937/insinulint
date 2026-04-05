/*
 * propositiones/commentaria.c — dele commentarium ex linea centri
 *
 * Columna moniti indicat initium commentarii.
 * Si linea tota commentarium est, lineam deletam proponit (num_novi = 0).
 * Si commentarium in fine lineae est, partem ante commentarium retinet.
 */

#include "commentaria.h"

#include <string.h>

int propone_commentaria(
    const propositum_t *prop,
    prop_eventus_t *ev
) {
    if (prop->idx_centri < 0)
        return 0;
    const prop_linea_t *cl = &prop->fen[prop->idx_centri];
    const char *corpus     = cl->textus;
    int corp_lon           = cl->lon;
    int col                = prop->columna;

    /* inveni byte-offset commentarii */
    int com_byte = 0;
    int cur_col  = 0;
    for (int i = 0; i < corp_lon; i++) {
        if (cur_col >= col) {
            com_byte = i;
            break;
        }
        if (corpus[i] == '\t')
            cur_col += 8 - (cur_col % 8);
        else
            cur_col++;
    }
    if (cur_col < col)
        com_byte = corp_lon;

    /* remove spatia ante commentarium */
    int ante = com_byte;
    while (
        ante > 0 &&
        (corpus[ante - 1] == ' ' || corpus[ante - 1] == '\t')
    )
        ante--;

    /* si nihil restat, propone deletionem */
    if (ante == 0) {
        ev->idx_ab   = prop->idx_centri;
        ev->idx_ad   = prop->idx_centri;
        ev->num_novi = 0;
        return 1;
    }

    /* trunca lineam ante commentarium */
    ev->idx_ab = prop->idx_centri;
    ev->idx_ad = prop->idx_centri;
    ev->num_novi = 1;
    ev->novi[0].numerus = cl->numerus;
    ev->novi[0].lon = ante;
    memcpy(ev->novi[0].textus, corpus, ante);
    ev->novi[0].textus[ante] = '\0';
    return 1;
}
