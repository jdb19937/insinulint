/*
 * propositiones/indentatio.c — reindenta lineam ad valorem expectatum
 *
 * fix_valor continet numerum spatiorum expectatum.
 * ind_tabulis determinat an tabulae an spatia adhibendae sint.
 */

#include "indentatio.h"

#include <string.h>

int propone_indentationem(
    const propositum_t *prop,
    prop_eventus_t *ev
) {
    if (prop->idx_centri < 0 || prop->fix_valor < 0)
        return 0;
    const prop_linea_t *cl = &prop->fen[prop->idx_centri];
    const char *corpus     = cl->textus;
    int corp_lon           = cl->lon;

    /* inveni finem indentationis veteris */
    int vet_fin = 0;
    while (
        vet_fin < corp_lon &&
        (corpus[vet_fin] == ' ' || corpus[vet_fin] == '\t')
    )
        vet_fin++;

    /* aedifica indentationem novam */
    int expectata = prop->fix_valor;
    char ind[256];
    int ind_lon = 0;

    if (prop->ind_tabulis) {
        int tabs = expectata / 8;
        int rest = expectata % 8;
        for (int i = 0; i < tabs && ind_lon < 255; i++)
            ind[ind_lon++] = '\t';
        for (int i = 0; i < rest && ind_lon < 255; i++)
            ind[ind_lon++] = ' ';
    } else {
        for (int i = 0; i < expectata && ind_lon < 255; i++)
            ind[ind_lon++] = ' ';
    }

    int rest_lon = corp_lon - vet_fin;
    int tot      = ind_lon + rest_lon;
    if (tot >= PROP_LINEA_MAX)
        return 0;

    /* inspice an re vera mutatio sit */
    if (
        ind_lon == vet_fin &&
        memcmp(ind, corpus, ind_lon) == 0
    )
        return 0;

    ev->idx_ab = prop->idx_centri;
    ev->idx_ad = prop->idx_centri;
    ev->num_novi = 1;
    ev->novi[0].numerus = cl->numerus;
    memcpy(ev->novi[0].textus, ind, ind_lon);
    memcpy(ev->novi[0].textus + ind_lon, corpus + vet_fin, rest_lon);
    ev->novi[0].lon         = tot;
    ev->novi[0].textus[tot] = '\0';
    return 1;
}
