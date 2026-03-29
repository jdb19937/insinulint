/*
 * inspectio.c — inspectiones regulares fontis C
 *
 * Regulae implementatae:
 *   - indentatio (cum continuatione per parentheses)
 *   - stilus bracchiorum (K&R vel Allman)
 *   - cubitus coniunctus (cuddled else)
 *   - bracchia necessaria (post if/for/while)
 *   - spatia post verba clavis (if, for, while, switch, return)
 *   - spatia circa operatores binarios
 *   - spatia post virgulam, ante semicolon
 *   - spatia terminalia
 *   - longitudo lineae
 *   - lineae vacuae consecutivae
 *   - linea nova ad finem fasciculi
 *   - tabulae et spatia mixtae
 */

#include "insinulint.h"
#include "ison.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================================================================
 * inspector — collectio monitorum
 * ================================================================ */

void inspector_initia(inspector_t *ins, const char *via)
{
    memset(ins, 0, sizeof(*ins));
    ins->via_fasciculi = via;
}

void inspector_adde(inspector_t *ins, gravitas_t g, int linea, int columna,
                    const char *regula, const char *nuntius)
{
    if (ins->num_monita >= MONITA_MAX)
        return;
    monitum_t *m = &ins->monita[ins->num_monita++];
    m->gravitas = g;
    m->linea    = linea;
    m->columna  = columna;
    snprintf(m->regula, sizeof(m->regula), "%s", regula);
    snprintf(m->nuntius, sizeof(m->nuntius), "%s", nuntius);
}

int inspector_scribe(const inspector_t *ins)
{
    for (int i = 0; i < ins->num_monita; i++) {
        const monitum_t *m = &ins->monita[i];
        fprintf(stderr, "%s:%d:%d: %s [%s] %s\n",
                ins->via_fasciculi ? ins->via_fasciculi : "<fons>",
                m->linea, m->columna,
                m->gravitas == GRAVITAS_ERRATUM ? "erratum" : "monitum",
                m->regula,
                m->nuntius);
    }
    return ins->num_monita;
}

/* ================================================================
 * speculum — configuratio ex JSON
 * ================================================================ */

/* auxiliarium: lege valorem veritatis ex JSON */
static int lege_verum(const char *ison, const char *via, int defaltum)
{
    char *crudum = ison_da_crudum(ison, via);
    if (!crudum)
        return defaltum;
    int res = (crudum[0] == 't' || crudum[0] == '1');
    free(crudum);
    return res;
}

/* valida sectionem speculi contra schema */
static int valida_sectionem(const char *schema_ison, const char *spec_ison,
                            const char *nomen)
{
    char *sec_schema = ison_da_crudum(schema_ison, nomen);
    if (!sec_schema) return 0; /* sectio in schemate non est */

    schema_t s;
    if (schema_lege(sec_schema, &s) < 0) {
        free(sec_schema);
        return 0;
    }
    free(sec_schema);

    char *sec_data = ison_da_crudum(spec_ison, nomen);
    if (!sec_data) return 0; /* sectio in speculo non est */

    ison_par_t pares[32];
    int np = ison_lege(sec_data, pares, 32);
    free(sec_data);
    if (np <= 0) return 0;

    char error[256];
    if (schema_valida(&s, pares, np, error, sizeof(error)) < 0) {
        fprintf(stderr, "insinulint: speculum.%s: %s\n", nomen, error);
        return -1;
    }
    return 0;
}

/* valida speculum integrum contra schema */
static int speculum_valida(const char *ison)
{
    char *schema_ison = ison_lege_plicam("speculum-schema.ison");
    if (!schema_ison) return 0; /* si schema non invenitur, transili */

    static const char *sectiones[] = {
        "indentatio", "bracchia", "spatia", "lineae"
    };
    int errores = 0;
    for (int i = 0; i < 4; i++) {
        if (valida_sectionem(schema_ison, ison, sectiones[i]) < 0)
            errores++;
    }
    free(schema_ison);
    return errores > 0 ? -1 : 0;
}

