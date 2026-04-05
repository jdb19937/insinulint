/*
 * propositiones/bracchia_stilus.c — propone K&R vel Allman stilum
 *
 * Monitum semper in linea '{' est.
 *
 * bra_stilus == 0 (K&R): '{' in linea propria — iunge cum linea praecedenti.
 *   Centrum = linea '{'. Linea praecedens (idx_centri - 1) habet controlum.
 *
 * bra_stilus == 1 (Allman): '{' in eadem linea cum controlo — scinde.
 *   Centrum = linea cum controlo et '{'.
 *   bra_columna indicat columnam '{'.
 */

#include "bracchia_stilus.h"

#include <string.h>

/* computa indentationem (spatia) */
static int computa_ind(const char *s, int lon)
{
    int col = 0;
    for (int i = 0; i < lon; i++) {
        if (s[i] == ' ')
            col++;
        else if (s[i] == '\t')
            col += 8 - (col % 8);
        else
            break;
    }
    return col;
}

int propone_bracchia_stilum(
    const propositum_t *prop,
    prop_eventus_t *ev
) {
    if (prop->idx_centri < 0 || prop->bra_columna < 0)
        return 0;

    const prop_linea_t *cl = &prop->fen[prop->idx_centri];

    if (prop->bra_stilus == 0) {
        /* K&R: centrum est linea '{'. Linea praecedens habet controlum. */
        int praec = prop->idx_centri - 1;
        if (praec < 0)
            return 0;
        const prop_linea_t *ante_l = &prop->fen[praec];

        /* verifica '{' in linea centri */
        int k = 0;
        while (
            k < cl->lon &&
            (cl->textus[k] == ' ' || cl->textus[k] == '\t')
        )
            k++;
        if (k >= cl->lon || cl->textus[k] != '{')
            return 0;

        /* pars ante: praecedens sine spatiis terminalibus */
        int ante_lon = ante_l->lon;
        while (
            ante_lon > 0 &&
            (ante_l->textus[ante_lon - 1] == ' ' ||
             ante_l->textus[ante_lon - 1] == '\t')
        )
            ante_lon--;

        char dest[PROP_LINEA_MAX];
        int dp = 0;
        if (ante_lon + 3 >= PROP_LINEA_MAX)
            return 0;
        memcpy(dest, ante_l->textus, ante_lon);
        dp = ante_lon;
        dest[dp++] = ' ';
        dest[dp++] = '{';

        /* si restat contentum post '{', adde */
        k++;
        while (
            k < cl->lon &&
            (cl->textus[k] == ' ' || cl->textus[k] == '\t')
        )
            k++;
        if (k < cl->lon) {
            int rest = cl->lon - k;
            if (dp + 1 + rest >= PROP_LINEA_MAX)
                return 0;
            dest[dp++] = ' ';
            memcpy(dest + dp, cl->textus + k, rest);
            dp += rest;
        }
        dest[dp] = '\0';

        ev->idx_ab = praec;
        ev->idx_ad = prop->idx_centri;
        ev->num_novi = 1;
        ev->novi[0].numerus = ante_l->numerus;
        ev->novi[0].lon = dp;
        memcpy(ev->novi[0].textus, dest, dp + 1);
        return 1;
    }

    /* Allman: '{' in eadem linea — scinde ad novam lineam */
    int bra_byte = -1;
    int cur_col  = 0;
    for (int i = 0; i < cl->lon; i++) {
        if (cur_col >= prop->bra_columna && cl->textus[i] == '{') {
            bra_byte = i;
            break;
        }
        if (cl->textus[i] == '\t')
            cur_col += 8 - (cur_col % 8);
        else
            cur_col++;
    }
    if (bra_byte < 0)
        return 0;

    /* pars ante '{' sine spatiis terminalibus */
    int ante = bra_byte;
    while (
        ante > 0 &&
        (cl->textus[ante - 1] == ' ' || cl->textus[ante - 1] == '\t')
    )
        ante--;

    int ind = computa_ind(cl->textus, cl->lon);

    ev->idx_ab = prop->idx_centri;
    ev->idx_ad = prop->idx_centri;
    ev->num_novi = 2;

    ev->novi[0].numerus = cl->numerus;
    ev->novi[0].lon = ante;
    memcpy(ev->novi[0].textus, cl->textus, ante);
    ev->novi[0].textus[ante] = '\0';

    /* linea 2: '{' cum indentatione */
    char dest2[PROP_LINEA_MAX];
    int dp2 = 0;
    if (prop->ind_tabulis) {
        int tabs = ind / 8;
        int rest = ind % 8;
        for (int i = 0; i < tabs; i++)
            dest2[dp2++] = '\t';
        for (int i = 0; i < rest; i++)
            dest2[dp2++] = ' ';
    } else {
        for (int i = 0; i < ind; i++)
            dest2[dp2++] = ' ';
    }
    dest2[dp2++] = '{';
    dest2[dp2]   = '\0';

    ev->novi[1].numerus = cl->numerus + 1;
    ev->novi[1].lon = dp2;
    memcpy(ev->novi[1].textus, dest2, dp2 + 1);

    return 1;
}
