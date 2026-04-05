/*
 * propositiones/longitudo.c — scinde lineam longam ad operatorem
 *
 * Reutit corrige_longitudinem ex correctiones/longitudo.c.
 * fix_valor continet longitudinem maximam.
 * ind_latitudo et ind_tabulis pro continuatione adhibentur.
 */

#include "longitudo.h"
#include "../correctiones/longitudo.h"
#include "../correctiones/communes.h"

#include <string.h>

int propone_longitudinem(
    const propositum_t *prop,
    prop_eventus_t *ev
) {
    if (prop->idx_centri < 0)
        return 0;
    int maxima = prop->lin_longitudo_max;
    if (maxima <= 0)
        maxima = prop->fix_valor;
    if (maxima <= 0)
        return 0;

    const prop_linea_t *cl = &prop->fen[prop->idx_centri];
    const char *corpus     = cl->textus;
    int corp_lon           = cl->lon;

    /* computa spatia initialia */
    int sp_init = 0;
    while (
        sp_init < corp_lon &&
        (corpus[sp_init] == ' ' || corpus[sp_init] == '\t')
    )
        sp_init++;

    /* continuatio: indentatio + unus gradus */
    int lat = prop->ind_latitudo;
    if (lat <= 0)
        lat = 4;
    int cont_ind = sp_init + lat;

    /* aedifica speculum temporarium pro scribe_indentationem */
    speculum_t spec_tmp;
    memset(&spec_tmp, 0, sizeof(spec_tmp));
    spec_tmp.ind_latitudo = prop->ind_latitudo;
    spec_tmp.ind_tabulis  = prop->ind_tabulis;

    char tmp[PROP_LINEA_MAX * 4];
    char *wp = corrige_longitudinem(
        tmp, corpus, corp_lon,
        sp_init, cont_ind, maxima, &spec_tmp
    );
    int tot = (int)(wp - tmp);

    /* remove '\n' terminalem */
    while (tot > 0 && tmp[tot - 1] == '\n')
        tot--;

    if (tot == corp_lon && memcmp(tmp, corpus, corp_lon) == 0)
        return 0;

    /* parse lineas ex eventum (separatae per '\n') */
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

        ev->novi[ev->num_novi].numerus = lin_num++;
        ev->novi[ev->num_novi].lon = llon;
        memcpy(ev->novi[ev->num_novi].textus, p, llon);
        ev->novi[ev->num_novi].textus[llon] = '\0';
        ev->num_novi++;

        p = (nl < fin) ? nl + 1 : nl;
    }

    return (ev->num_novi > 0) ? 1 : 0;
}
