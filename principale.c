/*
 * principale.c — punctum ingressus lintoris insinulint
 *
 * Usus: insinulint [-s insinulint.ison] [-c] plica.c [plicaque.c ...]
 */

#include "insinulint.h"
#include "correctio.h"
#include "ison.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================================================================
 * inspectio integra unius fasciculi
 * ================================================================ */

int insinulint_inspice(const char *via_fontis, const speculum_t *spec)
{
    inspector_t ins;
    char *fons;
    if (insinulint_lege_inspice(via_fontis, spec, &ins, &fons) < 0)
        return -1;
    int num = inspector_scribe(&ins);
    free(fons);
    return num;
}

/* ================================================================
 * functio principalis
 * ================================================================ */

static void auxilium(void)
{
    fprintf(
        stderr,
        "Usus: insinulint [-s insinulint.ison] [-c] fasciculus.c [...]\n"
        "\n"
        "Optiones:\n"
        "  -s VIA    speculum configurationis (ISON)\n"
        "            defaltum: ./insinulint.ison si exstat\n"
        "  -c        corriges fasciculos in loco (non solum inspice)\n"
        "  -h        hoc auxilium monstra\n"
        "\n"
        "Stili continuationis (indentatio.continuatio in speculo):\n"
        "  congruens  continuatio ad columnam post '(' alignata (defaltum)\n"
        "  massa      continuatio indentata per unum gradum\n"
        "  pendens    '(' in fine lineae, contentum +1 gradus, ')' ad basin\n"
        "  patens     ut pendens, et ')' debet esse primum signum in linea\n"
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
        "  commentaria_vetita  nulla commentaria permissa\n"
    );
}

int main(int argc, char **argv)
{
    const char *via_speculi = NULL;
    int speculum_explicatum = 0;
    int primus_fasciculus = 1;
    int corrige = 0;

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
            via_speculi       = argv[++i];
            speculum_explicatum = 1;
            primus_fasciculus = i + 1;
            continue;
        }
        if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--corrige") == 0) {
            corrige = 1;
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

    /* si -s non datur, proba insinulint.ison in directorio currenti */
    if (!speculum_explicatum) {
        FILE *f = fopen("insinulint.ison", "rb");
        if (f) {
            fclose(f);
            via_speculi = "insinulint.ison";
        }
    }

    /* lege speculum */
    speculum_t spec;
    if (speculum_lege(&spec, via_speculi) < 0)
        return 1;

    /* inspice (et corriges si -c) fasciculos */
    int summa_monitorum = 0;
    for (int i = primus_fasciculus; i < argc; i++) {
        if (corrige && spec.com_veta)
            correctio_excommenta(argv[i]);
        if (corrige)
            correctio_age(argv[i], &spec);
        int res = insinulint_inspice(argv[i], &spec);
        if (res < 0) {
            summa_monitorum++;
        } else {
            summa_monitorum += res;
        }
    }

    /* epilogus */
    if (summa_monitorum > 0) {
        fprintf(
            stderr, "\ninsinulint: %d monita inventa.\n",
            summa_monitorum
        );
        return 1;
    }

    return 0;
}
