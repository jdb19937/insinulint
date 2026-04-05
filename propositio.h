/*
 * propositio.h — propositiones correctionum in forma ddiff
 *
 * Pro quoque monito, proponit correctionem et scribit
 * in forma ddiff ad stdout. Potest operari ex inspectore
 * (inline) vel ex forma commendationis in stdin.
 */

#ifndef PROPOSITIO_H
#define PROPOSITIO_H

#include "insinulint.h"

#define PROP_LINEA_MAX 2048
#define PROP_FEN_MAX   16
#define PROPOSITA_MAX  4096

/* linea fontis cum numero */
typedef struct {
    int  numerus;
    char textus[PROP_LINEA_MAX];
    int  lon;
} prop_linea_t;

/* monitum cum fenestra contextus et metadatis */
typedef struct {
    char         via[VIA_MAX];
    int          linea;
    int          columna;
    char         regula[64];
    char         nuntius[NUNTIUS_MAX];
    prop_linea_t fen[PROP_FEN_MAX];
    int          num_fen;
    int          idx_centri;    /* index lineae violationis in fen[] */

    /* metadata ex monitum_t */
    int          fix_valor;     /* -1 = non fixabile; >= 0 = spatia */
    int          split_columna; /* -1 = non; >= 0 = columna ')' */
    int          apert_columna; /* -1 = non; >= 0 = columna '(' */
    int          bra_columna;   /* -1 = non; >= 0 = columna '{' */

    /* metadata ex speculum_t (config) */
    int          ind_latitudo;  /* spatia per gradum */
    int          ind_tabulis;   /* 1 = tabulae, 0 = spatia */
    int          bra_stilus;    /* 0 = kr, 1 = allman */
    int          lin_longitudo_max; /* longitudo maxima lineae */
} propositum_t;

/* eventus propositionis: lineae veteres consumptae et novae productae */
#define PROP_NOVI_MAX 16

typedef struct {
    int idx_ab;     /* primus index in fen[] consumptus */
    int idx_ad;     /* ultimus index in fen[] consumptus (inclusivus) */
    struct {
        int  numerus;
        char textus[PROP_LINEA_MAX];
        int  lon;
    } novi[PROP_NOVI_MAX];
    int num_novi;
} prop_eventus_t;

/* signum functionis propositionis */
typedef int (*propone_fn)(
    const propositum_t *prop,
    prop_eventus_t *ev
);

/*
 * propositio_ex_inspectore — construi proposita ex inspectore et fonte.
 * reddit numerum propositorum.
 */
int propositio_ex_inspectore(
    const inspector_t *ins, const char *fons,
    const speculum_t *spec,
    int ala, propositum_t *dest, int max
);

/*
 * propositio_ex_stdin — lege proposita ex stdin (forma commendationis).
 * reddit numerum propositorum.
 */
int propositio_ex_stdin(propositum_t *dest, int max);

/*
 * propositio_scribe — applica correctiones et scribe ddiff ad stdout.
 * reddit numerum mutationum scriptarum.
 */
int propositio_scribe(const propositum_t *prop, int num);

#endif /* PROPOSITIO_H */
