/*
 * correctiones/excommenta.c — exstirpat omnia commentaria ex plica
 */

#include "../correctio.h"
#include "../insinulint.h"
#include "../ison/ison.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int correctio_excommenta(const char *via)
{
    char *fons = ison_lege_plicam(via);
    if (!fons) {
        fprintf(stderr, "correctio: non possum legere '%s'\n", via);
        return -1;
    }
    size_t fons_lon = strlen(fons);

    lexator_t lex;
    if (lexator_disseca(&lex, fons, fons_lon) < 0) {
        free(fons);
        return -1;
    }

    char *out = malloc(fons_lon + 2);
    if (!out) {
        lexator_purgare(&lex);
        free(fons);
        return -1;
    }

    char *wp = out;
    const char *src = fons;

    for (int i = 0; i < lex.num_signa; i++) {
        const signum_t *s = &lex.signa[i];
        if (s->genus != SIGNUM_COMMENTARIUM)
            continue;

        const char *com_init = s->initium;
        const char *com_fin  = s->initium + s->longitudo;

        if (com_init > src) {
            memcpy(wp, src, (size_t)(com_init - src));
            wp += com_init - src;
        }

        while (wp > out && (*(wp - 1) == ' ' || *(wp - 1) == '\t'))
            wp--;

        int sola_linea = (wp == out || (wp > out && *(wp - 1) == '\n'));
        if (sola_linea && com_fin < fons + fons_lon && *com_fin == '\n')
            com_fin++;

        src = com_fin;
    }

    const char *fons_fin = fons + fons_lon;
    if (src < fons_fin) {
        memcpy(wp, src, (size_t)(fons_fin - src));
        wp += fons_fin - src;
    }

    size_t outlon = (size_t)(wp - out);

    int res = 0;
    if (outlon != fons_lon || memcmp(out, fons, outlon) != 0) {
        FILE *f = fopen(via, "wb");
        if (!f) {
            fprintf(stderr, "correctio: non possum scribere '%s'\n", via);
            res = -1;
        } else {
            if (fwrite(out, 1, outlon, f) != outlon) {
                fprintf(stderr, "correctio: erratum scribendi '%s'\n", via);
                res = -1;
            }
            fclose(f);
        }
    }

    lexator_purgare(&lex);
    free(fons);
    free(out);
    return res;
}
