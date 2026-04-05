/*
 * propositiones/spatium_semicolon.c — propositio semicolonorum
 */

#include "spatium_semicolon.h"
#include "../correctiones/semicolon.h"

#include <string.h>

int propone_spatium_semicolon(
    const prop_linea_t *centrum,
    char *dest, int dest_max
) {
    const char *corpus = centrum->textus;
    int corp_lon       = centrum->lon;

    int sp_init = 0;
    while (
        sp_init < corp_lon &&
        (corpus[sp_init] == ' ' || corpus[sp_init] == '\t')
    )
        sp_init++;

    char tmp[PROP_LINEA_MAX * 2];
    char *wp = corrige_spatium_semicolon(
        tmp, corpus + sp_init, corp_lon - sp_init
    );
    int clon = (int)(wp - tmp) - 1;

    int tot = sp_init + clon;
    if (tot >= dest_max)
        return -1;
    memcpy(dest, corpus, sp_init);
    memcpy(dest + sp_init, tmp, clon);
    dest[tot] = '\0';

    if (tot == corp_lon && memcmp(dest, corpus, corp_lon) == 0)
        return -1;
    return tot;
}
