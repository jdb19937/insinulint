/*
 * inspectiones/bracchia_necessaria.c — bracchia post if/for/while/else
 */

#include "../inspectio.h"

#include <stdio.h>

void inspice_bracchia_necessaria(
    inspector_t *ins,
    const lexator_t *lex
) {
    int n = lex->num_signa;
    const signum_t *signa = lex->signa;

    for (int i = 0; i < n; i++) {
        int necessitat_bracchia = 0;
        int post_idx = -1;

        /* if (...), for (...), while (...), switch (...) */
        if (
            est_verbum(&signa[i], "if") ||
            est_verbum(&signa[i], "for") ||
            est_verbum(&signa[i], "while") ||
            est_verbum(&signa[i], "switch")
        ) {

            /* quaere '(' */
            int par_ap = prox_significans(signa, n, i + 1);
            if (par_ap >= n || signa[par_ap].genus != SIGNUM_APERTIO_PAR)
                continue;

            /* quaere ')' congruam */
            int par_cl = quaere_par_clausam(signa, n, par_ap);
            if (par_cl < 0)
                continue;

            /* while post do-while — excludimus */
            if (est_verbum(&signa[i], "while")) {
                int ante = prox_significans_retro(signa, i - 1);
                if (ante >= 0 && signa[ante].genus == SIGNUM_CLAUSIO)
                    continue;
            }

            post_idx = prox_significans(signa, n, par_cl + 1);
            necessitat_bracchia = 1;
        }

        /* else (sine if) */
        if (est_verbum(&signa[i], "else")) {
            post_idx = prox_significans(signa, n, i + 1);
            if (post_idx < n && est_verbum(&signa[post_idx], "if"))
                continue;
            necessitat_bracchia = 1;
        }

        /* do */
        if (est_verbum(&signa[i], "do")) {
            post_idx = prox_significans(signa, n, i + 1);
            necessitat_bracchia = 1;
        }

        if (!necessitat_bracchia || post_idx < 0 || post_idx >= n)
            continue;

        if (signa[post_idx].genus != SIGNUM_APERTIO) {
            char nuntius[NUNTIUS_MAX];
            snprintf(
                nuntius, sizeof(nuntius),
                "bracchia necessaria post '%.*s'",
                signa[i].longitudo, signa[i].initium
            );
            inspector_adde(
                ins, GRAVITAS_MONITUM,
                signa[i].linea, signa[i].columna,
                "bracchia_necessaria", nuntius
            );
        }
    }
}
