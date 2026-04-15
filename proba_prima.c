/*
 * proba.c — probationes automataticae lintoris insinulint
 *
 * Unusquisque casus scribit fontem C temporarium, lint eum,
 * et verificat numerum monitorum et regulas inventas.
 */

#include "insinulint.h"
#include "inspectio.h"
#include "correctio.h"
#include "commendatio.h"
#include "propositio.h"
#include "ison.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================================================================
 * apparatus probationis
 * ================================================================ */

static int probationes_totae = 0;
static int probationes_bonae = 0;
static int probationes_malae = 0;

/* curre lintorem in chorda fontis et reddit inspectorem */
static int curre_lintorem(
    const char *fons, const speculum_t *spec,
    inspector_t *ins
) {
    lexator_t lex;
    size_t lon = strlen(fons);
    if (lexator_disseca(&lex, fons, lon) < 0)
        return -1;

    inspector_initia(ins, "<proba>");
    inspice_omnia(ins, &lex, spec);
    lexator_purgare(&lex);
    return ins->num_monita;
}

/* numera monita cum regula specifica */
static int numera_regulam(const inspector_t *ins, const char *regula)
{
    int n = 0;
    for (int i = 0; i < ins->num_monita; i++) {
        if (strcmp(ins->monita[i].regula, regula) == 0)
            n++;
    }
    return n;
}

/* verificatio: expecta exacte tot monita regulae */
static void expecta(
    const char *nomen, const inspector_t *ins,
    const char *regula, int expectata
) {
    probationes_totae++;
    int inventa = numera_regulam(ins, regula);
    if (inventa == expectata) {
        probationes_bonae++;
    } else {
        probationes_malae++;
        fprintf(
            stderr, "  MALUM: %s — regula '%s': "
            "%d inventa, %d expectata\n",
            nomen, regula, inventa, expectata
        );
        /* monstra monita ad auxilium debugandi */
        for (int i = 0; i < ins->num_monita; i++) {
            if (strcmp(ins->monita[i].regula, regula) == 0) {
                fprintf(
                    stderr, "    linea %d: %s\n",
                    ins->monita[i].linea,
                    ins->monita[i].nuntius
                );
            }
        }
    }
}

/* verificatio: expecta nullum monitum ullius regulae */
static void expecta_mundum(const char *nomen, const inspector_t *ins)
{
    probationes_totae++;
    if (ins->num_monita == 0) {
        probationes_bonae++;
    } else {
        probationes_malae++;
        fprintf(
            stderr, "  MALUM: %s — %d monita inventa, 0 expectata\n",
            nomen, ins->num_monita
        );
        for (int i = 0; i < ins->num_monita; i++) {
            fprintf(
                stderr, "    linea %d [%s]: %s\n",
                ins->monita[i].linea,
                ins->monita[i].regula,
                ins->monita[i].nuntius
            );
        }
    }
}

/* speculum defaltum */
static speculum_t spec_defaltum(void)
{
    speculum_t spec;
    speculum_lege(&spec, NULL);
    return spec;
}

/* speculum cum omnibus regulis inactivis */
static speculum_t spec_solum(void)
{
    speculum_t spec;
    memset(&spec, 0, sizeof(spec));
    spec.ind_latitudo = 4;
    return spec;
}

/* ================================================================
 * probationes: indentatio
 * ================================================================ */

static void proba_indentatio_simplex(void)
{
    fprintf(stderr, "proba: indentatio simplex\n");
    speculum_t spec = spec_solum();
    inspector_t ins;

    /* codex mundus — 4 spatia per gradum */
    curre_lintorem(
        "int main(void)\n"
        "{\n"
        "    int a = 1;\n"
        "    if (a) {\n"
        "        a = 2;\n"
        "    }\n"
        "    return 0;\n"
        "}\n",
        &spec, &ins
    );
    expecta_mundum("mundus", &ins);

    /* indentatio mala */
    curre_lintorem(
        "int main(void)\n"
        "{\n"
        "      int a = 1;\n"  /* 6 spatia, 4 expectata */
        "}\n",
        &spec, &ins
    );
    expecta("nimis indentatum", &ins, "indentatio", 1);

    /* parum indentatum */
    curre_lintorem(
        "int f(void)\n"
        "{\n"
        "    if (1) {\n"
        "    int x;\n"       /* 4 spatia, 8 expectata */
        "    }\n"
        "}\n",
        &spec, &ins
    );
    expecta("parum indentatum", &ins, "indentatio", 1);
}

static void proba_indentatio_case(void)
{
    fprintf(stderr, "proba: indentatio case/default\n");
    speculum_t spec = spec_solum();
    inspector_t ins;

    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    switch (x) {\n"
        "    case 1:\n"
        "        break;\n"
        "    default:\n"
        "        break;\n"
        "    }\n"
        "}\n",
        &spec, &ins
    );
    expecta_mundum("case mundus", &ins);
}

