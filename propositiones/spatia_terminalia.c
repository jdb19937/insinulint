/*
 * propositiones/spatia_terminalia.c — remove spatia terminalia
 */

#include "spatia_terminalia.h"

#include <string.h>

int propone_spatia_terminalia(
    const propositum_t *prop,
    prop_eventus_t *ev
) {
    if (prop->idx_centri < 0)
        return 0;
    const prop_linea_t *cl = &prop->fen[prop->idx_centri];
    const char *corpus     = cl->textus;
    int lon                = cl->lon;

    /* inveni finem sine spatiis */
    int nov = lon;
    while (
        nov > 0 &&
        (corpus[nov - 1] == ' ' || corpus[nov - 1] == '\t')
    )
        nov--;

    if (nov == lon)
        return 0;

    ev->idx_ab = prop->idx_centri;
    ev->idx_ad = prop->idx_centri;
    ev->num_novi = 1;
    ev->novi[0].numerus = cl->numerus;
    ev->novi[0].lon = nov;
    memcpy(ev->novi[0].textus, corpus, nov);
    ev->novi[0].textus[nov] = '\0';
    return 1;
}
