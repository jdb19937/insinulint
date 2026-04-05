/*
 * propositiones/una_sententia.c — scinde plures sententias in lineas proprias
 *
 * Reutit corrige_unam_sententiam ex correctiones/una_sententia.c.
 * fix_valor continet indentationem basalem.
 */

#include "una_sententia.h"
#include "../correctiones/una_sententia.h"
#include "../correctiones/communes.h"

#include <string.h>

int propone_unam_sententiam(
    const propositum_t *prop,
    prop_eventus_t *ev
) {
    if (prop->idx_centri < 0)
        return 0;

    const prop_linea_t *cl = &prop->fen[prop->idx_centri];
    const char *corpus     = cl->textus;
    int corp_lon           = cl->lon;

    /* computa indentationem */
    int ind = 0;
    for (int i = 0; i < corp_lon; i++) {
        if (corpus[i] == ' ')
            ind++;
        else if (corpus[i] == '\t')
            ind += 8 - (ind % 8);
        else
            break;
    }

    /* transili spatia initialia */
    int sp_init = 0;
    while (
        sp_init < corp_lon &&
        (corpus[sp_init] == ' ' || corpus[sp_init] == '\t')
    )
        sp_init++;

    if (sp_init >= corp_lon)
        return 0;

    /* aedifica speculum temporarium */
    speculum_t spec_tmp;
    memset(&spec_tmp, 0, sizeof(spec_tmp));
    spec_tmp.ind_latitudo = prop->ind_latitudo;
    spec_tmp.ind_tabulis  = prop->ind_tabulis;

    char tmp[PROP_LINEA_MAX * 8];
    char *wp = corrige_unam_sententiam(
        tmp, corpus + sp_init, corp_lon - sp_init,
        ind, &spec_tmp
    );
    int tot = (int)(wp - tmp);

    /* remove '\n' terminalem ultimam */
    while (tot > 0 && tmp[tot - 1] == '\n')
        tot--;

    if (tot == corp_lon - sp_init && memcmp(tmp, corpus + sp_init, tot) == 0)
        return 0;

    /* parse lineas ex eventu */
    ev->idx_ab   = prop->idx_centri;
    ev->idx_ad   = prop->idx_centri;
    ev->num_novi = 0;

    const char *p = tmp;
    const char *fin = tmp + tot;
    int lin_num = cl->numerus;

    while (p < fin && ev->num_novi < PROP_NOVI_MAX) {
        const char *nl = p;
        while (nl < fin && *nl != '\n')
            nl++;
        int llon = (int)(nl - p);
        if (llon >= PROP_LINEA_MAX)
            llon = PROP_LINEA_MAX - 1;

        /* adde indentationem initialem si correctio non iam addidit */
        ev->novi[ev->num_novi].numerus = lin_num++;
        ev->novi[ev->num_novi].lon = llon;
        memcpy(ev->novi[ev->num_novi].textus, p, llon);
        ev->novi[ev->num_novi].textus[llon] = '\0';
        ev->num_novi++;

        p = (nl < fin) ? nl + 1 : nl;
    }

    return (ev->num_novi > 0) ? 1 : 0;
}
