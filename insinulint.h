/*
 * insinulint.h — caput principale lintoris linguae C
 *
 * Solum C purum tractamus — sine C++ sive extensionibus novis.
 */

#ifndef INSINULINT_H
#define INSINULINT_H

#include <stddef.h>

/* ================================================================
 * signa (tokens) — lexator
 * ================================================================ */

typedef enum {
    SIGNUM_FINIS = 0,       /* finis fasciculi */
    SIGNUM_SPATIUM,         /* spatia et tabulae */
    SIGNUM_LINEA_NOVA,      /* '\n' */
    SIGNUM_COMMENTARIUM,    /* commentarium */
    SIGNUM_PRAEPROCESSOR,   /* #include, #define, etc. */
    SIGNUM_CHORDA,          /* "..." littera chordae */
    SIGNUM_LITTERA,         /* '.' littera characteris */
    SIGNUM_APERTIO,         /* { */
    SIGNUM_CLAUSIO,         /* } */
    SIGNUM_APERTIO_PAR,     /* ( */
    SIGNUM_CLAUSIO_PAR,     /* ) */
    SIGNUM_APERTIO_QUAD,    /* [ */
    SIGNUM_CLAUSIO_QUAD,    /* ] */
    SIGNUM_SEMICOLON,       /* ; */
    SIGNUM_VERBUM,          /* identificator vel verbum clavium */
    SIGNUM_NUMERUS,         /* littera numerica */
    SIGNUM_OPERATOR,        /* +, -, *, /, etc. */
    SIGNUM_ALIUD            /* cetera */
} signum_genus_t;

typedef struct {
    signum_genus_t genus;
    int            linea;       /* numerus lineae (ab 1) */
    int            columna;     /* columna (ab 0) */
    const char    *initium;     /* index in fonte */
    int            longitudo;   /* longitudo signi in bytes */
} signum_t;

/* capacitas maxima signorum */
#define SIGNA_MAX  (1 << 20)

/* ================================================================
 * lexator — dissector fontis C
 * ================================================================ */

typedef struct {
    const char *fons;           /* fons integer */
    size_t      fons_lon;       /* longitudo fontis */
    signum_t   *signa;          /* tabula signorum */
    int         num_signa;      /* numerus signorum */
    int         capacitas;      /* capacitas tabulae */
} lexator_t;

int lexator_disseca(lexator_t *lex, const char *fons, size_t lon);
void lexator_purgare(lexator_t *lex);

/* ================================================================
 * monitum — erratum vel admonitio
 * ================================================================ */

#define NUNTIUS_MAX 256
#define VIA_MAX     512

typedef enum {
    GRAVITAS_MONITUM = 0,   /* admonitio levis */
    GRAVITAS_ERRATUM         /* erratum grave */
} gravitas_t;

typedef struct {
    gravitas_t  gravitas;
    int         linea;
    int         columna;
    char        nuntius[NUNTIUS_MAX];
    char        regula[64];
    int         fix_valor;      /* -1 = non fixabile; >= 0 = spatia */
    int         split_columna;  /* -1 = non; >= 0 = columna ')' */
    int         apert_columna;  /* -1 = non; >= 0 = columna '(' */
    int         bra_columna;    /* -1 = non; >= 0 = columna '{' */
} monitum_t;

/* ================================================================
 * inspector — collectio monitorum
 * ================================================================ */

#define MONITA_MAX 4096

typedef struct {
    monitum_t  monita[MONITA_MAX];
    int        num_monita;
    const char *via_fasciculi;
} inspector_t;

void inspector_initia(inspector_t *ins, const char *via);
void inspector_adde(
    inspector_t *ins, gravitas_t g, int linea, int columna,
    const char *regula, const char *nuntius
);
int  inspector_scribe(const inspector_t *ins);

/* ================================================================
 * speculum — configuratio ex fasciculo JSON
 * ================================================================ */

typedef struct {
    /* indentatio */
    int ind_latitudo;           /* spatia per gradum (defaltum: 4) */
    int ind_tabulis;            /* 1 = tabulae, 0 = spatia */
    int ind_continuatio;        /* continuatio_t (vide inspectiones/indentatio.h) */

    /* bracchia */
    int bra_stilus;             /* 0 = kr (1TBS), 1 = allman */
    int bra_else_coniunctum;    /* 1 = } else { in eadem linea */
    int bra_necessaria;         /* 1 = bracchia necessaria post if/for/while */

    /* spatia */
    int spa_post_verba;         /* 1 = spatium post if/for/while/switch */
    int spa_circa_operatores;   /* 1 = spatia circa operatores binarios */
    int spa_post_virgulam;      /* 1 = spatium post virgulam */
    int spa_ante_semicolon;           /* 1 = veta spatium ante ; */
    int spa_adtributio_colineata;     /* 1 = colinea '=' in adtributionibus */

    /* lineae */
    int lin_longitudo_max;      /* longitudo maxima lineae (0 = sine limite) */
    int lin_spatia_terminalia;  /* 1 = veta spatia terminalia */
    int lin_vacuae_max;         /* lineae vacuae consecutivae maximae */
    int lin_finis_nova;         /* 1 = fasciculus debet finire cum \n */
    int lin_tabulae_mixtae;     /* 1 = veta tabulas et spatia mixtas */
    int lin_una_sententia;     /* 1 = veta plures sententias in eadem linea */

    /* corpus */
    int cor_separatum;          /* 1 = corpus unius sententiae in linea propria */

    /* commentaria */
    int com_veta;               /* 1 = veta omnia commentaria */
} speculum_t;

int speculum_lege(speculum_t *spec, const char *via);

/* ================================================================
 * inspectio integra
 * ================================================================ */

void inspice_omnia(
    inspector_t *ins, const lexator_t *lex,
    const speculum_t *spec
);

/*
 * insinulint_lege_inspice — lege plicam, lexa, inspice.
 * ins popula; *fons_out adtribuitur (vocans per free() liberat).
 * reddit 0 si bene, -1 si error.
 */
int insinulint_lege_inspice(
    const char *via, const speculum_t *spec,
    inspector_t *ins, char **fons_out
);

int insinulint_inspice(const char *via_fontis, const speculum_t *spec);

/* ================================================================
 * correctio — correctio plicarum in loco
 * ================================================================ */

/*
 * correctio_age — corriges plicam in loco ex speculo.
 * reddit 0 si bene, -1 si error.
 */
int correctio_age(const char *via, const speculum_t *spec);

/*
 * correctio_excommenta — exstirpat omnia commentaria ex plica.
 * reddit 0 si bene, -1 si error.
 */
int correctio_excommenta(const char *via);

#endif /* INSINULINT_H */