static void proba_indentatio_virtualis(void)
{
    fprintf(stderr, "proba: indentatio virtualis (corpus sine bracchiis)\n");
    speculum_t spec = spec_solum();
    inspector_t ins;

    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    if (1)\n"
        "        return;\n"    /* 8 spatia — gradus virtualis */
        "    x = 1;\n"        /* 4 spatia — revertit */
        "}\n",
        &spec, &ins
    );
    expecta_mundum("virtualis simplex", &ins);

    /* catenatio: if-else sine bracchiis */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    if (1)\n"
        "        a();\n"
        "    else\n"
        "        b();\n"
        "}\n",
        &spec, &ins
    );
    expecta_mundum("virtualis if-else", &ins);
}

static void proba_continuatio_congruens(void)
{
    fprintf(stderr, "proba: continuatio congruens (aligned)\n");
    speculum_t spec      = spec_solum();
    spec.ind_continuatio = CONTINUATIO_CONGRUENS;
    inspector_t ins;

    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    functio(a,\n"
        "            b,\n"       /* aliniatum cum 'a' */
        "            c);\n"
        "}\n",
        &spec, &ins
    );
    expecta_mundum("congruens", &ins);
}

static void proba_continuatio_massa(void)
{
    fprintf(stderr, "proba: continuatio massa (block)\n");
    speculum_t spec      = spec_solum();
    spec.ind_continuatio = CONTINUATIO_MASSA;
    inspector_t ins;

    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    functio(\n"
        "        a,\n"           /* basis + lat */
        "        b,\n"
        "        c\n"
        "    );\n"               /* basis */
        "}\n",
        &spec, &ins
    );
    expecta_mundum("massa", &ins);
}

static void proba_continuatio_pendens(void)
{
    fprintf(stderr, "proba: continuatio pendens\n");
    speculum_t spec      = spec_solum();
    spec.ind_continuatio = CONTINUATIO_PENDENS;
    inspector_t ins;

    /* mundus: '(' in fine lineae */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    functio(\n"
        "        a,\n"
        "        b\n"
        "    );\n"
        "}\n",
        &spec, &ins
    );
    expecta_mundum("pendens mundus", &ins);

    /* malum: contentum sequitur '(' in eadem linea */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    functio(a,\n"
        "        b\n"
        "    );\n"
        "}\n",
        &spec, &ins
    );
    expecta("pendens: '(' non ultimum", &ins, "indentatio", 1);
}

static void proba_continuatio_patens(void)
{
    fprintf(stderr, "proba: continuatio patens\n");
    speculum_t spec      = spec_solum();
    spec.ind_continuatio = CONTINUATIO_PATENS;
    inspector_t ins;

    /* mundus: '(' in fine, ')' primum in linea propria */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    functio(\n"
        "        a,\n"
        "        b\n"
        "    );\n"
        "}\n",
        &spec, &ins
    );
    expecta_mundum("patens mundus", &ins);

    /* malum: ')' non primum in linea */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    functio(\n"
        "        a,\n"
        "        b);\n"
        "}\n",
        &spec, &ins
    );
    expecta("patens: ')' non primum", &ins, "indentatio", 1);

    /* malum: '(' non ultimum in linea */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    functio(a,\n"
        "        b\n"
        "    );\n"
        "}\n",
        &spec, &ins
    );
    expecta("patens: '(' non ultimum", &ins, "indentatio", 1);
}

/* ================================================================
 * probationes: spatia
 * ================================================================ */

static void proba_spatia_verba(void)
{
    fprintf(stderr, "proba: spatia post verba clavis\n");
    speculum_t spec     = spec_solum();
    spec.spa_post_verba = 1;
    inspector_t ins;

    /* mundus */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    if (1) { }\n"
        "    for (;;) { }\n"
        "    while (1) { }\n"
        "    switch (x) { }\n"
        "    return 0;\n"
        "}\n",
        &spec, &ins
    );
    expecta_mundum("verba munda", &ins);

    /* mala */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    if(1) { }\n"
        "    for(;;) { }\n"
        "    while(1) { }\n"
        "}\n",
        &spec, &ins
    );
    expecta("verba sine spatio", &ins, "spatium_verbum", 3);
}

static void proba_spatia_operatores(void)
{
    fprintf(stderr, "proba: spatia circa operatores\n");
    speculum_t spec = spec_solum();
    spec.spa_circa_operatores = 1;
    inspector_t ins;

    /* mundus */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    int a = 1;\n"
        "    a += 2;\n"
        "    if (a == 3 && a != 4) { }\n"
        "}\n",
        &spec, &ins
    );
    expecta_mundum("operatores mundi", &ins);

    /* mala */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    int a=1;\n"       /* ante et post = */
        "    a +=2;\n"         /* post += */
        "}\n",
        &spec, &ins
    );
    expecta("operatores mali", &ins, "spatium_operator", 3);
}

