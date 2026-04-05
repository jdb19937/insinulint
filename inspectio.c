/*
 * inspectio.c — inspector, speculum, et orchestrator inspectionum
 *
 * Inspectiones singulae in inspectiones/ iacent.
 */

#include "insinulint.h"
#include "inspectio.h"
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

void inspector_adde(
    inspector_t *ins, gravitas_t g, int linea, int columna,
    const char *regula, const char *nuntius
) {
    if (ins->num_monita >= MONITA_MAX)
        return;
    monitum_t *m     = &ins->monita[ins->num_monita++];
    m->gravitas      = g;
    m->linea         = linea;
    m->columna       = columna;
    m->fix_valor     = -1;
    m->split_columna = -1;
    m->apert_columna = -1;
    m->bra_columna   = -1;
    snprintf(m->regula,  sizeof(m->regula),  "%s", regula);
    snprintf(m->nuntius, sizeof(m->nuntius), "%s", nuntius);
}

int inspector_scribe(const inspector_t *ins)
{
    for (int i = 0; i < ins->num_monita; i++) {
        const monitum_t *m = &ins->monita[i];
        fprintf(
            stderr, "%s:%d:%d: %s [%s] %s\n",
            ins->via_fasciculi ? ins->via_fasciculi : "<fons>",
            m->linea, m->columna,
            m->gravitas == GRAVITAS_ERRATUM ? "erratum" : "monitum",
            m->regula,
            m->nuntius
        );
    }
    return ins->num_monita;
}

/* ================================================================
 * speculum — configuratio ex ISON
 * ================================================================ */

/* auxiliarium: lege valorem veritatis ex ISON */
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
static int valida_sectionem(
    const char *schema_ison, const char *spec_ison,
    const char *nomen
) {
    char *sec_schema = ison_da_crudum(schema_ison, nomen);
    if (!sec_schema)
        return 0;

    schema_t s;
    if (schema_lege(sec_schema, &s) < 0) {
        free(sec_schema);
        return 0;
    }
    free(sec_schema);

    char *sec_data = ison_da_crudum(spec_ison, nomen);
    if (!sec_data)
        return 0;

    ison_par_t pares[32];
    int np = ison_lege(sec_data, pares, 32);
    free(sec_data);
    if (np <= 0)
        return 0;

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
    char *schema_ison = ison_lege_plicam("insinulint-schema.ison");
    if (!schema_ison)
        return 0;

    static const char *sectiones[] = {
        "indentatio", "bracchia", "spatia", "lineae",
        "corpus", "commentaria"
    };
    int errores = 0;
    for (int i = 0; i < 6; i++) {
        if (valida_sectionem(schema_ison, ison, sectiones[i]) < 0)
            errores++;
    }
    free(schema_ison);
    return errores > 0 ? -1 : 0;
}

