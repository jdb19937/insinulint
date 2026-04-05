/*
 * correctiones/bracchia.c — correctiones bracchiorum
 *
 * K&R: iunge '{' ex linea nova ad finem lineae praecedentis.
 * Allman: scinde '{' ex eadem linea ad lineam novam.
 * Necessaria: adde '{' et '}' circa corpora nuda.
 */

#include "../correctio.h"
#include "bracchia.h"
#include "../inspectio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================================================================
 * corrige_bracchia_kr — iunge '{' ad finem lineae currentis
 *
 * Linea currens (i) est linea ante '{'. Linea i+1 continet '{'.
 * Scribit contentum lineae currentis, addit ' {', transit lineam i+1.
 * Si contentum post '{' adest et una_ind_prox >= 0, scindit sententias.
 * ================================================================ */

char *corrige_bracchia_kr(
    char *wp, const char *corpus, int corp_lon,
    const linea_t *lineae, int i, int nlin,
    int *transili, int una_ind_prox, const speculum_t *spec
) {
    /* scribe contentum lineae currentis sine spatiis terminalibus */
    int lon = corp_lon;
    while (
        lon > 0 &&
        (corpus[lon - 1] == ' ' || corpus[lon - 1] == '\t')
    )
        lon--;

    /* inveni '//' commentarium (extra chordas) */
    int comm = -1;
    for (int j = 0; j < lon; j++) {
        if (corpus[j] == '"' || corpus[j] == '\'') {
            char d = corpus[j];
            for (j++; j < lon; j++) {
                if (corpus[j] == '\\' && j + 1 < lon)
                    j++;
                else if (corpus[j] == d)
                    break;
            }
            continue;
        }
        if (
            corpus[j] == '/' && j + 1 < lon &&
            corpus[j + 1] == '/'
        ) {
            comm = j;
            break;
        }
    }

    if (comm >= 0) {
        /* insere '{' ante '//' commentarium */
        int ante = comm;
        while (
            ante > 0 &&
            (corpus[ante - 1] == ' ' || corpus[ante - 1] == '\t')
        )
            ante--;
        memcpy(wp, corpus, ante);
        wp += ante;
        *wp++ = ' ';
        *wp++ = '{';
        *wp++ = ' ';
        memcpy(wp, corpus + comm, lon - comm);
        wp += lon - comm;
    } else {
        memcpy(wp, corpus, lon);
        wp += lon;
        *wp++ = ' ';
        *wp++ = '{';
    }

    /* emitte contentum post '{' in linea proxima */
    if (i + 1 < nlin) {
        const linea_t *ln = &lineae[i + 1];
        int k = 0;
        while (
            k < ln->lon &&
            (ln->initium[k] == ' ' || ln->initium[k] == '\t')
        )
            k++;
        /* transili '{' */
        if (k < ln->lon && ln->initium[k] == '{')
            k++;
        /* transili spatia post '{' */
        while (
            k < ln->lon &&
            (ln->initium[k] == ' ' || ln->initium[k] == '\t')
        )
            k++;

        if (k < ln->lon) {
            /* restat contentum post '{' */
            int rest_lon = ln->lon - k;
            while (
                rest_lon > 0 &&
                (
                    ln->initium[k + rest_lon - 1] == ' ' ||
                    ln->initium[k + rest_lon - 1] == '\t'
                )
            )
                rest_lon--;

            /* computa indentationem: linea praecedentis + unus gradus */
            const linea_t *lc = &lineae[i];
            int ind_cur       = 0;
            for (int j = 0; j < lc->lon; j++) {
                if (lc->initium[j] == ' ')
                    ind_cur++;
                else if (lc->initium[j] == '\t')
                    ind_cur += 8;
                else
                    break;
            }
            int lat = spec->ind_tabulis ? 8 : spec->ind_latitudo;
            if (lat <= 0)
                lat = 4;
            int ind_rest = ind_cur + lat;

            *wp++ = '\n';
            if (una_ind_prox >= 0) {
                /* sententias scinde */
                wp = scribe_indentationem(wp, ind_rest, spec);
                wp = corrige_unam_sententiam(
                    wp, ln->initium + k, rest_lon,
                    ind_rest, spec
                );
                /* corrige_unam_sententiam iam '\n' addidit */
            } else {
                wp = scribe_indentationem(wp, ind_rest, spec);
                memcpy(wp, ln->initium + k, rest_lon);
                wp += rest_lon;
                *wp++ = '\n';
            }
        } else {
            *wp++ = '\n';
        }
        *transili = 1;
    } else {
        *wp++ = '\n';
    }

    return wp;
}

/* ================================================================
 * corrige_bracchia_allman — scinde '{' ad lineam novam
 * ================================================================ */