static void proba_spatia_virgulae(void)
{
    fprintf(stderr, "proba: spatia virgularum\n");
    speculum_t spec        = spec_solum();
    spec.spa_post_virgulam = 1;
    inspector_t ins;

    /* mundus */
    curre_lintorem(
        "void f(int a, int b, int c) { }\n",
        &spec, &ins
    );
    expecta_mundum("virgulae mundae", &ins);

    /* malum: sine spatio post */
    curre_lintorem(
        "void f(int a,int b) { }\n",
        &spec, &ins
    );
    expecta("sine spatio post virgulam", &ins, "spatium_virgula", 1);

    /* malum: spatium ante */
    curre_lintorem(
        "void f(int a , int b) { }\n",
        &spec, &ins
    );
    expecta("spatium ante virgulam", &ins, "spatium_virgula", 1);
}

static void proba_spatia_semicolona(void)
{
    fprintf(stderr, "proba: spatia semicolonorum\n");
    speculum_t spec         = spec_solum();
    spec.spa_ante_semicolon = 1;
    inspector_t ins;

    /* mundus */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    int a = 1;\n"
        "    return;\n"
        "}\n",
        &spec, &ins
    );
    expecta_mundum("semicolona munda", &ins);

    /* malum */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    int a = 1 ;\n"
        "}\n",
        &spec, &ins
    );
    expecta("spatium ante semicolon", &ins, "spatium_semicolon", 1);

    /* exceptio: for ( ; ; ) */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    for ( ; ; ) { }\n"
        "}\n",
        &spec, &ins
    );
    expecta_mundum("for vacula", &ins);
}

/* ================================================================
 * probationes: bracchia
 * ================================================================ */

static void proba_bracchia_necessaria(void)
{
    fprintf(stderr, "proba: bracchia necessaria\n");
    speculum_t spec     = spec_solum();
    spec.bra_necessaria = 1;
    inspector_t ins;

    /* mundus */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    if (1) {\n"
        "        a();\n"
        "    }\n"
        "    for (;;) {\n"
        "        b();\n"
        "    }\n"
        "}\n",
        &spec, &ins
    );
    expecta_mundum("bracchia praesentia", &ins);

    /* mala */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    if (1)\n"
        "        a();\n"
        "    for (;;)\n"
        "        b();\n"
        "    while (1)\n"
        "        c();\n"
        "}\n",
        &spec, &ins
    );
    expecta("bracchia absentia", &ins, "bracchia_necessaria", 3);
}

static void proba_cubitus(void)
{
    fprintf(stderr, "proba: cubitus coniunctus\n");
    speculum_t spec = spec_solum();
    spec.bra_else_coniunctum = 1;
    inspector_t ins;

    /* mundus: } else { */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    if (1) {\n"
        "        a();\n"
        "    } else {\n"
        "        b();\n"
        "    }\n"
        "}\n",
        &spec, &ins
    );
    expecta_mundum("cubitus coniunctus", &ins);

    /* malum: else in nova linea */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    if (1) {\n"
        "        a();\n"
        "    }\n"
        "    else {\n"
        "        b();\n"
        "    }\n"
        "}\n",
        &spec, &ins
    );
    expecta("else separatum", &ins, "cubitus", 1);

    /* contrarium: cubitus non coniunctus */
    spec.bra_else_coniunctum = 0;

    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    if (1) {\n"
        "        a();\n"
        "    } else {\n"
        "        b();\n"
        "    }\n"
        "}\n",
        &spec, &ins
    );
    expecta("cubitus non desideratus", &ins, "cubitus", 1);
}

static void proba_bracchia_stilus(void)
{
    fprintf(stderr, "proba: stilus bracchiorum\n");
    speculum_t spec = spec_solum();
    spec.bra_stilus = 0;  /* kr */
    inspector_t ins;

    /* K&R mundus: '{' in eadem linea ac controlum */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    if (1) {\n"
        "        a();\n"
        "    }\n"
        "}\n",
        &spec, &ins
    );
    expecta_mundum("kr mundus", &ins);

    /* K&R malum: '{' in nova linea post controlum */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    if (1)\n"
        "    {\n"
        "        a();\n"
        "    }\n"
        "}\n",
        &spec, &ins
    );
    expecta("kr malum", &ins, "bracchia_stilus", 1);

    /* K&R: functio cum '{' in nova linea — permissum */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "}\n",
        &spec, &ins
    );
    expecta_mundum("kr functio", &ins);

    /* Allman */
    spec.bra_stilus = 1;
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    if (1) {\n"        /* malum in Allman */
        "        a();\n"
        "    }\n"
        "}\n",
        &spec, &ins
    );
    expecta("allman malum", &ins, "bracchia_stilus", 1);
}

/* ================================================================
 * probationes: lineae
 * ================================================================ */