int speculum_lege(speculum_t *spec, const char *via)
{
    /* defalta */
    spec->ind_latitudo        = 4;
    spec->ind_tabulis         = 0;
    spec->ind_continuatio     = 0;  /* congruens (aligned) */

    spec->bra_stilus           = 0;  /* kr */
    spec->bra_else_coniunctum = 1;
    spec->bra_necessaria      = 1;

    spec->spa_post_verba       = 1;
    spec->spa_circa_operatores = 1;
    spec->spa_post_virgulam    = 1;
    spec->spa_ante_semicolon   = 1;

    spec->lin_longitudo_max    = 80;
    spec->lin_spatia_terminalia = 1;
    spec->lin_vacuae_max       = 2;
    spec->lin_finis_nova       = 1;
    spec->lin_tabulae_mixtae   = 1;

    if (!via)
        return 0;

    char *ison = ison_lege_plicam(via);
    if (!ison) {
        fprintf(stderr, "insinulint: non possum legere speculum '%s'\n", via);
        return -1;
    }

    /* valida contra schema */
    if (speculum_valida(ison) < 0) {
        free(ison);
        return -1;
    }

    /* indentatio */
    long lat = ison_da_numerum(ison, "indentatio.latitudo");
    if (lat > 0 && lat <= 16)
        spec->ind_latitudo = (int)lat;

    char *modus = ison_da_chordam(ison, "indentatio.modus");
    if (modus) {
        if (strcmp(modus, "tabulae") == 0)
            spec->ind_tabulis = 1;
        free(modus);
    }

    char *cont = ison_da_chordam(ison, "indentatio.continuatio");
    if (cont) {
        if (strcmp(cont, "massa") == 0)
            spec->ind_continuatio = 1;
        free(cont);
    }

    /* bracchia */
    char *stilus = ison_da_chordam(ison, "bracchia.stilus");
    if (stilus) {
        if (strcmp(stilus, "allman") == 0)
            spec->bra_stilus = 1;
        free(stilus);
    }

    spec->bra_else_coniunctum = lege_verum(ison, "bracchia.else_coniunctum",
                                            spec->bra_else_coniunctum);
    spec->bra_necessaria = lege_verum(ison, "bracchia.necessaria",
                                       spec->bra_necessaria);

    /* spatia */
    spec->spa_post_verba = lege_verum(ison, "spatia.post_verba_clavis",
                                       spec->spa_post_verba);
    spec->spa_circa_operatores = lege_verum(ison, "spatia.circa_operatores",
                                             spec->spa_circa_operatores);
    spec->spa_post_virgulam = lege_verum(ison, "spatia.post_virgulam",
                                          spec->spa_post_virgulam);
    spec->spa_ante_semicolon = lege_verum(ison, "spatia.ante_semicolon",
                                           spec->spa_ante_semicolon);

    /* lineae */
    long lmax = ison_da_numerum(ison, "lineae.longitudo_maxima");
    if (lmax > 0)
        spec->lin_longitudo_max = (int)lmax;

    spec->lin_spatia_terminalia = lege_verum(ison, "lineae.spatia_terminalia",
                                              spec->lin_spatia_terminalia);

    long vmax = ison_da_numerum(ison, "lineae.vacuae_max");
    if (vmax > 0)
        spec->lin_vacuae_max = (int)vmax;

    spec->lin_finis_nova = lege_verum(ison, "lineae.finis_linea_nova",
                                       spec->lin_finis_nova);
    spec->lin_tabulae_mixtae = lege_verum(ison, "lineae.tabulae_mixtae",
                                           spec->lin_tabulae_mixtae);

    free(ison);
    return 0;
}

/* ================================================================
 * auxiliaria communia
 * ================================================================ */

/* est signum verbum specificum? */
static int est_verbum(const signum_t *s, const char *verbum)
{
    int lon = (int)strlen(verbum);
    return s->genus == SIGNUM_VERBUM &&
           s->longitudo == lon &&
           memcmp(s->initium, verbum, (size_t)lon) == 0;
}

/* est signum operator specificus? */
static int est_operator(const signum_t *s, const char *op)
{
    int lon = (int)strlen(op);
    return s->genus == SIGNUM_OPERATOR &&
           s->longitudo == lon &&
           memcmp(s->initium, op, (size_t)lon) == 0;
}

/* est signum verbum clavis requirendi spatium? */
static int est_verbum_cum_spatio(const signum_t *s)
{
    return est_verbum(s, "if") || est_verbum(s, "for") ||
           est_verbum(s, "while") || est_verbum(s, "switch") ||
           est_verbum(s, "return") || est_verbum(s, "case");
}

/* est operator binarius non ambiguus? (semper requirit spatia) */
static int est_operator_binarius(const signum_t *s)
{
    if (s->genus != SIGNUM_OPERATOR)
        return 0;
    /* compositi — semper binarii */
    if (est_operator(s, "==") || est_operator(s, "!=") ||
        est_operator(s, "<=") || est_operator(s, ">=") ||
        est_operator(s, "&&") || est_operator(s, "||") ||
        est_operator(s, "+=") || est_operator(s, "-=") ||
        est_operator(s, "*=") || est_operator(s, "/=") ||
        est_operator(s, "%=") || est_operator(s, "&=") ||
        est_operator(s, "|=") || est_operator(s, "^=") ||
        est_operator(s, "<<=") || est_operator(s, ">>="))
        return 1;
    /* assignatio simplex (longitudo 1, '=') */
    if (s->longitudo == 1 && s->initium[0] == '=')
        return 1;
    return 0;
}

/* quaere parenthesim congruam retro: dato indice ad ')', reddit indicem ad '(' */
static int quaere_par_apertam(const signum_t *signa, int idx)
{
    int prof = 0;
    for (int i = idx; i >= 0; i--) {
        if (signa[i].genus == SIGNUM_CLAUSIO_PAR)
            prof++;
        else if (signa[i].genus == SIGNUM_APERTIO_PAR) {
            prof--;
            if (prof == 0)
                return i;
        }
    }
    return -1;
}

