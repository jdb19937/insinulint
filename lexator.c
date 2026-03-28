/*
 * lexator.c — dissector fontis linguae C in signa
 *
 * Dissecamus fontem purum C (K&R, C89, C99) in signa discreta.
 * Nulla extensio C++ vel GNU hic tractatur.
 */

#include "insinulint.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ----------------------------------------------------------------
 * auxiliaria interna
 * ---------------------------------------------------------------- */

static int lex_adde(lexator_t *lex, signum_genus_t genus,
                    int linea, int columna,
                    const char *initium, int longitudo)
{
    if (lex->num_signa >= lex->capacitas) {
        int nova_cap = lex->capacitas * 2;
        if (nova_cap > SIGNA_MAX)
            nova_cap = SIGNA_MAX;
        if (lex->num_signa >= nova_cap)
            return -1;
        signum_t *nova = realloc(lex->signa,
                                 (size_t)nova_cap * sizeof(signum_t));
        if (!nova) return -1;
        lex->signa = nova;
        lex->capacitas = nova_cap;
    }

    signum_t *s = &lex->signa[lex->num_signa++];
    s->genus     = genus;
    s->linea     = linea;
    s->columna   = columna;
    s->initium   = initium;
    s->longitudo = longitudo;
    return 0;
}

/* est character vocabuli? (litterae, digiti, _) */
static int est_verbale(int c)
{
    return isalnum((unsigned char)c) || c == '_';
}

/* ----------------------------------------------------------------
 * dissectio principalis
 * ---------------------------------------------------------------- */

