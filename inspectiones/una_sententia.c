/*
 * inspectiones/una_sententia.c — una sententia per lineam
 *
 * Flagellat lineas cum pluribus sententiis (plura ';' extra parentheses).
 */

#include "../inspectio.h"
#include "una_sententia.h"

#include <stdio.h>

/*
 * est_scheda_primum — discerne si linea incipit cum labello
 * (case X, default, foo:). Si sic, reddit indicem signi ':' finalis
 * labelli; aliter -1.
 */
static int est_scheda_primum(
    const signum_t *signa, const versus_t *v
) {
    int primus = v->tok_primus;
    int est_casus = (
        est_verbum(&signa[primus], "case") ||
        est_verbum(&signa[primus], "default")
    );

    if (!est_casus) {
        /* labellum generale: VERBUM proximum sequitur ':' */
        if (signa[primus].genus != SIGNUM_VERBUM)
            return -1;
        for (int j = primus + 1; j < v->tok_finis; j++) {
            if (
                signa[j].genus == SIGNUM_SPATIUM ||
                signa[j].genus == SIGNUM_COMMENTARIUM
            )
                continue;
            if (
                signa[j].genus == SIGNUM_OPERATOR &&
                signa[j].longitudo == 1 &&
                signa[j].initium[0] == ':'
            )
                return j;
            return -1;
        }
        return -1;
    }

    /* case/default: quaere primum ':' ad profunditatem 0 */
    int prof_par = 0;
    for (int j = primus + 1; j < v->tok_finis; j++) {
        if (
            signa[j].genus == SIGNUM_APERTIO_PAR ||
            signa[j].genus == SIGNUM_APERTIO_QUAD
        )
            prof_par++;
        else if (
            signa[j].genus == SIGNUM_CLAUSIO_PAR ||
            signa[j].genus == SIGNUM_CLAUSIO_QUAD
        ) {
            if (prof_par > 0)
                prof_par--;
        } else if (
            prof_par == 0 &&
            signa[j].genus == SIGNUM_OPERATOR &&
            signa[j].longitudo == 1 &&
            signa[j].initium[0] == ':'
        )
            return j;
    }
    return -1;
}

void inspice_unam_sententiam(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus,
    const speculum_t *spec
) {
    const signum_t *signa = lex->signa;

    for (int vi = 0; vi < num_versus; vi++) {
        const versus_t *v = &versus[vi];
        if (versus_vacuus(v))
            continue;

        /* transili praeprocessorem */
        if (signa[v->tok_primus].genus == SIGNUM_PRAEPROCESSOR)
            continue;

        int scheda_idx = spec->lin_schedae_propria
            ? est_scheda_primum(signa, v) : -1;

        /* numera semicolona (et ':' labelli si adest) extra parentheses */
        int prof_par     = 0;
        int boundariae   = 0;
        int primum_extra = -1;
        int limen        = 1;

        for (int j = v->tok_initium; j < v->tok_finis; j++) {
            if (
                signa[j].genus == SIGNUM_APERTIO_PAR ||
                signa[j].genus == SIGNUM_APERTIO_QUAD
            )
                prof_par++;
            else if (
                signa[j].genus == SIGNUM_CLAUSIO_PAR ||
                signa[j].genus == SIGNUM_CLAUSIO_QUAD
            ) {
                if (prof_par > 0)
                    prof_par--;
            } else if (
                signa[j].genus == SIGNUM_SEMICOLON && prof_par == 0
            ) {
                boundariae++;
                if (boundariae == limen + 1 && primum_extra < 0)
                    primum_extra = j;
            } else if (
                j == scheda_idx && prof_par == 0
            ) {
                /* labellum ':' ipsum conputatur ut terminus,
                 * sed si aliquid sequitur (ad tok_finis) est violatio */
                int restat = 0;
                for (int k = j + 1; k < v->tok_finis; k++) {
                    if (
                        signa[k].genus != SIGNUM_SPATIUM &&
                        signa[k].genus != SIGNUM_COMMENTARIUM
                    ) {
                        restat = 1;
                        break;
                    }
                }
                if (restat) {
                    boundariae++;
                    if (primum_extra < 0)
                        primum_extra = j;
                }
            }
        }

        if (boundariae > limen && primum_extra >= 0) {
            char nuntius[NUNTIUS_MAX];
            snprintf(
                nuntius, sizeof(nuntius),
                "%d sententiae in eadem linea (una expectata)",
                boundariae
            );
            adde_fix(
                ins, GRAVITAS_MONITUM,
                v->numero, signa[primum_extra].columna,
                "una_sententia", nuntius,
                v->spatia
            );
        }
    }
}