char *corrige_bracchia_allman(
    char *wp, const char *corpus, int corp_lon,
    int col, int ind, const speculum_t *spec
) {
    /* inveni '{' in corpore */
    int bra_byte = -1;
    int cur_col  = 0;
    for (int j = 0; j < corp_lon; j++) {
        if (cur_col >= col && corpus[j] == '{') {
            bra_byte = j;
            break;
        }
        if (corpus[j] == '\t')
            cur_col += 8 - (cur_col % 8);
        else
            cur_col++;
    }
    if (bra_byte < 0) {
        memcpy(wp, corpus, corp_lon);
        wp += corp_lon;
        *wp++ = '\n';
        return wp;
    }

    /* pars ante '{' — sine spatiis terminalibus */
    int ante = bra_byte;
    while (
        ante > 0 &&
        (corpus[ante - 1] == ' ' || corpus[ante - 1] == '\t')
    )
        ante--;
    memcpy(wp, corpus, ante);
    wp += ante;
    *wp++ = '\n';

    /* '{' cum indentione in nova linea */
    wp    = scribe_indentationem(wp, ind, spec);
    *wp++ = '{';

    /* contentum post '{' */
    int post = bra_byte + 1;
    while (
        post < corp_lon &&
        (corpus[post] == ' ' || corpus[post] == '\t')
    )
        post++;
    if (post < corp_lon) {
        int lat = spec->ind_latitudo;
        if (lat <= 0)
            lat      = 4;
        *wp++        = '\n';
        int rest_lon = corp_lon - post;
        while (
            rest_lon > 0 &&
            (
                corpus[post + rest_lon - 1] == ' ' ||
                corpus[post + rest_lon - 1] == '\t'
            )
        )
            rest_lon--;
        wp = scribe_indentationem(wp, ind + lat, spec);
        memcpy(wp, corpus + post, rest_lon);
        wp += rest_lon;
    }
    *wp++ = '\n';

    return wp;
}

/* ================================================================
 * correctio_bracchia_necessaria — adde { } circa corpora nuda
 *
 * Passus separatus: legit plicam, invenit bracchia_necessaria monita,
 * inserit '{' et '}' in fontem. Correctiones formae (indentatio, stilus
 * bracchiorum) in passu sequenti per correctio_age applicantur.
 * Reddit 1 si mutatum, 0 si non, -1 si error.
 * ================================================================ */

/* insertio in fontem */
typedef struct {
    size_t offset;
    int    est_apertio;   /* 1 = ' {', 0 = indentatio + '}' + '\n' */
    int    indentatio;    /* spatia pro '}' */
} insertio_t;

#define INSERTIONES_MAX 256