int lexator_disseca(lexator_t *lex, const char *fons, size_t lon)
{
    memset(lex, 0, sizeof(*lex));
    lex->fons     = fons;
    lex->fons_lon = lon;
    lex->capacitas = 4096;
    lex->signa = malloc((size_t)lex->capacitas * sizeof(signum_t));
    if (!lex->signa) return -1;

    const char *p   = fons;
    const char *fin = fons + lon;
    int linea   = 1;
    int columna = 0;

    while (p < fin) {
        const char *initium = p;
        int lin0 = linea;
        int col0 = columna;

        /* --- linea nova --- */
        if (*p == '\n') {
            lex_adde(lex, SIGNUM_LINEA_NOVA, linea, columna, p, 1);
            p++;
            linea++;
            columna = 0;
            continue;
        }

        /* \r solo vel \r\n — tractamus ut lineam novam */
        if (*p == '\r') {
            int lon_cr = 1;
            if (p + 1 < fin && p[1] == '\n')
                lon_cr = 2;
            lex_adde(lex, SIGNUM_LINEA_NOVA, linea, columna, p, lon_cr);
            p += lon_cr;
            linea++;
            columna = 0;
            continue;
        }

        /* --- spatia (non linea nova) --- */
        if (*p == ' ' || *p == '\t') {
            while (p < fin && (*p == ' ' || *p == '\t')) {
                columna += (*p == '\t') ? 8 : 1;
                p++;
            }
            lex_adde(lex, SIGNUM_SPATIUM, lin0, col0, initium,
                     (int)(p - initium));
            continue;
        }

        /* --- commentarium lineare: // --- */
        if (p + 1 < fin && p[0] == '/' && p[1] == '/') {
            while (p < fin && *p != '\n')
                p++;
            lex_adde(lex, SIGNUM_COMMENTARIUM, lin0, col0, initium,
                     (int)(p - initium));
            columna += (int)(p - initium);
            continue;
        }

        /* --- commentarium clausum --- */
        if (p + 1 < fin && p[0] == '/' && p[1] == '*') {
            p += 2;
            while (p + 1 < fin && !(p[0] == '*' && p[1] == '/')) {
                if (*p == '\n') {
                    linea++;
                    columna = 0;
                } else {
                    columna++;
                }
                p++;
            }
            if (p + 1 < fin) p += 2; /* transili */
            lex_adde(lex, SIGNUM_COMMENTARIUM, lin0, col0, initium,
                     (int)(p - initium));
            columna += 2;
            continue;
        }

        /* --- directiva praeprocessoris: # --- */
        if (*p == '#' && col0 == 0) {
            /* lege directivam usque ad finem lineae, cum continuationibus */
            while (p < fin) {
                if (*p == '\n') {
                    /* inspice an linea continuata sit per \ */
                    if (p > fons && *(p - 1) == '\\') {
                        linea++;
                        columna = 0;
                        p++;
                        continue;
                    }
                    break;
                }
                p++;
            }
            lex_adde(lex, SIGNUM_PRAEPROCESSOR, lin0, col0, initium,
                     (int)(p - initium));
            columna = 0;
            continue;
        }
        /* # non in columna 0 — tractamus ut spatii indentati # */
        if (*p == '#') {
            /* directiva praeprocessoris post spatia */
            while (p < fin && *p != '\n') {
                if (*p == '\\' && p + 1 < fin && p[1] == '\n') {
                    p += 2;
                    linea++;
                    columna = 0;
                    continue;
                }
                p++;
            }
            lex_adde(lex, SIGNUM_PRAEPROCESSOR, lin0, col0, initium,
                     (int)(p - initium));
            columna = 0;
            continue;
        }

        /* --- chorda: "..." --- */
        if (*p == '"') {
            p++;
            columna++;
            while (p < fin && *p != '"') {
                if (*p == '\\' && p + 1 < fin) {
                    p += 2;
                    columna += 2;
                } else {
                    if (*p == '\n') { linea++; columna = 0; }
                    else columna++;
                    p++;
                }
            }
            if (p < fin) { p++; columna++; } /* transili '"' */
            lex_adde(lex, SIGNUM_CHORDA, lin0, col0, initium,
                     (int)(p - initium));
            continue;
        }

        /* --- littera characteris: '.' --- */
        if (*p == '\'') {
            p++;
            columna++;
            while (p < fin && *p != '\'') {
                if (*p == '\\' && p + 1 < fin) {
                    p += 2;
                    columna += 2;
                } else {
                    columna++;
                    p++;
                }
            }
            if (p < fin) { p++; columna++; }
            lex_adde(lex, SIGNUM_LITTERA, lin0, col0, initium,
                     (int)(p - initium));
            continue;
        }

        /* --- interpunctio structuralis --- */
        if (*p == '{') {
            lex_adde(lex, SIGNUM_APERTIO, linea, columna, p, 1);
            p++; columna++;
            continue;
        }
        if (*p == '}') {
            lex_adde(lex, SIGNUM_CLAUSIO, linea, columna, p, 1);
            p++; columna++;
            continue;
        }
        if (*p == '(') {
            lex_adde(lex, SIGNUM_APERTIO_PAR, linea, columna, p, 1);
            p++; columna++;
            continue;
        }
        if (*p == ')') {
            lex_adde(lex, SIGNUM_CLAUSIO_PAR, linea, columna, p, 1);
            p++; columna++;
            continue;
        }
        if (*p == '[') {
            lex_adde(lex, SIGNUM_APERTIO_QUAD, linea, columna, p, 1);
            p++; columna++;
            continue;
        }
        if (*p == ']') {
            lex_adde(lex, SIGNUM_CLAUSIO_QUAD, linea, columna, p, 1);
            p++; columna++;
            continue;
        }
        if (*p == ';') {
            lex_adde(lex, SIGNUM_SEMICOLON, linea, columna, p, 1);
            p++; columna++;
            continue;
        }

        /* --- verbum (identificator vel verbum clavium) --- */
        if (isalpha((unsigned char)*p) || *p == '_') {
            while (p < fin && est_verbale(*p)) {
                p++;
                columna++;
            }
            lex_adde(lex, SIGNUM_VERBUM, lin0, col0, initium,
                     (int)(p - initium));
            continue;
        }

        /* --- numerus --- */
        if (isdigit((unsigned char)*p)) {
            /* hexadecimalis */
            if (*p == '0' && p + 1 < fin &&
                (p[1] == 'x' || p[1] == 'X')) {
                p += 2; columna += 2;
                while (p < fin && isxdigit((unsigned char)*p)) {
                    p++; columna++;
                }
            } else {
                while (p < fin && (isdigit((unsigned char)*p) ||
                       *p == '.' || *p == 'e' || *p == 'E' ||
                       *p == 'x' || *p == 'X' ||
                       *p == 'a' || *p == 'b' || *p == 'c' ||
                       *p == 'd' || *p == 'f' || *p == 'A' ||
                       *p == 'B' || *p == 'C' || *p == 'D' ||
                       *p == 'F')) {
                    p++; columna++;
                }
            }
            /* suffixum: u, l, ul, ll, ull, f */
            while (p < fin && (*p == 'u' || *p == 'U' ||
                               *p == 'l' || *p == 'L' ||
                               *p == 'f' || *p == 'F')) {
                p++; columna++;
            }
            lex_adde(lex, SIGNUM_NUMERUS, lin0, col0, initium,
                     (int)(p - initium));
            continue;
        }

        /* --- operator (multi-characteres) --- */
        {
            int op_lon = 1;
            if (p + 1 < fin) {
                char duo[3] = { p[0], p[1], '\0' };
                if (strcmp(duo, "==") == 0 || strcmp(duo, "!=") == 0 ||
                    strcmp(duo, "<=") == 0 || strcmp(duo, ">=") == 0 ||
                    strcmp(duo, "&&") == 0 || strcmp(duo, "||") == 0 ||
                    strcmp(duo, "<<") == 0 || strcmp(duo, ">>") == 0 ||
                    strcmp(duo, "+=") == 0 || strcmp(duo, "-=") == 0 ||
                    strcmp(duo, "*=") == 0 || strcmp(duo, "/=") == 0 ||
                    strcmp(duo, "%=") == 0 || strcmp(duo, "&=") == 0 ||
                    strcmp(duo, "|=") == 0 || strcmp(duo, "^=") == 0 ||
                    strcmp(duo, "++") == 0 || strcmp(duo, "--") == 0 ||
                    strcmp(duo, "->") == 0)
                    op_lon = 2;
                if (p + 2 < fin) {
                    if (p[0] == '<' && p[1] == '<' && p[2] == '=')
                        op_lon = 3;
                    if (p[0] == '>' && p[1] == '>' && p[2] == '=')
                        op_lon = 3;
                }
            }
            lex_adde(lex, SIGNUM_OPERATOR, linea, columna, p, op_lon);
            p += op_lon;
            columna += op_lon;
        }
    }

    /* signum terminale */
    lex_adde(lex, SIGNUM_FINIS, linea, columna, p, 0);
    return 0;
}

void lexator_purgare(lexator_t *lex)
{
    free(lex->signa);
    memset(lex, 0, sizeof(*lex));
}
