/*
 * insinulint.c — punctum ingressus lintoris insinulint
 *
 * Usus: insinulint [-s insinulint.ison] [-c|-p] plica.c [plicaque.c ...]
 */

#include "insinulint.h"
#include "commendatio.h"
#include "correctio.h"
#include "propositio.h"
#include "ison.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================================================================
 * inspectio integra unius fasciculi
 * ================================================================ */

int insinulint_inspice(
    const char *via_fontis, const speculum_t *spec, int ala
) {
    inspector_t ins;
    char *fons;
    if (insinulint_lege_inspice(via_fontis, spec, &ins, &fons) < 0)
        return -1;
    int num = inspector_scribe(&ins);
    if (ala > 0)
        commendatio_scribe(&ins, fons, spec, ala);
    free(fons);
    return num;
}

/* ================================================================
 * propositio integra unius fasciculi
 * ================================================================ */

int insinulint_propone(
    const char *via_fontis, const speculum_t *spec, int ala
) {
    inspector_t ins;
    char *fons;
    if (insinulint_lege_inspice(via_fontis, spec, &ins, &fons) < 0)
        return -1;
    int num = inspector_scribe(&ins);

    propositum_t *prop = malloc(
        (size_t)PROPOSITA_MAX * sizeof(propositum_t)
    );
    if (prop) {
        int np = propositio_ex_inspectore(
            &ins, fons, spec, ala, prop, PROPOSITA_MAX
        );
        propositio_scribe(prop, np);
        free(prop);
    }

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
        "Usus: insinulint [-s insinulint.ison] [-i|-p|-c] [-n NUM] fasciculus.c [...]\n"
        "\n"
        "Modi (exclusivi):\n"
        "  -i        inspectio (defaltum): monita ad stderr, contextum ad stdout\n"
        "  -p        propositio: ddiff propositionum ad stdout\n"
        "            sine fasciculis: lege commendationem ex stdin\n"
        "  -c        correctio: fige fasciculos in loco\n"
        "\n"
        "Optiones:\n"
        "  -s VIA    speculum configurationis (ISON)\n"
        "            si non datur, defalta adhibentur\n"
        "  -n NUM    ala contextus (lineae ante/post; defaltum 3, -1 = sine)\n"
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
    int primus_fasciculus = 1;
    int corrige   = 0;
    int propone   = 0;
    int ala       = 3;

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
            primus_fasciculus = i + 1;
            continue;
        }
        if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--inspice") == 0) {
            /* explicite: inspectio (defaltum) */
            primus_fasciculus = i + 1;
            continue;
        }
        if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--corrige") == 0) {
            corrige = 1;
            primus_fasciculus = i + 1;
            continue;
        }
        if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--propone") == 0) {
            propone = 1;
            primus_fasciculus = i + 1;
            continue;
        }
        if (strcmp(argv[i], "-n") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "insinulint: -n requirit numerum\n");
                return 1;
            }
            ala          = atoi(argv[++i]);
            primus_fasciculus = i + 1;
            continue;
        }
        if (argv[i][0] == '-') {
            fprintf(stderr, "insinulint: optio ignota '%s'\n", argv[i]);
            return 1;
        }
        break;
    }

    if (corrige + propone > 1) {
        fprintf(stderr, "insinulint: -i, -p, -c sunt exclusivi\n");
        return 1;
    }

    /* -p sine fasciculis: lege commendationem ex stdin */
    if (propone && primus_fasciculus >= argc) {
        propositum_t *prop = malloc(
            (size_t)PROPOSITA_MAX * sizeof(propositum_t)
        );
        if (!prop)
            return 1;
        int np = propositio_ex_stdin(prop, PROPOSITA_MAX);
        int nm = propositio_scribe(prop, np);
        free(prop);
        return (nm > 0) ? 1 : 0;
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

    /* inspice (et corriges vel propone) fasciculos */
    int summa_monitorum = 0;
    for (int i = primus_fasciculus; i < argc; i++) {
        if (corrige && spec.com_veta)
            correctio_excommenta(argv[i]);
        if (corrige) {
            /* itera usque ad stabilitatem */
            for (int passus = 0; passus < 16; passus++) {
                int res_b = correctio_bracchia_necessaria(
                    argv[i], &spec
                );
                int res = correctio_age(argv[i], &spec);
                if (res_b <= 0 && res <= 0)
                    break;
            }
        }
        int res;
        if (propone)
            res = insinulint_propone(argv[i], &spec, ala);
        else
            res = insinulint_inspice(argv[i], &spec, ala);
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
        if (!corrige)
            return 1;
    }

    return 0;
}
