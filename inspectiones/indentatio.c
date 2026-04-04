/*
 * inspectiones/indentatio.c — indentatio per bracchia et parentheses
 */

#include "../inspectio.h"

#include <stdio.h>

#define CONTINUATIO_MAX 32

typedef struct {
    int columna_post;   /* columna characteris post '(' */
    int basis_spatia;   /* indentatio lineae cum '(' */
    int modus_massa;    /* 1 si nihil sequitur '(' in eadem linea */
} continuatio_gradus_t;

void inspice_indentationem(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus,
    const speculum_t *spec
) {
    int lat = spec->ind_tabulis
    ? spec->ind_latitudo * 8
        : spec->ind_latitudo;
    const signum_t *signa = lex->signa;
    int n = lex->num_signa;

    int profunditas = 0;    /* profunditas bracchiorum { } */
    continuatio_gradus_t acervus[CONTINUATIO_MAX];
    int prof_par         = 0;       /* profunditas parenthesium */
    int gradus_virtualis = 0;  /* +1 pro corpore sine bracchiis */
    int cont_basis       = -1; /* basis indentationis continuationis */

    for (int vi = 0; vi < num_versus; vi++) {
        const versus_t *v = &versus[vi];

        /* transili lineas vacuas */
        if (versus_vacuus(v))
            continue;

        int primus = v->tok_primus;

        /* transili praeprocessorem et commentaria pura */
        if (
            signa[primus].genus == SIGNUM_PRAEPROCESSOR ||
            signa[primus].genus == SIGNUM_COMMENTARIUM
        )
            goto adiusta_statum;

        /* computa indentationem expectatam */
        int expectata;

        if (prof_par > 0) {
            /* intra continuationem parenthesium */
            continuatio_gradus_t *ctx = &acervus[prof_par - 1];

            int primus_est_clausio =
                (
                    signa[primus].genus == SIGNUM_CLAUSIO_PAR ||
                    signa[primus].genus == SIGNUM_CLAUSIO_QUAD
                );

            if (primus_est_clausio) {
                expectata = ctx->basis_spatia;
            } else if (spec->ind_continuatio == 0 && !ctx->modus_massa) {
                expectata = ctx->columna_post;
                /* etiam accepta block indentatio inter basis+lat et
                 * columna_post — multi stili validi intra parentheses */
                if (
                    v->spatia >= ctx->basis_spatia + lat &&
                    v->spatia <= ctx->columna_post
                )
                    expectata = v->spatia;
            } else {
                expectata = ctx->basis_spatia + lat;
            }
        } else {
            /* indentatio normalis per bracchia */
            expectata = profunditas * lat;

            /* } decrescit ante inspectionem */
            if (signa[primus].genus == SIGNUM_CLAUSIO) {
                if (profunditas > 0)
                    profunditas--;
                expectata        = profunditas * lat;
                gradus_virtualis = 0;
            }

            /* applica gradum virtualem (corpus sine bracchiis) */
            if (
                gradus_virtualis > 0 &&
                signa[primus].genus != SIGNUM_APERTIO
            ) {
                expectata += gradus_virtualis * lat;
            }
            /* si '{' post gradum virtualem, cancellandum */
            if (signa[primus].genus == SIGNUM_APERTIO)
                gradus_virtualis = 0;

            /* case/default: unus gradus minus */
            if (
                est_verbum(&signa[primus], "case") ||
                est_verbum(&signa[primus], "default")
            ) {
                expectata = (profunditas > 0) ? (profunditas - 1) * lat
                                              : 0;
                gradus_virtualis = 0;
            }

            /* label (verbum sequitur ':'): unus gradus minus */
            if (
                signa[primus].genus == SIGNUM_VERBUM &&
                !est_verbum(&signa[primus], "case") &&
                !est_verbum(&signa[primus], "default")
            ) {
                for (int j = primus + 1; j < v->tok_finis; j++) {
                    if (signa[j].genus == SIGNUM_SPATIUM)
                        continue;
                    if (est_operator(&signa[j], ":"))
                        expectata = 0;
                    break;
                }
            }
        }

        /* continuatio expressionis sine parenthesibus */
        if (prof_par == 0) {
            int est_cont = 0;

            /* bracchia non sunt continuatio */
            if (
                signa[primus].genus == SIGNUM_APERTIO ||
                signa[primus].genus == SIGNUM_CLAUSIO
            )
                goto non_cont;

            /* concatenatio implicita chordarum — praetermitte */
            if (signa[primus].genus == SIGNUM_CHORDA) {
                int vp = vi - 1;
                while (vp >= 0 && versus_vacuus(&versus[vp]))
                    vp--;
                if (
                    vp >= 0 &&
                    signa[versus[vp].tok_primus].genus ==
                        SIGNUM_CHORDA &&
                    versus[vp].spatia == v->spatia
                ) {
                    goto adiusta_statum;
                }
            }

            /* post-stilus: linea incipit cum operatore binario,
             * '?' vel ':' */
            if (
                est_operator_binarius(&signa[primus]) ||
                est_operator(&signa[primus], "?") ||
                est_operator(&signa[primus], ":")
            ) {
                est_cont = 1;
            } else if (
                signa[primus].genus == SIGNUM_OPERATOR &&
                !est_operator(&signa[primus], "++") &&
                !est_operator(&signa[primus], "--") &&
                !est_operator(&signa[primus], "!") &&
                !est_operator(&signa[primus], "~") &&
                !est_operator(&signa[primus], "*") &&
                !est_operator(&signa[primus], "&")
            ) {
                est_cont = 1;
            }

            /* prae-stilus: linea prior finit cum operatore */
            if (!est_cont) {
                int vp = vi - 1;
                while (vp >= 0 && versus_vacuus(&versus[vp]))
                    vp--;
                if (vp >= 0) {
                    int ult = ultimus_significans(
                        signa,
                        versus[vp].tok_initium,
                        versus[vp].tok_finis
                    );
                    if (ult >= 0) {
                        const signum_t *su = &signa[ult];
                        if (
                            est_operator_binarius(su) ||
                            est_operator(su, "?")
                        ) {
                            est_cont = 1;
                        } else if (
                            su->genus == SIGNUM_OPERATOR &&
                            !est_operator(su, "++") &&
                            !est_operator(su, "--") &&
                            !est_operator(su, "->") &&
                            !est_operator(su, ".")
                        ) {
                            est_cont = 1;
                        }
                    }
                }
            }

            if (est_cont) {
                /* determina basim sententiae */
                if (cont_basis < 0) {
                    int vp = vi - 1;
                    while (vp >= 0 && versus_vacuus(&versus[vp]))
                        vp--;
                    cont_basis = (vp >= 0) ? versus[vp].spatia
                                           : 0;
                }
                int cont_exp = cont_basis + lat;

                if (v->spatia != cont_exp) {
                    char nuntius[NUNTIUS_MAX];
                    snprintf(
                        nuntius, sizeof(nuntius),
                        "%d spatia inventa, %d expectata"
                        " (continuatio, basis %d)",
                        v->spatia, cont_exp, cont_basis
                    );
                    adde_fix(
                        ins, GRAVITAS_MONITUM,
                        v->numero, 0,
                        "indentatio", nuntius, cont_exp
                    );
                }
                goto adiusta_statum;
            } else {
non_cont:
                cont_basis = -1;
            }
        }

        /* verifica */
        if (v->spatia != expectata) {
            char nuntius[NUNTIUS_MAX];
            snprintf(
                nuntius, sizeof(nuntius),
                "%d spatia inventa, %d expectata (prof. %d, par. %d)",
                v->spatia, expectata, profunditas, prof_par
            );
            adde_fix(
                ins, GRAVITAS_MONITUM,
                v->numero, 0, "indentatio", nuntius, expectata
            );
        }

adiusta_statum:
        /* percurre signa lineae et adiusta statum */
        for (int j = v->tok_initium; j < v->tok_finis; j++) {
            if (signa[j].genus == SIGNUM_APERTIO) {
                if (prof_par == 0)
                    profunditas++;
            } else if (signa[j].genus == SIGNUM_CLAUSIO) {
                if (prof_par == 0 && j != v->tok_primus) {
                    if (profunditas > 0)
                        profunditas--;
                }
            } else if (
                signa[j].genus == SIGNUM_APERTIO_PAR ||
                signa[j].genus == SIGNUM_APERTIO_QUAD
            ) {
                if (prof_par < CONTINUATIO_MAX) {
                    continuatio_gradus_t *ctx = &acervus[prof_par];
                    ctx->basis_spatia = v->spatia;
                    ctx->columna_post = signa[j].columna + 1;
                    ctx->modus_massa = 1;
                    for (int k = j + 1; k < v->tok_finis; k++) {
                        if (
                            signa[k].genus == SIGNUM_SPATIUM ||
                            signa[k].genus == SIGNUM_COMMENTARIUM
                        )
                            continue;
                        ctx->columna_post = signa[k].columna;
                        ctx->modus_massa  = 0;
                        break;
                    }
                    /* stilus pendens/patens: '(' debet esse ultimum signum in
                     * linea. solum parentheses '()' multilineares flagella —
                     * '[' non tractatur (subscripti fere semper unilineares). */
                    if (
                        (
                            spec->ind_continuatio == 2 ||
                            spec->ind_continuatio == 3
                        ) &&
                        signa[j].genus == SIGNUM_APERTIO_PAR &&
                        !ctx->modus_massa
                    ) {
                        int par_cl = quaere_par_clausam(signa, n, j);
                        if (
                            par_cl >= 0 &&
                            signa[par_cl].linea != signa[j].linea
                        ) {
                            adde_apert(
                                ins, GRAVITAS_MONITUM,
                                v->numero, signa[j].columna,
                                "indentatio",
                                "'(' debet esse ultimum signum"
                                " in linea (stilus pendens/patens)",
                                v->spatia + lat,
                                signa[j].columna
                            );
                        }
                    }
                }
                prof_par++;
            } else if (
                signa[j].genus == SIGNUM_CLAUSIO_PAR ||
                signa[j].genus == SIGNUM_CLAUSIO_QUAD
            ) {
                if (prof_par > 0) {
                    /* stilus patens: ')' debet esse primum signum in linea */
                    if (
                        spec->ind_continuatio == 3 &&
                        signa[j].genus == SIGNUM_CLAUSIO_PAR &&
                        j != v->tok_primus
                    ) {
                        int par_ap = quaere_par_apertam(signa, j);
                        if (
                            par_ap >= 0 &&
                            signa[par_ap].linea != signa[j].linea
                        ) {
                            int bas = acervus[prof_par - 1].basis_spatia;
                            adde_scissio(
                                ins, GRAVITAS_MONITUM,
                                v->numero, signa[j].columna,
                                "indentatio",
                                "')' debet esse primum signum"
                                " in linea (stilus patens)",
                                bas, signa[j].columna
                            );
                        }
                    }
                    prof_par--;
                }
            }
        }

        /* determina an linea sequens debeat habere gradum virtualem */
        if (prof_par == 0 && !versus_vacuus(v)) {
            int ult = ultimus_significans(signa, v->tok_initium, v->tok_finis);
            int habet_apertionem = linea_habet_bracchium(
                signa,
                v->tok_initium,
                v->tok_finis
            );

            /* resette gradum virtualem si linea consumpsit eum */
            if (
                gradus_virtualis > 0 &&
                signa[primus].genus != SIGNUM_APERTIO &&
                signa[primus].genus != SIGNUM_PRAEPROCESSOR &&
                signa[primus].genus != SIGNUM_COMMENTARIUM
            ) {
                /* haec linea erat corpus — si non est ipsa controlum,
                 * resette; si est controlum sine bracchiis, catena */
                int est_cont = (
                    ult >= 0 &&
                    signa[ult].genus == SIGNUM_CLAUSIO_PAR &&
                    !habet_apertionem
                );
                int est_else_do = (
                    ult >= 0 &&
                    (
                        est_verbum(&signa[ult], "else") ||
                        est_verbum(&signa[ult], "do")
                    ) &&
                    !habet_apertionem
                );
                if (!est_cont && !est_else_do)
                    gradus_virtualis = 0;
            }

            /* pone gradum virtualem pro linea sequenti */
            if (ult >= 0 && !habet_apertionem) {
                /* post ')' sine '{': inspice an sit controlum */
                if (signa[ult].genus == SIGNUM_CLAUSIO_PAR) {
                    int apertio = quaere_par_apertam(signa, ult);
                    if (apertio >= 0) {
                        int ante = prox_significans_retro(signa, apertio - 1);
                        if (
                            ante >= 0 &&
                            (
                                est_verbum(&signa[ante], "if") ||
                                est_verbum(&signa[ante], "for") ||
                                est_verbum(&signa[ante], "while") ||
                                est_verbum(&signa[ante], "switch")
                            )
                        ) {
                            gradus_virtualis++;
                        }
                    }
                }
                /* post 'else' vel 'do' sine '{' */
                if (
                    est_verbum(&signa[ult], "else") ||
                    est_verbum(&signa[ult], "do")
                ) {
                    gradus_virtualis++;
                }
            }
        }
    }
}