static void proba_spatia_terminalia(void)
{
    fprintf(stderr, "proba: spatia terminalia\n");
    speculum_t spec = spec_solum();
    spec.lin_spatia_terminalia = 1;
    inspector_t ins;

    /* mundus */
    curre_lintorem(
        "int a;\n"
        "int b;\n",
        &spec, &ins
    );
    expecta_mundum("sine spatiis terminalibus", &ins);

    /* malum */
    curre_lintorem(
        "int a;  \n"        /* spatia terminalia */
        "int b;\n",
        &spec, &ins
    );
    expecta("spatia terminalia", &ins, "spatia_terminalia", 1);
}

static void proba_longitudo_lineae(void)
{
    fprintf(stderr, "proba: longitudo lineae\n");
    speculum_t spec        = spec_solum();
    spec.lin_longitudo_max = 40;
    inspector_t ins;

    /* mundus */
    curre_lintorem(
        "int a = 1;\n",    /* brevis */
        &spec, &ins
    );
    expecta_mundum("linea brevis", &ins);

    /* malum */
    curre_lintorem(
        "int a_very_long_variable_name_that_exceeds = 1;\n",
        &spec, &ins
    );
    expecta("linea longa", &ins, "longitudo_lineae", 1);
}

static void proba_lineae_vacuae(void)
{
    fprintf(stderr, "proba: lineae vacuae consecutivae\n");
    speculum_t spec     = spec_solum();
    spec.lin_vacuae_max = 1;
    inspector_t ins;

    /* mundus: una vacua */
    curre_lintorem(
        "int a;\n"
        "\n"
        "int b;\n",
        &spec, &ins
    );
    expecta_mundum("una vacua", &ins);

    /* malum: duae vacuae */
    curre_lintorem(
        "int a;\n"
        "\n"
        "\n"
        "int b;\n",
        &spec, &ins
    );
    expecta("duae vacuae", &ins, "lineae_vacuae", 1);
}

static void proba_finis_lineae(void)
{
    fprintf(stderr, "proba: finis lineae novae\n");
    speculum_t spec     = spec_solum();
    spec.lin_finis_nova = 1;
    inspector_t ins;

    /* mundus */
    curre_lintorem(
        "int a;\n",
        &spec, &ins
    );
    expecta_mundum("finis cum \\n", &ins);

    /* malum */
    curre_lintorem(
        "int a;",
        &spec, &ins
    );
    expecta("finis sine \\n", &ins, "finis_lineae", 1);
}

static void proba_tabulae_mixtae(void)
{
    fprintf(stderr, "proba: tabulae mixtae\n");
    speculum_t spec         = spec_solum();
    spec.lin_tabulae_mixtae = 1;
    inspector_t ins;

    /* mundus: solum spatia intra functionem */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    int a;\n"
        "}\n",
        &spec, &ins
    );
    expecta_mundum("solum spatia", &ins);

    /* malum: mixta */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "\t int a;\n"
        "}\n",
        &spec, &ins
    );
    expecta("mixta", &ins, "tabulae_mixtae", 1);
}

/* ================================================================
 * probatio: speculum
 * ================================================================ */

static void proba_speculum_defaltum(void)
{
    fprintf(stderr, "proba: speculum defaltum\n");
    probationes_totae++;
    speculum_t spec = spec_defaltum();
    if (
        spec.ind_latitudo == 4 &&
        spec.ind_tabulis == 0 &&
        spec.bra_stilus == 0 &&
        spec.bra_else_coniunctum == 1 &&
        spec.bra_necessaria == 0 &&
        spec.spa_post_verba == 1 &&
        spec.spa_circa_operatores == 1 &&
        spec.lin_longitudo_max == 128 &&
        spec.lin_vacuae_max == 2
    ) {
        probationes_bonae++;
    } else {
        probationes_malae++;
        fprintf(stderr, "  MALUM: defalta non congruunt\n");
    }
}

/* ================================================================
 * probatio: lexator
 * ================================================================ */