/* quaere parenthesim congruam: dato indice ad '(', reddit indicem ad ')' */
static int quaere_par_clausam(const signum_t *signa, int n, int idx)
{
    int prof = 0;
    for (int i = idx; i < n; i++) {
        if (signa[i].genus == SIGNUM_APERTIO_PAR)
            prof++;
        else if (signa[i].genus == SIGNUM_CLAUSIO_PAR) {
            prof--;
            if (prof == 0)
                return i;
        }
    }
    return -1;
}

/* inveni primum signum non-spatium/non-linea-nova post indicem */
static int prox_significans(const signum_t *signa, int n, int idx)
{
    for (int i = idx; i < n; i++) {
        if (signa[i].genus != SIGNUM_SPATIUM &&
            signa[i].genus != SIGNUM_LINEA_NOVA &&
            signa[i].genus != SIGNUM_COMMENTARIUM)
            return i;
    }
    return n;
}

/* inveni signum significans retro */
static int prox_significans_retro(const signum_t *signa, int idx)
{
    for (int i = idx; i >= 0; i--) {
        if (signa[i].genus != SIGNUM_SPATIUM &&
            signa[i].genus != SIGNUM_LINEA_NOVA &&
            signa[i].genus != SIGNUM_COMMENTARIUM)
            return i;
    }
    return -1;
}

/* est linea nova inter duos indices? */
static int habet_lineam_novam(const signum_t *signa, int a, int b)
{
    for (int i = a; i <= b; i++) {
        if (signa[i].genus == SIGNUM_LINEA_NOVA)
            return 1;
    }
    return 0;
}

/* ================================================================
 * versus — repraesentatio linearum
 * ================================================================ */

#define VERSUS_MAX (1 << 16)

typedef struct {
    int tok_initium;    /* index primi signi in linea */
    int tok_finis;      /* index post ultimum (ante LINEA_NOVA/FINIS) */
    int tok_primus;     /* index primi signi non-spatii */
    int numero;         /* numerus lineae (ab 1) */
    int spatia;         /* spatia initialia (characteres) */
} versus_t;

static int aedifica_versus(const lexator_t *lex, versus_t *vv, int max)
{
    int nv = 0;
    int i = 0;
    int n = lex->num_signa;
    const signum_t *s = lex->signa;

    while (i < n && nv < max) {
        versus_t *v = &vv[nv];
        v->tok_initium = i;

        /* inveni finem lineae */
        int j = i;
        while (j < n && s[j].genus != SIGNUM_LINEA_NOVA &&
               s[j].genus != SIGNUM_FINIS)
            j++;
        v->tok_finis = j;

        /* primus non-spatium */
        v->tok_primus = i;
        if (v->tok_primus < j && s[v->tok_primus].genus == SIGNUM_SPATIUM)
            v->tok_primus++;

        /* numerus lineae */
        if (v->tok_primus < j)
            v->numero = s[v->tok_primus].linea;
        else if (j < n)
            v->numero = s[j].linea;
        else
            v->numero = nv + 1;

        /* spatia initialia */
        v->spatia = 0;
        if (i < j && s[i].genus == SIGNUM_SPATIUM) {
            for (int k = 0; k < s[i].longitudo; k++) {
                if (s[i].initium[k] == '\t')
                    v->spatia += 8;
                else
                    v->spatia++;
            }
        }

        nv++;

        /* transili LINEA_NOVA */
        if (j < n && s[j].genus == SIGNUM_LINEA_NOVA)
            i = j + 1;
        else
            i = j;

        if (j < n && s[j].genus == SIGNUM_FINIS)
            break;
    }
    return nv;
}

/* est versus vacuus? (nulla signa non-spatia) */
static int versus_vacuus(const versus_t *v)
{
    return v->tok_primus >= v->tok_finis;
}

/* ================================================================
 * 1. INDENTATIO — cum continuatione per parentheses
 * ================================================================ */

#define CONTINUATIO_MAX 32

typedef struct {
    int columna_post;   /* columna characteris post '(' */
    int basis_spatia;   /* indentatio lineae cum '(' */
    int modus_massa;    /* 1 si nihil sequitur '(' in eadem linea */
} continuatio_gradus_t;

/*
 * inspice ultimum signum significans in linea.
 * reddit indicem, vel -1 si linea vacua.
 */
static int ultimus_significans(const signum_t *signa, int initium, int finis)
{
    for (int j = finis - 1; j >= initium; j--) {
        if (signa[j].genus != SIGNUM_SPATIUM &&
            signa[j].genus != SIGNUM_COMMENTARIUM)
            return j;
    }
    return -1;
}

/*
 * est_linea_habet_bracchium — inspice an '{' sit in linea.
 */
static int linea_habet_bracchium(const signum_t *signa, int initium, int finis)
{
    for (int j = initium; j < finis; j++) {
        if (signa[j].genus == SIGNUM_APERTIO)
            return 1;
    }
    return 0;
}

