/*
 * propositiones/spatia_terminalia.c — remove spatia terminalia
 */

#include "spatia_terminalia.h"

#include <string.h>

int propone_spatia_terminalia(
    const prop_linea_t *centrum,
    char *dest, int dest_max
) {
    const char *corpus = centrum->textus;
    int lon            = centrum->lon;

    /* inveni finem sine spatiis */
    int nov = lon;
    while (
        nov > 0 &&
        (corpus[nov - 1] == ' ' || corpus[nov - 1] == '\t')
    )
        nov--;

    if (nov == lon)
        return -1;
    if (nov >= dest_max)
        return -1;

    memcpy(dest, corpus, nov);
    dest[nov] = '\0';
    return nov;
}