static void proba_lexator(void)
{
    fprintf(stderr, "proba: lexator\n");
    lexator_t lex;
    const char *fons =
        "int main(void) {\n"
        "    /* commentarium */\n"
        "    return 0;\n"
        "}\n";

    probationes_totae++;
    if (
        lexator_disseca(&lex, fons, strlen(fons)) == 0 &&
        lex.num_signa > 0
    ) {

        /* primum signum debet esse VERBUM "int" */
        int bonum = 1;
        if (
            lex.signa[0].genus != SIGNUM_VERBUM ||
            lex.signa[0].longitudo != 3 ||
            memcmp(lex.signa[0].initium, "int", 3) != 0
        ) {
            bonum = 0;
            fprintf(stderr, "  MALUM: primum signum non est 'int'\n");
        }

        /* debet habere COMMENTARIUM */
        int habet_comm = 0;
        for (int i = 0; i < lex.num_signa; i++) {
            if (lex.signa[i].genus == SIGNUM_COMMENTARIUM)
                habet_comm = 1;
        }
        if (!habet_comm) {
            bonum = 0;
            fprintf(stderr, "  MALUM: commentarium non inventum\n");
        }

        /* ultimum non-FINIS debet esse LINEA_NOVA vel CLAUSIO */
        int ult = lex.num_signa - 2;
        if (ult >= 0 && lex.signa[ult].genus != SIGNUM_LINEA_NOVA) {
            bonum = 0;
            fprintf(stderr, "  MALUM: penultimum signum non est LINEA_NOVA\n");
        }

        if (bonum)
            probationes_bonae++;
        else
            probationes_malae++;

        lexator_purgare(&lex);
    } else {
        probationes_malae++;
        fprintf(stderr, "  MALUM: lexator_disseca fallit\n");
    }

    /* chordae et litterae */
    probationes_totae++;
    const char *fons2 = "char *s = \"hello\\n\";\nchar c = 'x';\n";
    if (lexator_disseca(&lex, fons2, strlen(fons2)) == 0) {
        int habet_chordam = 0, habet_litteram = 0;
        for (int i = 0; i < lex.num_signa; i++) {
            if (lex.signa[i].genus == SIGNUM_CHORDA)
                habet_chordam = 1;
            if (lex.signa[i].genus == SIGNUM_LITTERA)
                habet_litteram = 1;
        }
        if (habet_chordam && habet_litteram)
            probationes_bonae++;
        else {
            probationes_malae++;
            fprintf(stderr, "  MALUM: chorda vel littera non inventa\n");
        }
        lexator_purgare(&lex);
    } else {
        probationes_malae++;
    }

    /* praeprocessor */
    probationes_totae++;
    const char *fons3 = "#include <stdio.h>\nint x;\n";
    if (lexator_disseca(&lex, fons3, strlen(fons3)) == 0) {
        int habet_pp = 0;
        for (int i = 0; i < lex.num_signa; i++) {
            if (lex.signa[i].genus == SIGNUM_PRAEPROCESSOR)
                habet_pp = 1;
        }
        if (habet_pp)
            probationes_bonae++;
        else {
            probationes_malae++;
            fprintf(stderr, "  MALUM: praeprocessor non inventus\n");
        }
        lexator_purgare(&lex);
    } else {
        probationes_malae++;
    }

    /* operatores multi-characteres */
    probationes_totae++;
    const char *fons4 = "a == b && c != d || e->f\n";
    if (lexator_disseca(&lex, fons4, strlen(fons4)) == 0) {
        int habet_eq = 0, habet_and = 0, habet_neq = 0;
        int habet_or = 0, habet_arrow = 0;
        for (int i = 0; i < lex.num_signa; i++) {
            if (lex.signa[i].genus != SIGNUM_OPERATOR)
                continue;
            if (lex.signa[i].longitudo == 2) {
                if (memcmp(lex.signa[i].initium, "==", 2) == 0)
                    habet_eq = 1;
                if (memcmp(lex.signa[i].initium, "&&", 2) == 0)
                    habet_and = 1;
                if (memcmp(lex.signa[i].initium, "!=", 2) == 0)
                    habet_neq = 1;
                if (memcmp(lex.signa[i].initium, "||", 2) == 0)
                    habet_or = 1;
                if (memcmp(lex.signa[i].initium, "->", 2) == 0)
                    habet_arrow = 1;
            }
        }
        if (habet_eq && habet_and && habet_neq && habet_or && habet_arrow)
            probationes_bonae++;
        else {
            probationes_malae++;
            fprintf(stderr, "  MALUM: operatores multi non omnes inventi\n");
        }
        lexator_purgare(&lex);
    } else {
        probationes_malae++;
    }
}

/* ================================================================
 * probatio: codex mundus nullum monitum generat
 * ================================================================ */

static void proba_codex_mundus(void)
{
    fprintf(stderr, "proba: codex mundus (omnes regulae activae)\n");
    speculum_t spec        = spec_defaltum();
    spec.lin_longitudo_max = 120;  /* permitte lineas longiores */
    inspector_t ins;

    curre_lintorem(
        "#include <stdio.h>\n"
        "\n"
        "int main(void)\n"
        "{\n"
        "    int a = 1;\n"
        "    int b = 2;\n"
        "\n"
        "    if (a == b) {\n"
        "        printf(\"pares\\n\");\n"
        "    } else {\n"
        "        printf(\"impares\\n\");\n"
        "    }\n"
        "\n"
        "    for (int i = 0; i < 10; i++) {\n"
        "        a += i;\n"
        "    }\n"
        "\n"
        "    while (a > 0) {\n"
        "        a--;\n"
        "    }\n"
        "\n"
        "    switch (b) {\n"
        "    case 1:\n"
        "        a = 10;\n"
        "        break;\n"
        "    default:\n"
        "        a = 20;\n"
        "        break;\n"
        "    }\n"
        "\n"
        "    return 0;\n"
        "}\n",
        &spec, &ins
    );
    expecta_mundum("codex mundus integer", &ins);
}