static void inspice_indentationem(inspector_t *ins, const lexator_t *lex,
                                  const versus_t *versus, int num_versus,
                                  const speculum_t *spec)
{
    int lat = spec->ind_latitudo;
    const signum_t *signa = lex->signa;
    (void)lex;

    int profunditas = 0;    /* profunditas bracchiorum { } */
    continuatio_gradus_t acervus[CONTINUATIO_MAX];
    int prof_par = 0;       /* profunditas parenthesium */
    int gradus_virtualis = 0;  /* +1 pro corpore sine bracchiis */

    for (int vi = 0; vi < num_versus; vi++) {
        const versus_t *v = &versus[vi];

        /* transili lineas vacuas */
        if (versus_vacuus(v))
            continue;

        int primus = v->tok_primus;

        /* transili praeprocessorem et commentaria pura */
        if (signa[primus].genus == SIGNUM_PRAEPROCESSOR ||
            signa[primus].genus == SIGNUM_COMMENTARIUM)
            goto adiusta_statum;

        /* computa indentationem expectatam */
        int expectata;

        if (prof_par > 0) {
            /* intra continuationem parenthesium */
            continuatio_gradus_t *ctx = &acervus[prof_par - 1];

            int primus_est_clausio =
                (signa[primus].genus == SIGNUM_CLAUSIO_PAR ||
                 signa[primus].genus == SIGNUM_CLAUSIO_QUAD);

            if (primus_est_clausio) {
                expectata = ctx->basis_spatia;
            } else if (spec->ind_continuatio == 0 && !ctx->modus_massa) {
                expectata = ctx->columna_post;
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
                expectata = profunditas * lat;
                gradus_virtualis = 0;
            }

            /* applica gradum virtualem (corpus sine bracchiis) */
            if (gradus_virtualis > 0 &&
                signa[primus].genus != SIGNUM_APERTIO) {
                expectata += gradus_virtualis * lat;
            }
            /* si '{' post gradum virtualem, cancellandum */
            if (signa[primus].genus == SIGNUM_APERTIO)
                gradus_virtualis = 0;

            /* case/default: unus gradus minus */
            if (est_verbum(&signa[primus], "case") ||
                est_verbum(&signa[primus], "default")) {
                expectata = (profunditas > 0) ? (profunditas - 1) * lat
                                              : 0;
                gradus_virtualis = 0;
            }

            /* label (verbum sequitur ':'): unus gradus minus */
            if (signa[primus].genus == SIGNUM_VERBUM &&
                !est_verbum(&signa[primus], "case") &&
                !est_verbum(&signa[primus], "default")) {
                for (int j = primus + 1; j < v->tok_finis; j++) {
                    if (signa[j].genus == SIGNUM_SPATIUM)
                        continue;
                    if (est_operator(&signa[j], ":"))
                        expectata = 0;
                    break;
                }
            }
        }

        /* verifica */
        if (v->spatia != expectata) {
            char nuntius[NUNTIUS_MAX];
            snprintf(nuntius, sizeof(nuntius),
                     "%d spatia inventa, %d expectata (prof. %d, par. %d)",
                     v->spatia, expectata, profunditas, prof_par);
            inspector_adde(ins, GRAVITAS_MONITUM,
                           v->numero, 0, "indentatio", nuntius);
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
            } else if (signa[j].genus == SIGNUM_APERTIO_PAR ||
                       signa[j].genus == SIGNUM_APERTIO_QUAD) {
                if (prof_par < CONTINUATIO_MAX) {
                    continuatio_gradus_t *ctx = &acervus[prof_par];
                    ctx->basis_spatia = v->spatia;
                    ctx->columna_post = signa[j].columna + 1;
                    ctx->modus_massa = 1;
                    for (int k = j + 1; k < v->tok_finis; k++) {
                        if (signa[k].genus == SIGNUM_SPATIUM ||
                            signa[k].genus == SIGNUM_COMMENTARIUM)
                            continue;
                        ctx->columna_post = signa[k].columna;
                        ctx->modus_massa = 0;
                        break;
                    }
                }
                prof_par++;
            } else if (signa[j].genus == SIGNUM_CLAUSIO_PAR ||
                       signa[j].genus == SIGNUM_CLAUSIO_QUAD) {
                if (prof_par > 0)
                    prof_par--;
            }
        }

        /* determina an linea sequens debeat habere gradum virtualem */
        if (prof_par == 0 && !versus_vacuus(v)) {
            int ult = ultimus_significans(signa, v->tok_initium, v->tok_finis);
            int habet_apertionem = linea_habet_bracchium(signa,
                                                          v->tok_initium,
                                                          v->tok_finis);

            /* resette gradum virtualem si linea consumpsit eum */
            if (gradus_virtualis > 0 &&
                signa[primus].genus != SIGNUM_APERTIO &&
                signa[primus].genus != SIGNUM_PRAEPROCESSOR &&
                signa[primus].genus != SIGNUM_COMMENTARIUM) {
                /* haec linea erat corpus — si non est ipsa controlum,
                 * resette; si est controlum sine bracchiis, catena */
                int est_cont = (ult >= 0 &&
                    signa[ult].genus == SIGNUM_CLAUSIO_PAR &&
                    !habet_apertionem);
                int est_else_do = (ult >= 0 &&
                    (est_verbum(&signa[ult], "else") ||
                     est_verbum(&signa[ult], "do")) &&
                    !habet_apertionem);
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
                        if (ante >= 0 &&
                            (est_verbum(&signa[ante], "if") ||
                             est_verbum(&signa[ante], "for") ||
                             est_verbum(&signa[ante], "while") ||
                             est_verbum(&signa[ante], "switch"))) {
                            gradus_virtualis++;
                        }
                    }
                }
                /* post 'else' vel 'do' sine '{' */
                if (est_verbum(&signa[ult], "else") ||
                    est_verbum(&signa[ult], "do")) {
                    gradus_virtualis++;
                }
            }
        }
    }
}

