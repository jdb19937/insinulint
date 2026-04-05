/*
 * propositiones/colineatio.c — colinea '=' per spatia
 *
 * fix_valor continet delta spatiorum (positivum = adde, negativum = remove).
 * columna indicat columnam '=' in linea.
 */

#include "colineatio.h"
#include "../correctiones/colineatio.h"

#include <string.h>

int propone_colineationem(
    const propositum_t *prop,
    prop_eventus_t *ev
) {
    if (prop->idx_centri < 0)
        return 0;
    const prop_linea_t *cl = &prop->fen[prop->idx_centri];
    const char *corpus     = cl->textus;
    int corp_lon           = cl->lon;

    /* computa sp_init (spatia initialia) */
    int sp_init = 0;
    while (
        sp_init < corp_lon &&
        (corpus[sp_init] == ' ' || corpus[sp_init] == '\t')
    )
        sp_init++;

    char tmp[PROP_LINEA_MAX * 2];
    char *wp = corrige_colineationem(
        tmp, corpus, corp_lon,
        prop->columna, prop->fix_valor, sp_init
    );
    int tot = (int)(wp - tmp) - 1; /* sine '\n' */

    if (tot <= 0 || tot >= PROP_LINEA_MAX)
        return 0;
    if (tot == corp_lon && memcmp(tmp, corpus, corp_lon) == 0)
        return 0;

    ev->idx_ab = prop->idx_centri;
    ev->idx_ad = prop->idx_centri;
    ev->num_novi = 1;
    ev->novi[0].numerus = cl->numerus;
    ev->novi[0].lon = tot;
    memcpy(ev->novi[0].textus, tmp, tot);
    ev->novi[0].textus[tot] = '\0';
    return 1;
}