/* ================================================================
 * probationes: commentaria vetita
 * ================================================================ */

static void proba_commentaria_vetita(void)
{
    fprintf(stderr, "proba: commentaria vetita\n");
    speculum_t spec = spec_solum();
    spec.com_veta   = 1;
    inspector_t ins;

    /* mundus: nullum commentarium */
    curre_lintorem(
        "int main(void)\n"
        "{\n"
        "    return 0;\n"
        "}\n",
        &spec, &ins
    );
    expecta_mundum("sine commentariis", &ins);

    /* commentarium lineare */
    curre_lintorem(
        "int x = 5; // numerus\n",
        &spec, &ins
    );
    expecta("// lineare", &ins, "commentaria_vetita", 1);

    /* commentarium clausum */
    curre_lintorem(
        "int x = 5; /* numerus */\n",
        &spec, &ins
    );
    expecta("/* clausum */", &ins, "commentaria_vetita", 1);

    /* commentarium multi-lineare — unum signum */
    curre_lintorem(
        "/*\n"
        " * multi\n"
        " */\n"
        "int x;\n",
        &spec, &ins
    );
    expecta("multi-lineare", &ins, "commentaria_vetita", 1);

    /* plura commentaria */
    curre_lintorem(
        "/* primum */\n"
        "int x; // secundum\n"
        "/* tertium */\n",
        &spec, &ins
    );
    expecta("plura commentaria", &ins, "commentaria_vetita", 3);

    /* inactivum: com_veta = 0 */
    spec.com_veta = 0;
    curre_lintorem(
        "int x; // commentarium\n",
        &spec, &ins
    );
    expecta_mundum("inactivum", &ins);
}

/* ================================================================
 * probationes: correctio commentariorum
 * ================================================================ */

static const char *via_temp = "/tmp/insinulint_proba_com.c";

/* scribe chordam in plicam temporalem */
static void scribe_temp(const char *fons)
{
    FILE *f = fopen(via_temp, "wb");
    if (!f)
        return;
    fwrite(fons, 1, strlen(fons), f);
    fclose(f);
}

/* lege plicam temporalem in memoriam (vocans liberet) */
static char *lege_temp(void)
{
    FILE *f = fopen(via_temp, "rb");
    if (!f)
        return NULL;
    fseek(f, 0, SEEK_END);
    long lon = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc((size_t)lon + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }
    fread(buf, 1, (size_t)lon, f);
    buf[lon] = '\0';
    fclose(f);
    return buf;
}

/* numera commentaria in fonte per lexatorem */
static int numera_commentaria(const char *fons)
{
    lexator_t lex;
    if (lexator_disseca(&lex, fons, strlen(fons)) < 0)
        return -1;
    int n = 0;
    for (int i = 0; i < lex.num_signa; i++) {
        if (lex.signa[i].genus == SIGNUM_COMMENTARIUM)
            n++;
    }
    lexator_purgare(&lex);
    return n;
}

static void proba_correctio_excommenta(void)
{
    fprintf(stderr, "proba: correctio excommenta\n");

    const char *fons =
        "/* caput */\n"
        "#include <stdio.h>\n"
        "\n"
        "int main(void)\n"
        "{\n"
        "    int x = 5;  // numerus\n"
        "    /* commentarium internum */\n"
        "    printf(\"%d\\n\", x);\n"
        "    return 0;\n"
        "}\n";

    /* correctio prima */
    scribe_temp(fons);
    correctio_excommenta(via_temp);
    char *post1 = lege_temp();

    /* nulla commentaria remanent */
    probationes_totae++;
    int com = numera_commentaria(post1);
    if (com == 0) {
        probationes_bonae++;
    } else {
        probationes_malae++;
        fprintf(
            stderr, "  MALUM: %d commentaria remanent post correctionem\n",
            com
        );
    }

    /* idempotentia: F(F(x)) = F(x) */
    correctio_excommenta(via_temp);
    char *post2 = lege_temp();

    probationes_totae++;
    if (strcmp(post1, post2) == 0) {
        probationes_bonae++;
    } else {
        probationes_malae++;
        fprintf(
            stderr, "  MALUM: correctio non idempotens\n"
            "    post 1: [%s]\n    post 2: [%s]\n",
            post1, post2
        );
    }

    free(post1);
    free(post2);
    remove(via_temp);
}

/* ================================================================
 * probationes: corpus separatum
 * ================================================================ */

static const char *via_temp_corp = "/tmp/insinulint_proba_corp.c";

