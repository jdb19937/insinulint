/*
 * propositiones/cubitus.c — iunge '}' cum 'else'
 *
 * Monitum est in linea 'else'.
 *
 * bra_else_coniunctum (defaltum):
 *   Centrum = linea 'else'. Linea praecedens (idx_centri - 1) habet '}'.
 *   Propositio: iunge "}\nelse" → "} else"
 *
 * Non coniunctum: monitum est in linea cum "} else" in eadem linea.
 *   Propositio: scinde in duas lineas. (non implementatum adhuc)
 */

#include "cubitus.h"

#include <string.h>

int propone_cubitum(
    const propositum_t *prop,
    prop_eventus_t *ev
) {
    if (prop->idx_centri < 0)
        return 0;

    /* casus coniuncti: iunge lineas }  +  else */
    int praec = prop->idx_centri - 1;
    if (praec < 0)
        return 0;

    const prop_linea_t *cl_bra  = &prop->fen[praec];
    const prop_linea_t *cl_else = &prop->fen[prop->idx_centri];

    /* linea praecedens: '}' sine spatiis terminalibus */
    int ante = cl_bra->lon;
    while (
        ante > 0 &&
        (cl_bra->textus[ante - 1] == ' ' || cl_bra->textus[ante - 1] == '\t')
    )
        ante--;

    /* linea else: transili spatia, inveni contentum */
    int k = 0;
    while (
        k < cl_else->lon &&
        (cl_else->textus[k] == ' ' || cl_else->textus[k] == '\t')
    )
        k++;
    int rest_lon = cl_else->lon - k;
    while (
        rest_lon > 0 &&
        (cl_else->textus[k + rest_lon - 1] == ' ' ||
         cl_else->textus[k + rest_lon - 1] == '\t')
    )
        rest_lon--;

    if (rest_lon <= 0)
        return 0;

    char dest[PROP_LINEA_MAX];
    int dp = 0;
    if (ante + 1 + rest_lon >= PROP_LINEA_MAX)
        return 0;
    memcpy(dest, cl_bra->textus, ante);
    dp = ante;
    dest[dp++] = ' ';
    memcpy(dest + dp, cl_else->textus + k, rest_lon);
    dp += rest_lon;
    dest[dp] = '\0';

    ev->idx_ab = praec;
    ev->idx_ad = prop->idx_centri;
    ev->num_novi = 1;
    ev->novi[0].numerus = cl_bra->numerus;
    ev->novi[0].lon = dp;
    memcpy(ev->novi[0].textus, dest, dp + 1);
    return 1;
}