/* ================================================================
 * 2. SPATIA TERMINALIA — veta spatia ad finem lineae
 * ================================================================ */

static void inspice_spatia_terminalia(inspector_t *ins,
                                      const lexator_t *lex,
                                      const versus_t *versus,
                                      int num_versus)
{
    const signum_t *signa = lex->signa;

    for (int vi = 0; vi < num_versus; vi++) {
        const versus_t *v = &versus[vi];
        if (v->tok_finis <= v->tok_initium)
            continue;
        int ultimus = v->tok_finis - 1;
        if (signa[ultimus].genus == SIGNUM_SPATIUM) {
            inspector_adde(ins, GRAVITAS_MONITUM,
                           v->numero, signa[ultimus].columna,
                           "spatia_terminalia",
                           "spatia terminalia inventa");
        }
    }
}

/* ================================================================
 * 3. LONGITUDO LINEAE
 * ================================================================ */

static void inspice_longitudinem(inspector_t *ins, const lexator_t *lex,
                                 const versus_t *versus, int num_versus,
                                 int maxima)
{
    if (maxima <= 0)
        return;
    const signum_t *signa = lex->signa;

    for (int vi = 0; vi < num_versus; vi++) {
        const versus_t *v = &versus[vi];
        if (v->tok_finis <= v->tok_initium)
            continue;
        /* columna ultimi characteris */
        int ultimus = v->tok_finis - 1;
        int lon = signa[ultimus].columna + signa[ultimus].longitudo;
        if (lon > maxima) {
            char nuntius[NUNTIUS_MAX];
            snprintf(nuntius, sizeof(nuntius),
                     "linea %d columnarum (max %d)", lon, maxima);
            inspector_adde(ins, GRAVITAS_MONITUM,
                           v->numero, maxima,
                           "longitudo_lineae", nuntius);
        }
    }
}

/* ================================================================
 * 4. LINEAE VACUAE CONSECUTIVAE
 * ================================================================ */

static void inspice_lineas_vacuas(inspector_t *ins,
                                  const versus_t *versus, int num_versus,
                                  int maxima)
{
    if (maxima <= 0)
        return;
    int vacuae = 0;
    for (int vi = 0; vi < num_versus; vi++) {
        if (versus_vacuus(&versus[vi])) {
            vacuae++;
        } else {
            if (vacuae > maxima) {
                char nuntius[NUNTIUS_MAX];
                snprintf(nuntius, sizeof(nuntius),
                         "%d lineae vacuae consecutivae (max %d)",
                         vacuae, maxima);
                inspector_adde(ins, GRAVITAS_MONITUM,
                               versus[vi].numero, 0,
                               "lineae_vacuae", nuntius);
            }
            vacuae = 0;
        }
    }
}

/* ================================================================
 * 5. LINEA NOVA AD FINEM FASCICULI
 * ================================================================ */

static void inspice_finem(inspector_t *ins, const lexator_t *lex)
{
    int n = lex->num_signa;
    if (n < 2)
        return;
    /* ultimum signum est FINIS; inspice penultimum */
    if (lex->signa[n - 2].genus != SIGNUM_LINEA_NOVA) {
        inspector_adde(ins, GRAVITAS_MONITUM,
                       lex->signa[n - 1].linea, 0,
                       "finis_lineae",
                       "fasciculus non finit cum linea nova");
    }
}

/* ================================================================
 * 6. TABULAE ET SPATIA MIXTAE
 * ================================================================ */