static void proba_corpus_separatum(void)
{
    fprintf(stderr, "proba: corpus separatum\n");
    speculum_t spec    = spec_solum();
    spec.cor_separatum = 1;
    inspector_t ins;

    /* corpus in eadem linea — flagellandum */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    if (x) return;\n"
        "}\n",
        &spec, &ins
    );
    expecta("if inline", &ins, "corpus_separatum", 1);

    /* corpus in linea propria — mundum */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    if (x)\n"
        "        return;\n"
        "}\n",
        &spec, &ins
    );
    expecta("if separatum", &ins, "corpus_separatum", 0);

    /* for inline */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    for (int i = 0; i < n; i++) sum += i;\n"
        "}\n",
        &spec, &ins
    );
    expecta("for inline", &ins, "corpus_separatum", 1);

    /* while inline */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    while (p) p = p->next;\n"
        "}\n",
        &spec, &ins
    );
    expecta("while inline", &ins, "corpus_separatum", 1);

    /* else inline */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    if (x) {\n"
        "        a;\n"
        "    } else b;\n"
        "}\n",
        &spec, &ins
    );
    expecta("else inline", &ins, "corpus_separatum", 1);

    /* else if — non flagellandum */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    if (x) {\n"
        "        a;\n"
        "    } else if (y) {\n"
        "        b;\n"
        "    }\n"
        "}\n",
        &spec, &ins
    );
    expecta("else if", &ins, "corpus_separatum", 0);

    /* do-while: while non flagellandum */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    do {\n"
        "        a;\n"
        "    } while (x);\n"
        "}\n",
        &spec, &ins
    );
    expecta("do-while", &ins, "corpus_separatum", 0);

    /* corpus cum bracchiis — non flagellandum */
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    if (x) { return; }\n"
        "}\n",
        &spec, &ins
    );
    expecta("bracchia", &ins, "corpus_separatum", 0);

    /* regula inactiva — non flagellandum */
    speculum_t spec2    = spec_solum();
    spec2.cor_separatum = 0;
    curre_lintorem(
        "void f(void)\n"
        "{\n"
        "    if (x) return;\n"
        "}\n",
        &spec2, &ins
    );
    expecta("inactiva", &ins, "corpus_separatum", 0);
}

static void proba_correctio_corpus_separatum(void)
{
    fprintf(stderr, "proba: correctio corpus separatum\n");
    speculum_t spec;
    speculum_lege(&spec, NULL);
    spec.cor_separatum = 1;

    const char *fons =
        "void f(void)\n"
        "{\n"
        "    if (x) return;\n"
        "    for (int i = 0; i < n; i++) sum += i;\n"
        "}\n";

    const char *expectatum =
        "void f(void)\n"
        "{\n"
        "    if (x)\n"
        "        return;\n"
        "    for (int i = 0; i < n; i++)\n"
        "        sum += i;\n"
        "}\n";

    FILE *f = fopen(via_temp_corp, "wb");
    if (!f)
        return;
    fwrite(fons, 1, strlen(fons), f);
    fclose(f);

    correctio_age(via_temp_corp, &spec);

    f = fopen(via_temp_corp, "rb");
    if (!f)
        return;
    fseek(f, 0, SEEK_END);
    long lon = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *res = malloc((size_t)lon + 1);
    if (!res) {
        fclose(f);
        return;
    }
    fread(res, 1, (size_t)lon, f);
    res[lon] = '\0';
    fclose(f);

    probationes_totae++;
    if (strcmp(res, expectatum) == 0) {
        probationes_bonae++;
    } else {
        probationes_malae++;
        fprintf(
            stderr, "  MALUM: correctio corpus_separatum\n"
            "    expectatum:\n%s"
            "    inventum:\n%s",
            expectatum, res
        );
    }

    free(res);
    remove(via_temp_corp);
}

/* ================================================================
 * probatio: commendatio
 * ================================================================ */

static void proba_commendatio(void)
{
    fprintf(stderr, "proba: commendatio\n");
    speculum_t spec     = spec_solum();
    spec.spa_post_verba = 1;

    const char *fons =
        "int main(void)\n"
        "{\n"
        "    if(x) return 1;\n"
        "    return 0;\n"
        "}\n";

    lexator_t lex;
    lexator_disseca(&lex, fons, strlen(fons));
    inspector_t ins;
    inspector_initia(&ins, "proba.c");
    inspice_omnia(&ins, &lex, &spec);
    lexator_purgare(&lex);

    /* debet habere monitum in linea 3 */
    probationes_totae++;
    if (ins.num_monita > 0 && ins.monita[0].linea == 3) {
        probationes_bonae++;
    } else {
        probationes_malae++;
        fprintf(stderr, "  MALUM: commendatio — monitum non in linea 3\n");
        return;
    }

    /* commendatio scribit ad stdout — capimus in alveo */
    /* verifica structuram: redirigimus stdout ad plicam */
    FILE *antea = stdout;
    FILE *f     = fopen("/tmp/proba_commendatio.txt", "w");
    if (!f) {
        probationes_malae++;
        fprintf(stderr, "  MALUM: non possum aperire plicam temporariam\n");
        return;
    }
    stdout = f;
    commendatio_scribe(&ins, fons, &spec, 1);
    fclose(f);
    stdout = antea;

    /* lege resultatum */
    char *res = ison_lege_plicam("/tmp/proba_commendatio.txt");
    remove("/tmp/proba_commendatio.txt");
    if (!res) {
        probationes_malae++;
        fprintf(stderr, "  MALUM: non possum legere resultatum\n");
        return;
    }

    /* verifica: caput continet 'proba.c:3' et '>' notationem */
    probationes_totae++;
    if (strstr(res, "proba.c:3:") && strstr(res, ">")) {
        probationes_bonae++;
    } else {
        probationes_malae++;
        fprintf(stderr, "  MALUM: commendatio forma — caput vel '>' deest\n");
        fprintf(stderr, "  inventum:\n%s", res);
    }

    free(res);
}

