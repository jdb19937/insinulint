/*
 * inspectio.h — caput internum inspectionum
 *
 * Typi, auxiliaria, et declarationes communes omnium inspectionum.
 */

#ifndef INSPECTIO_H
#define INSPECTIO_H

#include "insinulint.h"

#include <string.h>

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

int aedifica_versus(const lexator_t *lex, versus_t *vv, int max);
int versus_vacuus(const versus_t *v);

/* ================================================================
 * auxiliaria communia
 * ================================================================ */

int est_verbum(const signum_t *s, const char *verbum);
int est_operator(const signum_t *s, const char *op);
int est_verbum_cum_spatio(const signum_t *s);
int est_operator_binarius(const signum_t *s);

int quaere_par_apertam(const signum_t *signa, int idx);
int quaere_par_clausam(const signum_t *signa, int n, int idx);
int prox_significans(const signum_t *signa, int n, int idx);
int prox_significans_retro(const signum_t *signa, int idx);
int habet_lineam_novam(const signum_t *signa, int a, int b);
int ultimus_significans(
    const signum_t *signa, int initium, int finis
);
int linea_habet_bracchium(
    const signum_t *signa, int initium, int finis
);

/* ================================================================
 * adde monitum cum correctione
 * ================================================================ */

void adde_fix(
    inspector_t *ins, gravitas_t g, int linea, int columna,
    const char *regula, const char *nuntius, int fix_valor
);
void adde_scissio(
    inspector_t *ins, gravitas_t g, int linea,
    int columna, const char *regula, const char *nuntius,
    int ind_novi, int split_col
);
void adde_apert(
    inspector_t *ins, gravitas_t g, int linea,
    int columna, const char *regula, const char *nuntius,
    int ind_cont, int apert_col
);
void adde_bracchia(
    inspector_t *ins, gravitas_t g, int linea, int columna,
    const char *regula, const char *nuntius,
    int fix_valor, int bra_col
);

/* ================================================================
 * inspectiones — una functio per regulam
 * ================================================================ */

void inspice_indentationem(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus,
    const speculum_t *spec
);

void inspice_spatia_terminalia(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus
);

void inspice_longitudinem(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus,
    int maxima, int lat
);

void inspice_lineas_vacuas(
    inspector_t *ins,
    const versus_t *versus, int num_versus,
    int maxima
);

void inspice_finem(inspector_t *ins, const lexator_t *lex);

void inspice_tabulas_mixtas(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus
);

void inspice_spatia_verborum(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus
);

void inspice_spatia_operatorum(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus
);

void inspice_virgulas(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus
);

void inspice_semicolona(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus
);

void inspice_cubitum(
    inspector_t *ins, const lexator_t *lex,
    const speculum_t *spec
);

void inspice_bracchia_stilum(
    inspector_t *ins, const lexator_t *lex,
    const speculum_t *spec
);

void inspice_bracchia_necessaria(
    inspector_t *ins, const lexator_t *lex
);

void inspice_adtributiones_colineatas(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus
);

void inspice_commentaria(
    inspector_t *ins, const lexator_t *lex
);

void inspice_corpus_separatum(
    inspector_t *ins, const lexator_t *lex,
    const speculum_t *spec
);

void inspice_unam_sententiam(
    inspector_t *ins, const lexator_t *lex,
    const versus_t *versus, int num_versus
);

#endif /* INSPECTIO_H */
