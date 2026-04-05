/*
 * inspectiones/bracchia_stilus.c — stilus bracchiorum (K&R vel Allman)
 */

#include "../inspectio.h"
#include "bracchia_stilus.h"

/*
 * est_bracchium_controli — inspice an '{' pertineat ad structuram controli.
 * reddit: 1 = controlum (if/for/while/switch/else/do)
 *         0 = functio vel aliud
 */
static int est_bracchium_controli(const signum_t *signa, int idx_apertio)
{
    int ante = prox_significans_retro(signa, idx_apertio - 1);
    if (ante < 0)
        return 0;

    /* directe post else vel do */
    if (
        est_verbum(&signa[ante], "else") ||
        est_verbum(&signa[ante], "do")
    )
        return 1;

    /* post ')' — debemus inspicere an parenthesis pertineat ad controlum */
    if (signa[ante].genus == SIGNUM_CLAUSIO_PAR) {
        int apertio = quaere_par_apertam(signa, ante);
        if (apertio < 0)
            return 0;
        int ante_par = prox_significans_retro(signa, apertio - 1);
        if (ante_par < 0)
            return 0;
        if (
            est_verbum(&signa[ante_par], "if") ||
            est_verbum(&signa[ante_par], "for") ||
            est_verbum(&signa[ante_par], "while") ||
            est_verbum(&signa[ante_par], "switch")
        )
            return 1;
        return 0;
    }
    return 0;
}

void inspice_bracchia_stilum(
    inspector_t *ins, const lexator_t *lex,
    const speculum_t *spec
) {
    int n = lex->num_signa;
    const signum_t *signa = lex->signa;

    for (int i = 0; i < n; i++) {
        if (signa[i].genus != SIGNUM_APERTIO)
            continue;

        int ante = prox_significans_retro(signa, i - 1);
        if (ante < 0)
            continue;

        int controlum = est_bracchium_controli(signa, i);

        /* K&R: controlum in eadem linea, functiones in nova linea */
        /* Allman: omnia in nova linea */
        int linea_nova = habet_lineam_novam(signa, ante + 1, i);

        /* computa indentationem lineae '{' */
        int ind_bra = 0;
        {
            int k = i;
            while (k > 0 && signa[k - 1].genus != SIGNUM_LINEA_NOVA)
                k--;
            if (signa[k].genus == SIGNUM_SPATIUM) {
                for (int j = 0; j < signa[k].longitudo; j++) {
                    if (signa[k].initium[j] == '\t')
                        ind_bra += 8;
                    else
                        ind_bra++;
                }
            }
        }

        if (controlum) {
            if (spec->bra_stilus == 0 && linea_nova) {
                adde_bracchia(
                    ins, GRAVITAS_MONITUM,
                    signa[i].linea, signa[i].columna,
                    "bracchia_stilus",
                    "'{' debet esse in eadem linea (stilus K&R)",
                    ind_bra, signa[i].columna
                );
            }
            if (spec->bra_stilus == 1 && !linea_nova) {
                adde_bracchia(
                    ins, GRAVITAS_MONITUM,
                    signa[i].linea, signa[i].columna,
                    "bracchia_stilus",
                    "'{' debet esse in nova linea (stilus Allman)",
                    ind_bra, signa[i].columna
                );
            }
        } else {
            /* functiones et struct/enum: in Allman, in nova linea */
            if (spec->bra_stilus == 1 && !linea_nova) {
                if (
                    signa[ante].genus == SIGNUM_CLAUSIO_PAR ||
                    est_verbum(&signa[ante], "struct") ||
                    est_verbum(&signa[ante], "union") ||
                    est_verbum(&signa[ante], "enum")
                ) {
                    adde_bracchia(
                        ins, GRAVITAS_MONITUM,
                        signa[i].linea, signa[i].columna,
                        "bracchia_stilus",
                        "'{' debet esse in nova linea "
                        "(stilus Allman)",
                        ind_bra, signa[i].columna
                    );
                }
            }
        }
    }
}