static void inspice_tabulas_mixtas(inspector_t *ins, const lexator_t *lex,
                                   const versus_t *versus, int num_versus)
{
    const signum_t *signa = lex->signa;

    for (int vi = 0; vi < num_versus; vi++) {
        const versus_t *v = &versus[vi];
        if (v->tok_initium >= v->tok_finis)
            continue;
        if (signa[v->tok_initium].genus != SIGNUM_SPATIUM)
            continue;
        const signum_t *sp = &signa[v->tok_initium];
        int habet_tab = 0, habet_sp = 0;
        for (int k = 0; k < sp->longitudo; k++) {
            if (sp->initium[k] == '\t') habet_tab = 1;
            if (sp->initium[k] == ' ')  habet_sp = 1;
        }
        if (habet_tab && habet_sp) {
            inspector_adde(ins, GRAVITAS_MONITUM,
                           v->numero, 0,
                           "tabulae_mixtae",
                           "tabulae et spatia mixta in indentatione");
        }
    }
}

/* ================================================================
 * 7. SPATIA POST VERBA CLAVIS
 * ================================================================ */

static void inspice_spatia_verborum(inspector_t *ins, const lexator_t *lex,
                                    const versus_t *versus, int num_versus)
{
    const signum_t *signa = lex->signa;

    for (int vi = 0; vi < num_versus; vi++) {
        const versus_t *v = &versus[vi];
        for (int j = v->tok_initium; j < v->tok_finis; j++) {
            if (!est_verbum_cum_spatio(&signa[j]))
                continue;
            /* inspice sequens signum */
            int seq = j + 1;
            if (seq >= v->tok_finis)
                continue;
            /* si sequens est '(' directe sine spatio */
            if (signa[seq].genus == SIGNUM_APERTIO_PAR) {
                char nuntius[NUNTIUS_MAX];
                snprintf(nuntius, sizeof(nuntius),
                         "spatium requiritur post '%.*s'",
                         signa[j].longitudo, signa[j].initium);
                inspector_adde(ins, GRAVITAS_MONITUM,
                               signa[j].linea, signa[j].columna,
                               "spatium_verbum", nuntius);
            }
            /* si 'return' sequitur expressio sine spatio (non '(') */
            else if (est_verbum(&signa[j], "return") &&
                     signa[seq].genus != SIGNUM_SPATIUM &&
                     signa[seq].genus != SIGNUM_SEMICOLON &&
                     signa[seq].genus != SIGNUM_LINEA_NOVA) {
                inspector_adde(ins, GRAVITAS_MONITUM,
                               signa[j].linea, signa[j].columna,
                               "spatium_verbum",
                               "spatium requiritur post 'return'");
            }
        }
    }
}

/* ================================================================
 * 8. SPATIA CIRCA OPERATORES BINARIOS
 * ================================================================ */

static void inspice_spatia_operatorum(inspector_t *ins, const lexator_t *lex,
                                      const versus_t *versus, int num_versus)
{
    const signum_t *signa = lex->signa;

    for (int vi = 0; vi < num_versus; vi++) {
        const versus_t *v = &versus[vi];
        for (int j = v->tok_initium; j < v->tok_finis; j++) {
            if (!est_operator_binarius(&signa[j]))
                continue;

            /* inspice spatium ante operatorem */
            if (j > v->tok_initium &&
                signa[j - 1].genus != SIGNUM_SPATIUM) {
                char nuntius[NUNTIUS_MAX];
                snprintf(nuntius, sizeof(nuntius),
                         "spatium requiritur ante '%.*s'",
                         signa[j].longitudo, signa[j].initium);
                inspector_adde(ins, GRAVITAS_MONITUM,
                               signa[j].linea, signa[j].columna,
                               "spatium_operator", nuntius);
            }

            /* inspice spatium post operatorem */
            int seq = j + 1;
            if (seq < v->tok_finis &&
                signa[seq].genus != SIGNUM_SPATIUM &&
                signa[seq].genus != SIGNUM_LINEA_NOVA) {
                char nuntius[NUNTIUS_MAX];
                snprintf(nuntius, sizeof(nuntius),
                         "spatium requiritur post '%.*s'",
                         signa[j].longitudo, signa[j].initium);
                inspector_adde(ins, GRAVITAS_MONITUM,
                               signa[j].linea, signa[j].columna,
                               "spatium_operator", nuntius);
            }
        }
    }
}

/* ================================================================
 * 9. SPATIA POST VIRGULAM
 * ================================================================ */

static void inspice_virgulas(inspector_t *ins, const lexator_t *lex,
                             const versus_t *versus, int num_versus)
{
    const signum_t *signa = lex->signa;

    for (int vi = 0; vi < num_versus; vi++) {
        const versus_t *v = &versus[vi];
        for (int j = v->tok_initium; j < v->tok_finis; j++) {
            if (!est_operator(&signa[j], ","))
                continue;

            /* spatium ante virgulam non debet esse */
            if (j > v->tok_initium &&
                signa[j - 1].genus == SIGNUM_SPATIUM) {
                inspector_adde(ins, GRAVITAS_MONITUM,
                               signa[j].linea, signa[j].columna,
                               "spatium_virgula",
                               "spatium non debet esse ante virgulam");
            }

            /* spatium post virgulam debet esse */
            int seq = j + 1;
            if (seq < v->tok_finis &&
                signa[seq].genus != SIGNUM_SPATIUM &&
                signa[seq].genus != SIGNUM_LINEA_NOVA) {
                inspector_adde(ins, GRAVITAS_MONITUM,
                               signa[j].linea, signa[j].columna,
                               "spatium_virgula",
                               "spatium requiritur post virgulam");
            }
        }
    }
}

