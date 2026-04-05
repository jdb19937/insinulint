/*
 * propositiones/spatium_operator.c — propositio operatorum
 */

#include "spatium_operator.h"
#include "../correctiones/operatores.h"

#include <string.h>

int propone_spatium_operator(
    const propositum_t *prop,
    prop_eventus_t *ev
) {
    if (prop->idx_centri < 0)
        return 0;
    const prop_linea_t *cl = &prop->fen[prop->idx_centri];
    const char *corpus     = cl->textus;
    int corp_lon           = cl->lon;

    int sp_init = 0;
    while (
        sp_init < corp_lon &&
        (corpus[sp_init] == ' ' || corpus[sp_init] == '\t')
    )
        sp_init++;

    char tmp[PROP_LINEA_MAX * 2];
    char *wp = corrige_operatores(
        tmp, corpus + sp_init, corp_lon - sp_init
    );
    int clon = (int)(wp - tmp) - 1;

    int tot = sp_init + clon;
    if (tot >= PROP_LINEA_MAX)
        return 0;

    char dest[PROP_LINEA_MAX];
    memcpy(dest, corpus, sp_init);
    memcpy(dest + sp_init, tmp, clon);
    dest[tot] = '\0';

    if (tot == corp_lon && memcmp(dest, corpus, corp_lon) == 0)
        return 0;

    ev->idx_ab = prop->idx_centri;
    ev->idx_ad = prop->idx_centri;
    ev->num_novi = 1;
    ev->novi[0].numerus = cl->numerus;
    ev->novi[0].lon = tot;
    memcpy(ev->novi[0].textus, dest, tot);
    ev->novi[0].textus[tot] = '\0';
    return 1;
}