int correctio_bracchia_necessaria(const char *via, const speculum_t *spec)
{
    if (!spec->bra_necessaria)
        return 0;

    /* lege, lexa, inspice */
    inspector_t ins;
    char *fons;
    if (insinulint_lege_inspice(via, spec, &ins, &fons) < 0)
        return -1;
    size_t fons_lon = strlen(fons);

    /* lexa denuo (lege_inspice purgat lexatorem) */
    lexator_t lex;
    if (lexator_disseca(&lex, fons, fons_lon) < 0) {
        free(fons);
        return -1;
    }

    const signum_t *signa = lex.signa;
    int n = lex.num_signa;

    insertio_t insertiones[INSERTIONES_MAX];
    int num_ins = 0;

    for (int mi = 0; mi < ins.num_monita; mi++) {
        const monitum_t *m = &ins.monita[mi];
        if (strcmp(m->regula, "bracchia_necessaria") != 0)
            continue;

        /* inveni signum verbi clavis per lineam et columnam */
        int ki = -1;
        for (int j = 0; j < n; j++) {
            if (
                signa[j].linea == m->linea &&
                signa[j].columna == m->columna &&
                signa[j].genus == SIGNUM_VERBUM
            ) {
                ki = j;
                break;
            }
        }
        if (ki < 0)
            continue;

        /* inveni initium corporis et punctum insertionis '{' */
        int corp_init       = -1;
        size_t apert_offset = 0;

        if (
            est_verbum(&signa[ki], "else") ||
            est_verbum(&signa[ki], "do")
        ) {
            corp_init = prox_significans(signa, n, ki + 1);
            apert_offset = (size_t)(
                signa[ki].initium - fons
            ) + (size_t)signa[ki].longitudo;
        } else {
            /* if/for/while/switch: quaere ')' clausam */
            int par_ap = prox_significans(signa, n, ki + 1);
            if (par_ap >= n || signa[par_ap].genus != SIGNUM_APERTIO_PAR)
                continue;
            int par_cl = quaere_par_clausam(signa, n, par_ap);
            if (par_cl < 0)
                continue;
            corp_init = prox_significans(signa, n, par_cl + 1);
            apert_offset = (size_t)(
                signa[par_cl].initium - fons
            ) + (size_t)signa[par_cl].longitudo;
        }
        if (corp_init < 0 || corp_init >= n)
            continue;
        if (signa[corp_init].genus == SIGNUM_APERTIO)
            continue; /* iam habet bracchia */

        /* inveni finem corporis: ';' ad profunditatem 0 */
        int corp_finis = -1;
        int depth      = 0;
        for (int j = corp_init; j < n; j++) {
            if (
                signa[j].genus == SIGNUM_APERTIO_PAR ||
                signa[j].genus == SIGNUM_APERTIO_QUAD
            )
                depth++;
            else if (
                signa[j].genus == SIGNUM_CLAUSIO_PAR ||
                signa[j].genus == SIGNUM_CLAUSIO_QUAD
            )
                depth--;
            else if (signa[j].genus == SIGNUM_SEMICOLON && depth == 0) {
                corp_finis = j;
                break;
            }
        }
        if (corp_finis < 0)
            continue;

        /* computa indentationem verbi clavis */
        int ind       = 0;
        const char *p = signa[ki].initium;
        while (p > fons && *(p - 1) != '\n')
            p--;
        while (*p == ' ' || *p == '\t') {
            if (*p == '\t')
                ind += 8;
            else
                ind++;
            p++;
        }

        /* '}' statim post ';' corporis */
        size_t claus_offset = (size_t)(
            signa[corp_finis].initium - fons
        ) + (size_t)signa[corp_finis].longitudo;

        if (num_ins + 2 > INSERTIONES_MAX)
            break;

        insertiones[num_ins].offset      = apert_offset;
        insertiones[num_ins].est_apertio = 1;
        insertiones[num_ins].indentatio  = ind;
        num_ins++;

        insertiones[num_ins].offset      = claus_offset;
        insertiones[num_ins].est_apertio = 0;
        insertiones[num_ins].indentatio  = ind;
        num_ins++;
    }

    if (num_ins == 0) {
        lexator_purgare(&lex);
        free(fons);
        return 0;
    }

    /* ordina per offset crescens; si aequales, apertio ante clausionem,
     * et clausio cum maiore indentione ante minorem */
    for (int i = 0; i < num_ins - 1; i++) {
        for (int j = i + 1; j < num_ins; j++) {
            int swap = 0;
            if (insertiones[j].offset < insertiones[i].offset)
                swap = 1;
            else if (insertiones[j].offset == insertiones[i].offset) {
                if (
                    insertiones[j].est_apertio >
                    insertiones[i].est_apertio
                )
                    swap = 1;
                else if (
                    insertiones[j].est_apertio ==
                    insertiones[i].est_apertio &&
                    !insertiones[j].est_apertio &&
                    insertiones[j].indentatio >
                    insertiones[i].indentatio
                )
                    swap = 1;
            }
            if (swap) {
                insertio_t tmp = insertiones[i];
                insertiones[i] = insertiones[j];
                insertiones[j] = tmp;
            }
        }
    }

    /* aedifica novum fontem per segmenta */
    size_t max_lon = fons_lon + (size_t)num_ins * 128;
    char *novum    = malloc(max_lon);
    if (!novum) {
        lexator_purgare(&lex);
        free(fons);
        return -1;
    }

    char *wp       = novum;
    size_t ultimum = 0;

    for (int i = 0; i < num_ins; i++) {
        size_t off = insertiones[i].offset;

        /* copia segmentum ante insertionem */
        if (off > ultimum) {
            memcpy(wp, fons + ultimum, off - ultimum);
            wp += off - ultimum;
        }
        ultimum = off;

        if (insertiones[i].est_apertio) {
            *wp++ = ' ';
            *wp++ = '{';
        } else {
            int ind_val = insertiones[i].indentatio;
            *wp++       = '\n';
            if (spec->ind_tabulis) {
                int tabs = ind_val / 8;
                int rest = ind_val % 8;
                for (int t = 0; t < tabs; t++)
                    *wp++ = '\t';
                for (int t = 0; t < rest; t++)
                    *wp++ = ' ';
            } else {
                for (int t = 0; t < ind_val; t++)
                    *wp++ = ' ';
            }
            *wp++ = '}';
        }
    }

    /* copia residuum */
    if (ultimum < fons_lon) {
        memcpy(wp, fons + ultimum, fons_lon - ultimum);
        wp += fons_lon - ultimum;
    }

    size_t nova_lon = (size_t)(wp - novum);

    /* scribe si mutatum */
    int mutatum = (nova_lon != fons_lon || memcmp(fons, novum, fons_lon) != 0);
    int res     = 0;

    if (mutatum) {
        FILE *f = fopen(via, "wb");
        if (!f) {
            fprintf(
                stderr,
                "correctio: non possum scribere '%s'\n", via
            );
            res = -1;
        } else {
            if (fwrite(novum, 1, nova_lon, f) != nova_lon) {
                fprintf(
                    stderr,
                    "correctio: erratum scribendi '%s'\n", via
                );
                res = -1;
            }
            fclose(f);
        }
    }

    free(novum);
    lexator_purgare(&lex);
    free(fons);
    return (res < 0) ? -1 : mutatum;
}