/* ================================================================
 * 10. SPATIA ANTE SEMICOLON
 * ================================================================ */

static void inspice_semicolona(inspector_t *ins, const lexator_t *lex,
                               const versus_t *versus, int num_versus)
{
    const signum_t *signa = lex->signa;

    for (int vi = 0; vi < num_versus; vi++) {
        const versus_t *v = &versus[vi];
        for (int j = v->tok_initium; j < v->tok_finis; j++) {
            if (signa[j].genus != SIGNUM_SEMICOLON)
                continue;
            if (j <= v->tok_initium)
                continue;
            if (signa[j - 1].genus != SIGNUM_SPATIUM)
                continue;

            /* exceptio: for ( ; ; ) — praecedentia '(' vel ';' */
            int ante = j - 2;
            while (ante >= v->tok_initium &&
                   signa[ante].genus == SIGNUM_SPATIUM)
                ante--;
            if (ante >= v->tok_initium &&
                (signa[ante].genus == SIGNUM_APERTIO_PAR ||
                 signa[ante].genus == SIGNUM_SEMICOLON))
                continue;

            inspector_adde(ins, GRAVITAS_MONITUM,
                           signa[j].linea, signa[j].columna,
                           "spatium_semicolon",
                           "spatium non debet esse ante semicolon");
        }
    }
}

/* ================================================================
 * 11. CUBITUS CONIUNCTUS (cuddled else)
 * ================================================================ */

static void inspice_cubitum(inspector_t *ins, const lexator_t *lex,
                            const speculum_t *spec)
{
    int n = lex->num_signa;
    const signum_t *signa = lex->signa;

    for (int i = 0; i < n; i++) {
        if (!est_verbum(&signa[i], "else"))
            continue;

        /* quaere '}' ante 'else' */
        int ante = i - 1;
        while (ante >= 0 && signa[ante].genus == SIGNUM_SPATIUM)
            ante--;

        if (ante < 0)
            continue;

        int linea_nova_inter = 0;
        if (signa[ante].genus == SIGNUM_LINEA_NOVA) {
            linea_nova_inter = 1;
            ante--;
            while (ante >= 0 && signa[ante].genus == SIGNUM_SPATIUM)
                ante--;
        }

        if (ante < 0 || signa[ante].genus != SIGNUM_CLAUSIO)
            continue;

        if (spec->bra_else_coniunctum && linea_nova_inter) {
            inspector_adde(ins, GRAVITAS_MONITUM,
                           signa[i].linea, signa[i].columna,
                           "cubitus",
                           "'else' debet esse in eadem linea ac '}'");
        }
        if (!spec->bra_else_coniunctum && !linea_nova_inter) {
            inspector_adde(ins, GRAVITAS_MONITUM,
                           signa[i].linea, signa[i].columna,
                           "cubitus",
                           "'else' debet esse in nova linea post '}'");
        }
    }
}

/* ================================================================
 * 12. STILUS BRACCHIORUM (K&R vel Allman)
 * ================================================================ */

/*
 * est_bracchium_controli — inspice an '{' pertineat ad structuram controli.
 * distinguit a functionibus et initializers.
 * reddit: 1 = controlum (if/for/while/switch/else/do)
 *         0 = functio vel aliud
 */
static int est_bracchium_controli(const signum_t *signa, int idx_apertio)
{
    int ante = prox_significans_retro(signa, idx_apertio - 1);
    if (ante < 0)
        return 0;

    /* directe post else vel do */
    if (est_verbum(&signa[ante], "else") ||
        est_verbum(&signa[ante], "do"))
        return 1;

    /* post ')' — debemus inspicere an parenthesis pertineat ad controlum */
    if (signa[ante].genus == SIGNUM_CLAUSIO_PAR) {
        int apertio = quaere_par_apertam(signa, ante);
        if (apertio < 0)
            return 0;
        int ante_par = prox_significans_retro(signa, apertio - 1);
        if (ante_par < 0)
            return 0;
        /* si verbum clavis controli praecedit '(' */
        if (est_verbum(&signa[ante_par], "if") ||
            est_verbum(&signa[ante_par], "for") ||
            est_verbum(&signa[ante_par], "while") ||
            est_verbum(&signa[ante_par], "switch"))
            return 1;
        /* aliter est functio */
        return 0;
    }
    return 0;
}

