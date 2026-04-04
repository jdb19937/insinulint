/*
 * correctio.h — caput correctionum
 *
 * Interfacies publica et typi interni correctionum.
 */

#ifndef CORRECTIO_H
#define CORRECTIO_H

#include "insinulint.h"

#include <string.h>

/* ================================================================
 * interfacies publica
 * ================================================================ */

int correctio_age(const char *via, const speculum_t *spec);
int correctio_excommenta(const char *via);

/* ================================================================
 * linea — portio fontis inter '\n'
 * ================================================================ */

typedef struct {
    const char *initium;
    int         lon;
} linea_t;

int scinde_in_lineas(
    const char *fons, size_t fons_lon,
    linea_t *lineae, int max_lineae
);
int linea_vacua(const linea_t *l);

/* converte columnam (tab=8) ad byte offset in linea */
int columna_ad_byte(const char *initium, int lon, int columna);

/* scribe indentationem per spatia vel tabulas secundum speculum */
char *scribe_indentationem(char *wp, int spatia, const speculum_t *spec);

/* ================================================================
 * correctiones linearum
 * ================================================================ */

char *corrige_corpus(
    char *wp, const char *corpus, int corp_lon,
    int col, int ind, const speculum_t *spec
);

char *corrige_apertionem(
    char *wp, const char *corpus, int corp_lon,
    int col, int ind, int sp_init, const speculum_t *spec
);

char *corrige_scissionem(
    char *wp, const char *corpus, int corp_lon,
    int col, int ind, int sp_init,
    const linea_t *l, const linea_t *lineae,
    int i, int nlin, int *transili,
    const speculum_t *spec
);

char *corrige_colineationem(
    char *wp, const char *corpus, int corp_lon,
    int col, int spa, int sp_init
);

char *corrige_bracchia_kr(
    char *wp, const char *corpus, int corp_lon,
    const linea_t *lineae, int i, int nlin,
    int *transili, const speculum_t *spec
);

char *corrige_bracchia_allman(
    char *wp, const char *corpus, int corp_lon,
    int col, int ind, const speculum_t *spec
);

char *corrige_unam_sententiam(
    char *wp, const char *corpus, int corp_lon,
    int ind, const speculum_t *spec
);

#endif /* CORRECTIO_H */