int speculum_lege(speculum_t *spec, const char *via)
{
    /* defalta */
    spec->ind_latitudo    = 4;
    spec->ind_tabulis     = 0;
    spec->ind_continuatio = 0;

    spec->bra_stilus           = 0;
    spec->bra_else_coniunctum  = 1;
    spec->bra_necessaria       = 1;

    spec->spa_post_verba       = 1;
    spec->spa_circa_operatores = 1;
    spec->spa_post_virgulam    = 1;
    spec->spa_ante_semicolon   = 1;
    spec->spa_adtributio_colineata = 1;

    spec->lin_longitudo_max     = 80;
    spec->lin_spatia_terminalia = 1;
    spec->lin_vacuae_max        = 2;
    spec->lin_finis_nova        = 1;
    spec->lin_tabulae_mixtae    = 1;
    spec->lin_una_sententia     = 1;

    spec->cor_separatum = 1;

    spec->com_veta = 0;

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
        else if (strcmp(cont, "pendens") == 0)
            spec->ind_continuatio = 2;
        else if (strcmp(cont, "patens") == 0)
            spec->ind_continuatio = 3;
        free(cont);
    }

    /* bracchia */
    char *stilus = ison_da_chordam(ison, "bracchia.stilus");
    if (stilus) {
        if (strcmp(stilus, "allman") == 0)
            spec->bra_stilus = 1;
        free(stilus);
    }

    spec->bra_else_coniunctum = lege_verum(
        ison, "bracchia.else_coniunctum",
        spec->bra_else_coniunctum
    );
    spec->bra_necessaria = lege_verum(
        ison, "bracchia.necessaria",
        spec->bra_necessaria
    );

    /* spatia */
    spec->spa_post_verba = lege_verum(
        ison, "spatia.post_verba_clavis",
        spec->spa_post_verba
    );
    spec->spa_circa_operatores = lege_verum(
        ison, "spatia.circa_operatores",
        spec->spa_circa_operatores
    );
    spec->spa_post_virgulam = lege_verum(
        ison, "spatia.post_virgulam",
        spec->spa_post_virgulam
    );
    spec->spa_ante_semicolon = lege_verum(
        ison, "spatia.ante_semicolon",
        spec->spa_ante_semicolon
    );
    spec->spa_adtributio_colineata = lege_verum(
        ison, "spatia.adtributio_colineata",
        spec->spa_adtributio_colineata
    );

    /* lineae */
    long lmax = ison_da_numerum(ison, "lineae.longitudo_maxima");
    if (lmax > 0)
        spec->lin_longitudo_max = (int)lmax;

    spec->lin_spatia_terminalia = lege_verum(
        ison, "lineae.spatia_terminalia",
        spec->lin_spatia_terminalia
    );

    long vmax = ison_da_numerum(ison, "lineae.vacuae_max");
    if (vmax > 0)
        spec->lin_vacuae_max = (int)vmax;

    spec->lin_finis_nova = lege_verum(
        ison, "lineae.finis_linea_nova",
        spec->lin_finis_nova
    );
    spec->lin_tabulae_mixtae = lege_verum(
        ison, "lineae.tabulae_mixtae",
        spec->lin_tabulae_mixtae
    );
    spec->lin_una_sententia = lege_verum(
        ison, "lineae.una_sententia",
        spec->lin_una_sententia
    );

    /* corpus */
    spec->cor_separatum = lege_verum(
        ison, "corpus.separatum",
        spec->cor_separatum
    );

    /* commentaria */
    spec->com_veta = lege_verum(
        ison, "commentaria.veta",
        spec->com_veta
    );

    free(ison);
    return 0;
}

/* ================================================================
 * insinulint_lege_inspice — pipeline communis: lege, lexa, inspice
 * ================================================================ */

int insinulint_lege_inspice(
    const char *via, const speculum_t *spec,
    inspector_t *ins, char **fons_out
) {
    char *fons = ison_lege_plicam(via);
    if (!fons) {
        fprintf(stderr, "insinulint: non possum legere '%s'\n", via);
        return -1;
    }
    size_t lon = strlen(fons);

    lexator_t lex;
    if (lexator_disseca(&lex, fons, lon) < 0) {
        fprintf(stderr, "insinulint: erratum dissecandi '%s'\n", via);
        free(fons);
        return -1;
    }

    inspector_initia(ins, via);
    inspice_omnia(ins, &lex, spec);
    lexator_purgare(&lex);

    *fons_out = fons;
    return 0;
}

/* ================================================================
 * inspice_omnia — orchestrator inspectionum
 * ================================================================ */

void inspice_omnia(
    inspector_t *ins, const lexator_t *lex,
    const speculum_t *spec
) {
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
        inspice_longitudinem(
            ins, lex, versus, nv, spec->lin_longitudo_max,
            spec->ind_tabulis ? 8 : spec->ind_latitudo
        );

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

    /* 14. adtributiones colineatae */
    if (spec->spa_adtributio_colineata)
        inspice_adtributiones_colineatas(ins, lex, versus, nv);

    /* 15. commentaria vetita */
    if (spec->com_veta)
        inspice_commentaria(ins, lex);

    /* 16. corpus separatum */
    if (spec->cor_separatum)
        inspice_corpus_separatum(ins, lex, spec);

    /* 17. una sententia per lineam */
    if (spec->lin_una_sententia)
        inspice_unam_sententiam(ins, lex, versus, nv);

    free(versus);
}