/* ================================================================
 * probatio: propositio
 * ================================================================ */

static void proba_propositio(void)
{
    fprintf(stderr, "proba: propositio\n");
    speculum_t spec     = spec_solum();
    spec.spa_post_verba = 1;

    const char *fons =
        "int main(void)\n"
        "{\n"
        "    if(x) return 1;\n"
        "    return 0;\n"
        "}\n";

    lexator_t lex;
    lexator_disseca(&lex, fons, strlen(fons));
    inspector_t ins;
    inspector_initia(&ins, "proba.c");
    inspice_omnia(&ins, &lex, &spec);
    lexator_purgare(&lex);

    /* construi proposita */
    propositum_t prop[16];
    int np = propositio_ex_inspectore(&ins, fons, &spec, 3, prop, 16);

    probationes_totae++;
    if (np > 0) {
        probationes_bonae++;
    } else {
        probationes_malae++;
        fprintf(stderr, "  MALUM: propositio — nulla proposita\n");
        return;
    }

    /* scribe ddiff ad plicam */
    FILE *antea = stdout;
    FILE *f     = fopen("/tmp/proba_propositio.ddiff", "w");
    if (!f) {
        probationes_malae++;
        fprintf(stderr, "  MALUM: non possum aperire plicam temporariam\n");
        return;
    }
    stdout = f;
    int nm = propositio_scribe(prop, np);
    fclose(f);
    stdout = antea;

    /* verifica: ddiff productus */
    probationes_totae++;
    if (nm > 0) {
        probationes_bonae++;
    } else {
        probationes_malae++;
        fprintf(stderr, "  MALUM: propositio — nullae mutationes\n");
        remove("/tmp/proba_propositio.ddiff");
        return;
    }

    /* verifica formam ddiff */
    char *res = ison_lege_plicam("/tmp/proba_propositio.ddiff");
    remove("/tmp/proba_propositio.ddiff");
    if (!res) {
        probationes_malae++;
        fprintf(stderr, "  MALUM: non possum legere ddiff\n");
        return;
    }

    probationes_totae++;
    if (
        strstr(res, "ddiff versio I") &&
        strstr(res, "PLICAE") &&
        strstr(res, "MUTATIONES") &&
        strstr(res, "if(x)") &&
        strstr(res, "if (x)")
    ) {
        probationes_bonae++;
    } else {
        probationes_malae++;
        fprintf(stderr, "  MALUM: propositio ddiff — forma non valida\n");
        fprintf(stderr, "  inventum:\n%s", res);
    }

    free(res);
}

/* ================================================================
 * principale
 * ================================================================ */

int main(void)
{
    fprintf(stderr, "\n=== insinulint probationes ===\n\n");

    /* lexator */
    proba_lexator();

    /* speculum */
    proba_speculum_defaltum();

    /* indentatio */
    proba_indentatio_simplex();
    proba_indentatio_case();
    proba_indentatio_virtualis();
    proba_continuatio_congruens();
    proba_continuatio_massa();
    proba_continuatio_pendens();
    proba_continuatio_patens();

    /* spatia */
    proba_spatia_verba();
    proba_spatia_operatores();
    proba_spatia_virgulae();
    proba_spatia_semicolona();

    /* bracchia */
    proba_bracchia_necessaria();
    proba_cubitus();
    proba_bracchia_stilus();

    /* lineae */
    proba_spatia_terminalia();
    proba_longitudo_lineae();
    proba_lineae_vacuae();
    proba_finis_lineae();
    proba_tabulae_mixtae();

    /* commentaria */
    proba_commentaria_vetita();
    proba_correctio_excommenta();

    /* corpus separatum */
    proba_corpus_separatum();
    proba_correctio_corpus_separatum();

    /* codex mundus */
    proba_codex_mundus();

    /* commendatio */
    proba_commendatio();

    /* propositio */
    proba_propositio();

    /* summa */
    fprintf(stderr, "\n=== summa ===\n");
    fprintf(
        stderr, "probationes: %d totae, %d bonae, %d malae\n\n",
        probationes_totae, probationes_bonae, probationes_malae
    );

    return probationes_malae > 0 ? 1 : 0;
}
