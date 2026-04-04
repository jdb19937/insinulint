/*
 * inspectiones/corpus_separatum.c — corpus in linea propria
 */

#include "../inspectio.h"

#include <stdio.h>

void inspice_corpus_separatum(
    inspector_t *ins, const lexator_t *lex,
    const speculum_t *spec
) {
    int n   = lex->num_signa;
    int lat = spec->ind_latitudo;
    const signum_t *signa = lex->signa;

    for (int i = 0; i < n; i++) {
        int corpus_idx = -1;

        /* if (...), for (...), while (...), switch (...) */
        if (
            est_verbum(&signa[i], "if") ||
            est_verbum(&signa[i], "for") ||
            est_verbum(&signa[i], "while") ||
            est_verbum(&signa[i], "switch")
        ) {
            int par_ap = prox_significans(signa, n, i + 1);
            if (par_ap >= n || signa[par_ap].genus != SIGNUM_APERTIO_PAR)
                continue;
            int par_cl = quaere_par_clausam(signa, n, par_ap);
            if (par_cl < 0)
                continue;

            /* while post do-while — excludimus */
            if (est_verbum(&signa[i], "while")) {
                int ante = prox_significans_retro(signa, i - 1);
                if (ante >= 0 && signa[ante].genus == SIGNUM_CLAUSIO)
                    continue;
            }

            corpus_idx = prox_significans(signa, n, par_cl + 1);
        }

        /* else (sine if sequente) */
        if (est_verbum(&signa[i], "else")) {
            int seq = prox_significans(signa, n, i + 1);
            if (seq < n && est_verbum(&signa[seq], "if"))
                continue;
            corpus_idx = seq;
        }

        /* do */
        if (est_verbum(&signa[i], "do"))
            corpus_idx = prox_significans(signa, n, i + 1);

        if (corpus_idx < 0 || corpus_idx >= n)
            continue;

        /* si corpus est '{', nihil agendum */
        if (signa[corpus_idx].genus == SIGNUM_APERTIO)
            continue;

        /* si corpus in eadem linea ac structura controli — flagella */
        if (signa[corpus_idx].linea != signa[i].linea)
            continue;

        /* computa indentationem lineae */
        int k = i;
        while (k > 0 && signa[k - 1].genus != SIGNUM_LINEA_NOVA)
            k--;
        int ind_lineae = 0;
        if (signa[k].genus == SIGNUM_SPATIUM) {
            for (int j = 0; j < signa[k].longitudo; j++) {
                if (signa[k].initium[j] == '\t')
                    ind_lineae += 8;
                else
                    ind_lineae++;
            }
        }

        char nuntius[NUNTIUS_MAX];
        snprintf(
            nuntius, sizeof(nuntius),
            "corpus post '%.*s' debet esse in linea propria",
            signa[i].longitudo, signa[i].initium
        );
        adde_fix(
            ins, GRAVITAS_MONITUM,
            signa[i].linea, signa[corpus_idx].columna,
            "corpus_separatum", nuntius,
            ind_lineae + lat
        );
    }
}
