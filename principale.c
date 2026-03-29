/*
 * principale.c — punctum ingressus lintoris insinulint
 *
 * Usus: insinulint [-s speculum.ison] plica.c [plicaque.c ...]
 */

#include "insinulint.h"
#include "ison.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================================================================
 * inspectio integra unius fasciculi
 * ================================================================ */

int insinulint_inspice(const char *via_fontis, const speculum_t *spec)
{
    /* lege plicam */
    char *fons = ison_lege_plicam(via_fontis);
    if (!fons) {
        fprintf(stderr, "insinulint: non possum legere '%s'\n", via_fontis);
        return -1;
    }
    size_t lon = strlen(fons);

    /* disseca in signa */
    lexator_t lex;
    if (lexator_disseca(&lex, fons, lon) < 0) {
        fprintf(stderr, "insinulint: erratum dissecandi '%s'\n", via_fontis);
        free(fons);
        return -1;
    }

    /* praepara inspectorem */
    inspector_t ins;
    inspector_initia(&ins, via_fontis);

    /* applica regulas */
    inspice_omnia(&ins, &lex, spec);

    /* scribe monita */
    int num = inspector_scribe(&ins);

    /* purga */
    lexator_purgare(&lex);
    free(fons);

    return num;
}

/* ================================================================
 * functio principalis
 * ================================================================ */

static void auxilium(void)
{
    fprintf(stderr,
        "Usus: insinulint [-s speculum.ison] fasciculus.c [...]\n"
        "\n"
        "Optiones:\n"
        "  -s VIA    speculum configurationis (JSON)\n"
        "  -h        hoc auxilium monstra\n"
        "\n"
        "Regulae:\n"
        "  indentatio          gradus per bracchia et continuationem\n"
        "  spatium_verbum      spatium post if/for/while/switch/return\n"
        "  spatium_operator    spatia circa =, ==, !=, +=, &&, ||, etc.\n"
        "  spatium_virgula     spatium post virgulam, non ante\n"
        "  spatium_semicolon   nullum spatium ante ;\n"
        "  bracchia_stilus     stilus K&R vel Allman pro '{'\n"
        "  bracchia_necessaria bracchia post if/for/while/else\n"
        "  cubitus             } else { in eadem linea (vel non)\n"
        "  longitudo_lineae    longitudo maxima lineae\n"
        "  spatia_terminalia   nulla spatia ad finem lineae\n"
        "  lineae_vacuae       lineae vacuae consecutivae maximae\n"
        "  finis_lineae        fasciculus finit cum \\n\n"
        "  tabulae_mixtae      veta tabulas et spatia mixtas\n"
    );
}

int main(int argc, char **argv)
{
    const char *via_speculi = NULL;
    int primus_fasciculus = 1;

    /* argumenta */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--auxilium") == 0) {
            auxilium();
            return 0;
        }
        if (strcmp(argv[i], "-s") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "insinulint: -s requirit viam speculi\n");
                return 1;
            }
            via_speculi = argv[++i];
            primus_fasciculus = i + 1;
            continue;
        }
        if (argv[i][0] == '-') {
            fprintf(stderr, "insinulint: optio ignota '%s'\n", argv[i]);
            return 1;
        }
        break;
    }

    if (primus_fasciculus >= argc) {
        fprintf(stderr, "insinulint: nullus fasciculus datus\n");
        auxilium();
        return 1;
    }

    /* lege speculum */
    speculum_t spec;
    if (speculum_lege(&spec, via_speculi) < 0)
        return 1;

    /* inspice fasciculos */
    int summa_monitorum = 0;
    for (int i = primus_fasciculus; i < argc; i++) {
        int res = insinulint_inspice(argv[i], &spec);
        if (res < 0) {
            summa_monitorum++;
        } else {
            summa_monitorum += res;
        }
    }

    /* epilogus */
    if (summa_monitorum > 0) {
        fprintf(stderr, "\ninsinulint: %d monita inventa.\n",
                summa_monitorum);
        return 1;
    }

    return 0;
}