static void inspice_bracchia_stilum(inspector_t *ins, const lexator_t *lex,
                                    const speculum_t *spec)
{
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

        if (controlum) {
            if (spec->bra_stilus == 0 && linea_nova) {
                inspector_adde(ins, GRAVITAS_MONITUM,
                               signa[i].linea, signa[i].columna,
                               "bracchia_stilus",
                               "'{' debet esse in eadem linea (stilus K&R)");
            }
            if (spec->bra_stilus == 1 && !linea_nova) {
                inspector_adde(ins, GRAVITAS_MONITUM,
                               signa[i].linea, signa[i].columna,
                               "bracchia_stilus",
                               "'{' debet esse in nova linea (stilus Allman)");
            }
        } else {
            /* functiones et struct/enum: in Allman, in nova linea */
            if (spec->bra_stilus == 1 && !linea_nova) {
                /* si praecedentia est ')' vel verbum declarativum */
                if (signa[ante].genus == SIGNUM_CLAUSIO_PAR ||
                    est_verbum(&signa[ante], "struct") ||
                    est_verbum(&signa[ante], "union") ||
                    est_verbum(&signa[ante], "enum")) {
                    inspector_adde(ins, GRAVITAS_MONITUM,
                                   signa[i].linea, signa[i].columna,
                                   "bracchia_stilus",
                                   "'{' debet esse in nova linea "
                                   "(stilus Allman)");
                }
            }
            /* K&R: functiones in nova linea — ergo non monemus si in nova linea */
            /* sed si functio habet '{' in eadem linea cum K&R, permittimus etiam */
        }
    }
}

/* ================================================================
 * 13. BRACCHIA NECESSARIA (post if/for/while/else)
 * ================================================================ */

static void inspice_bracchia_necessaria(inspector_t *ins,
                                        const lexator_t *lex)
{
    int n = lex->num_signa;
    const signum_t *signa = lex->signa;

    for (int i = 0; i < n; i++) {
        int necessitat_bracchia = 0;
        int post_idx = -1;

        /* if (...), for (...), while (...), switch (...) */
        if (est_verbum(&signa[i], "if") ||
            est_verbum(&signa[i], "for") ||
            est_verbum(&signa[i], "while") ||
            est_verbum(&signa[i], "switch")) {

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
                /* si ante est '}', est do-while — non monemus */
                if (ante >= 0 && signa[ante].genus == SIGNUM_CLAUSIO)
                    continue;
            }

            post_idx = prox_significans(signa, n, par_cl + 1);
            necessitat_bracchia = 1;
        }

        /* else (sine if) */
        if (est_verbum(&signa[i], "else")) {
            post_idx = prox_significans(signa, n, i + 1);
            /* si sequitur 'if', est 'else if' — non monemus */
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
            snprintf(nuntius, sizeof(nuntius),
                     "bracchia necessaria post '%.*s'",
                     signa[i].longitudo, signa[i].initium);
            inspector_adde(ins, GRAVITAS_MONITUM,
                           signa[i].linea, signa[i].columna,
                           "bracchia_necessaria", nuntius);
        }
    }
}

/* ================================================================
 * inspice_omnia — orchestrator inspectionum
 * ================================================================ */

void inspice_omnia(inspector_t *ins, const lexator_t *lex,
                   const speculum_t *spec)
{
    /* aedifica tabulam versuum */
    versus_t *versus = malloc((size_t)VERSUS_MAX * sizeof(versus_t));
    if (!versus)
        return;
    int nv = aedifica_versus(lex, versus, VERSUS_MAX);

    /* 1. indentatio (cum continuatione) */
    inspice_indentationem(ins, lex, versus, nv, spec);

    /* 2. spatia terminalia */
    if (spec->lin_spatia_terminalia)
        inspice_spatia_terminalia(ins, lex, versus, nv);

    /* 3. longitudo lineae */
    if (spec->lin_longitudo_max > 0)
        inspice_longitudinem(ins, lex, versus, nv, spec->lin_longitudo_max);

    /* 4. lineae vacuae consecutivae */
    if (spec->lin_vacuae_max > 0)
        inspice_lineas_vacuas(ins, versus, nv, spec->lin_vacuae_max);

    /* 5. linea nova ad finem */
    if (spec->lin_finis_nova)
        inspice_finem(ins, lex);

    /* 6. tabulae mixtae */
    if (spec->lin_tabulae_mixtae)
        inspice_tabulas_mixtas(ins, lex, versus, nv);

    /* 7. spatia post verba clavis */
    if (spec->spa_post_verba)
        inspice_spatia_verborum(ins, lex, versus, nv);

    /* 8. spatia circa operatores */
    if (spec->spa_circa_operatores)
        inspice_spatia_operatorum(ins, lex, versus, nv);

    /* 9. spatia post virgulam */
    if (spec->spa_post_virgulam)
        inspice_virgulas(ins, lex, versus, nv);

    /* 10. spatia ante semicolon */
    if (spec->spa_ante_semicolon)
        inspice_semicolona(ins, lex, versus, nv);

    /* 11. cubitus coniunctus */
    inspice_cubitum(ins, lex, spec);

    /* 12. stilus bracchiorum */
    inspice_bracchia_stilum(ins, lex, spec);

    /* 13. bracchia necessaria */
    if (spec->bra_necessaria)
        inspice_bracchia_necessaria(ins, lex);

    free(versus);
}
