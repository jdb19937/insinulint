/*
 * inspectiones/auxiliaria.c — functiones auxiliares communes
 *
 * versus, quaestiones signorum, adde_fix et similia.
 */

#include "../inspectio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================================================================
 * versus
 * ================================================================ */

int aedifica_versus(const lexator_t *lex, versus_t *vv, int max)
{
    int nv = 0;
    int i  = 0;
    int n  = lex->num_signa;
    const signum_t *s = lex->signa;

    while (i < n && nv < max) {
        versus_t *v    = &vv[nv];
        v->tok_initium = i;

        /* inveni finem lineae */
        int j = i;
        while (
            j < n && s[j].genus != SIGNUM_LINEA_NOVA &&
            s[j].genus != SIGNUM_FINIS
        )
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
int versus_vacuus(const versus_t *v)
{
    return v->tok_primus >= v->tok_finis;
}

/* ================================================================
 * quaestiones signorum
 * ================================================================ */

int est_verbum(const signum_t *s, const char *verbum)
{
    int lon = (int)strlen(verbum);
    return s->genus == SIGNUM_VERBUM &&
        s->longitudo == lon &&
        memcmp(s->initium, verbum, (size_t)lon) == 0;
}

int est_operator(const signum_t *s, const char *op)
{
    int lon = (int)strlen(op);
    return s->genus == SIGNUM_OPERATOR &&
        s->longitudo == lon &&
        memcmp(s->initium, op, (size_t)lon) == 0;
}

int est_verbum_cum_spatio(const signum_t *s)
{
    return est_verbum(s, "if") || est_verbum(s, "for") ||
        est_verbum(s, "while") || est_verbum(s, "switch") ||
        est_verbum(s, "return") || est_verbum(s, "case");
}

int est_operator_binarius(const signum_t *s)
{
    if (s->genus != SIGNUM_OPERATOR)
        return 0;
    if (
        est_operator(s, "==") || est_operator(s, "!=") ||
        est_operator(s, "<=") || est_operator(s, ">=") ||
        est_operator(s, "&&") || est_operator(s, "||") ||
        est_operator(s, "+=") || est_operator(s, "-=") ||
        est_operator(s, "*=") || est_operator(s, "/=") ||
        est_operator(s, "%=") || est_operator(s, "&=") ||
        est_operator(s, "|=") || est_operator(s, "^=") ||
        est_operator(s, "<<=") || est_operator(s, ">>=")
    )
        return 1;
    if (s->longitudo == 1 && s->initium[0] == '=')
        return 1;
    return 0;
}

/* ================================================================
 * navigatio signorum
 * ================================================================ */

int quaere_par_apertam(const signum_t *signa, int idx)
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

int quaere_par_clausam(const signum_t *signa, int n, int idx)
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

int prox_significans(const signum_t *signa, int n, int idx)
{
    for (int i = idx; i < n; i++) {
        if (
            signa[i].genus != SIGNUM_SPATIUM &&
            signa[i].genus != SIGNUM_LINEA_NOVA &&
            signa[i].genus != SIGNUM_COMMENTARIUM
        )
            return i;
    }
    return n;
}

int prox_significans_retro(const signum_t *signa, int idx)
{
    for (int i = idx; i >= 0; i--) {
        if (
            signa[i].genus != SIGNUM_SPATIUM &&
            signa[i].genus != SIGNUM_LINEA_NOVA &&
            signa[i].genus != SIGNUM_COMMENTARIUM
        )
            return i;
    }
    return -1;
}

int habet_lineam_novam(const signum_t *signa, int a, int b)
{
    for (int i = a; i <= b; i++) {
        if (signa[i].genus == SIGNUM_LINEA_NOVA)
            return 1;
    }
    return 0;
}

int ultimus_significans(
    const signum_t *signa, int initium, int finis
) {
    for (int j = finis - 1; j >= initium; j--) {
        if (
            signa[j].genus != SIGNUM_SPATIUM &&
            signa[j].genus != SIGNUM_COMMENTARIUM
        )
            return j;
    }
    return -1;
}

int linea_habet_bracchium(
    const signum_t *signa, int initium, int finis
) {
    for (int j = initium; j < finis; j++) {
        if (signa[j].genus == SIGNUM_APERTIO)
            return 1;
    }
    return 0;
}

/* ================================================================
 * adde monitum cum correctione
 * ================================================================ */

void adde_fix(
    inspector_t *ins, gravitas_t g, int linea, int columna,
    const char *regula, const char *nuntius, int fix_valor
) {
    int pre = ins->num_monita;
    inspector_adde(ins, g, linea, columna, regula, nuntius);
    if (ins->num_monita > pre)
        ins->monita[pre].fix_valor = fix_valor;
}

void adde_scissio(
    inspector_t *ins, gravitas_t g, int linea,
    int columna, const char *regula, const char *nuntius,
    int ind_novi, int split_col
) {
    int pre = ins->num_monita;
    inspector_adde(ins, g, linea, columna, regula, nuntius);
    if (ins->num_monita > pre) {
        ins->monita[pre].fix_valor     = ind_novi;
        ins->monita[pre].split_columna = split_col;
    }
}

void adde_apert(
    inspector_t *ins, gravitas_t g, int linea,
    int columna, const char *regula, const char *nuntius,
    int ind_cont, int apert_col
) {
    int pre = ins->num_monita;
    inspector_adde(ins, g, linea, columna, regula, nuntius);
    if (ins->num_monita > pre) {
        ins->monita[pre].fix_valor     = ind_cont;
        ins->monita[pre].apert_columna = apert_col;
    }
}

void adde_bracchia(
    inspector_t *ins, gravitas_t g, int linea, int columna,
    const char *regula, const char *nuntius,
    int fix_valor, int bra_col
) {
    int pre = ins->num_monita;
    inspector_adde(ins, g, linea, columna, regula, nuntius);
    if (ins->num_monita > pre) {
        ins->monita[pre].fix_valor   = fix_valor;
        ins->monita[pre].bra_columna = bra_col;
    }
}
