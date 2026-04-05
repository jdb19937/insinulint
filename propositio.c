/*
 * propositio.c — propositiones in forma ddiff
 *
 * Tria officia:
 *   1. propositio_ex_inspectore — construi proposita ex inspectore
 *   2. propositio_ex_stdin — reparsere formam commendationis
 *   3. propositio_scribe — applicare propositiones, scribere ddiff
 *
 * Regulae tractatae (per-lineam):
 *   spatia_terminalia, spatium_virgula, spatium_operator,
 *   spatium_verbum, spatium_semicolon
 */

#include "propositio.h"
#include "propositiones/spatia_terminalia.h"
#include "propositiones/spatium_virgula.h"
#include "propositiones/spatium_operator.h"
#include "propositiones/spatium_verbum.h"
#include "propositiones/spatium_semicolon.h"
#include "propositiones/indentatio.h"
#include "propositiones/commentaria.h"
#include "propositiones/colineatio.h"
#include "propositiones/longitudo.h"
#include "propositiones/bracchia_stilus.h"
#include "propositiones/cubitus.h"
#include "propositiones/corpus.h"
#include "propositiones/una_sententia.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================================================================
 * propositio_ex_inspectore — inline modus
 * ================================================================ */

#define LINEAE_MAX (1 << 16)

/* aedifica indicem linearum in fonte (ut in commendatio.c) */
static int aedifica_lineas(
    const char *fons, const char **lineae, int max
) {
    int nl       = 0;
    lineae[nl++] = fons;
    for (const char *p = fons; *p && nl < max; p++) {
        if (*p == '\n')
            lineae[nl++] = p + 1;
    }
    return nl;
}

static int lon_lineae(const char *linea)
{
    const char *p = linea;
    while (*p && *p != '\n')
        p++;
    return (int)(p - linea);
}

int propositio_ex_inspectore(
    const inspector_t *ins, const char *fons,
    const speculum_t *spec,
    int ala, propositum_t *dest, int max
) {
    const char **lineae = malloc(
        (size_t)LINEAE_MAX * sizeof(const char *)
    );
    if (!lineae)
        return 0;
    int num_lin = aedifica_lineas(fons, lineae, LINEAE_MAX);

    const char *via = ins->via_fasciculi
        ? ins->via_fasciculi
        : "<fons>";
    int np = 0;

    for (int i = 0; i < ins->num_monita && np < max; i++) {
        const monitum_t *m = &ins->monita[i];
        propositum_t *p    = &dest[np];

        snprintf(p->via,     sizeof(p->via),     "%s", via);
        snprintf(p->regula,  sizeof(p->regula),  "%s", m->regula);
        snprintf(p->nuntius, sizeof(p->nuntius), "%s", m->nuntius);
        p->linea   = m->linea;
        p->columna = m->columna;

        /* metadata ex monitum_t */
        p->fix_valor     = m->fix_valor;
        p->split_columna = m->split_columna;
        p->apert_columna = m->apert_columna;
        p->bra_columna   = m->bra_columna;

        /* metadata ex speculum_t */
        if (spec) {
            p->ind_latitudo     = spec->ind_latitudo;
            p->ind_tabulis      = spec->ind_tabulis;
            p->bra_stilus       = spec->bra_stilus;
            p->lin_longitudo_max = spec->lin_longitudo_max;
        } else {
            p->ind_latitudo     = 4;
            p->ind_tabulis      = 0;
            p->bra_stilus       = 0;
            p->lin_longitudo_max = 0;
        }

        int idx = m->linea - 1;
        if (idx < 0)
            idx = 0;
        if (idx >= num_lin)
            idx = num_lin - 1;

        int ab = idx - ala;
        int ad = idx + ala;
        if (ab < 0)
            ab = 0;
        if (ad >= num_lin)
            ad = num_lin - 1;

        p->num_fen    = 0;
        p->idx_centri = -1;
        for (int j = ab; j <= ad; j++) {
            if (p->num_fen >= PROP_FEN_MAX)
                break;
            prop_linea_t *fl = &p->fen[p->num_fen];
            fl->numerus      = j + 1;
            fl->lon          = lon_lineae(lineae[j]);
            if (fl->lon >= PROP_LINEA_MAX)
                fl->lon = PROP_LINEA_MAX - 1;
            memcpy(fl->textus, lineae[j], fl->lon);
            fl->textus[fl->lon] = '\0';
            if (j == idx)
                p->idx_centri = p->num_fen;
            p->num_fen++;
        }
        np++;
    }

    free(lineae);
    return np;
}

/* ================================================================
 * propositio_ex_stdin — parser formae commendationis
 * ================================================================ */

/* compara clavem non-terminatam cum chorda nota */
static int clavis_aequat(const char *cv, int cv_lon, const char *nomen)
{
    int nlon = 0;
    while (nomen[nlon])
        nlon++;
    return cv_lon == nlon && strncmp(cv, nomen, nlon) == 0;
}

/*
 * Forma commendationis:
 *   via:linea:col: tipo [regula] nuntius
 *    NN:textus...
 *   >NN:textus...       (centrum)
 *    NN:textus...
 *   --
 */

int propositio_ex_stdin(propositum_t *dest, int max)
{
    char buf[4096];
    int np               = 0;
    propositum_t *cur    = NULL;

    while (fgets(buf, sizeof(buf), stdin) && np < max) {
        /* remove '\n' terminalem */
        int blon = (int)strlen(buf);
        if (blon > 0 && buf[blon - 1] == '\n')
            buf[--blon] = '\0';

        /* separator */
        if (strcmp(buf, "--") == 0) {
            cur = NULL;
            continue;
        }

        /* metadata: '  @clavem=valorem ...' */
        if (
            cur && blon >= 3 &&
            buf[0] == ' ' && buf[1] == ' ' && buf[2] == '@'
        ) {
            const char *p = buf + 2;
            while (*p == '@') {
                p++; /* transili '@' */
                /* parse clavem */
                const char *cv = p;
                while (*p && *p != '=' && *p != ' ')
                    p++;
                int cv_lon = (int)(p - cv);
                if (*p != '=') break;
                p++; /* transili '=' */
                /* parse valorem */
                int val = atoi(p);
                while (*p && *p != ' ')
                    p++;
                while (*p == ' ')
                    p++;

                if (clavis_aequat(cv, cv_lon, "fix_valor"))
                    cur->fix_valor = val;
                else if (clavis_aequat(cv, cv_lon, "split_columna"))
                    cur->split_columna = val;
                else if (clavis_aequat(cv, cv_lon, "apert_columna"))
                    cur->apert_columna = val;
                else if (clavis_aequat(cv, cv_lon, "bra_columna"))
                    cur->bra_columna = val;
                else if (clavis_aequat(cv, cv_lon, "ind_latitudo"))
                    cur->ind_latitudo = val;
                else if (clavis_aequat(cv, cv_lon, "ind_tabulis"))
                    cur->ind_tabulis = val;
                else if (clavis_aequat(cv, cv_lon, "bra_stilus"))
                    cur->bra_stilus = val;
                else if (clavis_aequat(cv, cv_lon, "lin_longitudo_max"))
                    cur->lin_longitudo_max = val;
            }
            continue;
        }

        /* linea contextus: ' NN:...' vel '>NN:...' */
        if (cur && (buf[0] == ' ' || buf[0] == '>')) {
            int est_centrum = (buf[0] == '>');
            /* parse numerum lineae */
            int pos = 1;
            while (pos < blon && buf[pos] >= '0' && buf[pos] <= '9')
                pos++;
            if (pos < blon && buf[pos] == ':') {
                if (cur->num_fen < PROP_FEN_MAX) {
                    prop_linea_t *fl = &cur->fen[cur->num_fen];
                    /* numerus inter buf[1..pos-1] */
                    char tmp[16];
                    int nlon = pos - 1;
                    if (nlon >= (int)sizeof(tmp))
                        nlon = (int)sizeof(tmp) - 1;
                    memcpy(tmp, buf + 1, nlon);
                    tmp[nlon]   = '\0';
                    fl->numerus = atoi(tmp);

                    /* textus post ':' */
                    const char *txt = buf + pos + 1;
                    fl->lon         = blon - (pos + 1);
                    if (fl->lon >= PROP_LINEA_MAX)
                        fl->lon = PROP_LINEA_MAX - 1;
                    memcpy(fl->textus, txt, fl->lon);
                    fl->textus[fl->lon] = '\0';

                    if (est_centrum)
                        cur->idx_centri = cur->num_fen;
                    cur->num_fen++;
                }
            }
            continue;
        }

        /* caput: via:linea:col: tipo [regula] nuntius */
        {
            /* parse via */
            const char *p = buf;

            /* inveni primum ':' post via (via potest continere '/' etc.) */
            /* forma: via:NUM:NUM: tipo [regula] nuntius */
            /* quaere pattern :NUM:NUM: */
            const char *colon1 = NULL;
            for (const char *s = p; *s; s++) {
                if (
                    *s == ':' &&
                    s[1] >= '0' && s[1] <= '9'
                ) {
                    /* potentiale initium :linea:col: */
                    const char *t = s + 1;
                    while (*t >= '0' && *t <= '9')
                        t++;
                    if (
                        *t == ':' &&
                        t[1] >= '0' && t[1] <= '9'
                    ) {
                        colon1 = s;
                        break;
                    }
                }
            }
            if (!colon1)
                continue;

            cur = &dest[np];
            memset(cur, 0, sizeof(*cur));

            /* via */
            int via_lon = (int)(colon1 - p);
            if (via_lon >= VIA_MAX)
                via_lon = VIA_MAX - 1;
            memcpy(cur->via, p, via_lon);
            cur->via[via_lon] = '\0';

            /* linea */
            p = colon1 + 1;
            cur->linea = atoi(p);
            while (*p >= '0' && *p <= '9')
                p++;

            /* columna */
            if (*p == ':')
                p++;
            cur->columna = atoi(p);
            while (*p >= '0' && *p <= '9')
                p++;

            /* transili ': ' */
            if (*p == ':')
                p++;
            while (*p == ' ')
                p++;

            /* transili tipo ('monitum' vel 'erratum') */
            while (*p && *p != ' ')
                p++;
            while (*p == ' ')
                p++;

            /* regula inter '[' et ']' */
            if (*p == '[') {
                p++;
                const char *q = p;
                while (*q && *q != ']')
                    q++;
                int rlon = (int)(q - p);
                if (rlon >= 64)
                    rlon = 63;
                memcpy(cur->regula, p, rlon);
                cur->regula[rlon] = '\0';
                p = (*q == ']') ? q + 1 : q;
                while (*p == ' ')
                    p++;
            }

            /* nuntius — residuum */
            snprintf(cur->nuntius, sizeof(cur->nuntius), "%s", p);

            cur->num_fen        = 0;
            cur->idx_centri     = -1;
            cur->fix_valor      = -1;
            cur->split_columna  = -1;
            cur->apert_columna  = -1;
            cur->bra_columna    = -1;
            cur->ind_latitudo   = 4;
            cur->ind_tabulis    = 0;
            cur->bra_stilus     = 0;
            cur->lin_longitudo_max = 0;
            np++;
        }
    }

    return np;
}

/* ================================================================
 * dispatch — voca propositionem appropriatam pro regula
 * ================================================================ */

static const struct {
    const char *regula;
    propone_fn  fn;
} tabula_prop[] = {
    { "spatia_terminalia",  propone_spatia_terminalia },
    { "spatium_virgula",    propone_spatium_virgula },
    { "spatium_operator",   propone_spatium_operator },
    { "spatium_verbum",     propone_spatium_verbum },
    { "spatium_semicolon",  propone_spatium_semicolon },
    { "indentatio",         propone_indentationem },
    { "commentaria_vetita", propone_commentaria },
    { "adtributio_colineata", propone_colineationem },
    { "longitudo_lineae",   propone_longitudinem },
    { "bracchia_stilus",    propone_bracchia_stilum },
    { "cubitus",            propone_cubitum },
    { "corpus_separatum",   propone_corpus },
    { "una_sententia",      propone_unam_sententiam },
};
static const int num_prop = (int)(
    sizeof(tabula_prop) / sizeof(tabula_prop[0])
);

/*
 * propone_mutationem — inveni propositionem pro regula, applica.
 * Reddit 1 si mutatio proposita, 0 si nulla.
 */
static int propone_mutationem(
    const propositum_t *p,
    prop_eventus_t *ev
) {
    for (int i = 0; i < num_prop; i++) {
        if (strcmp(p->regula, tabula_prop[i].regula) == 0)
            return tabula_prop[i].fn(p, ev);
    }
    return 0;
}

/* ================================================================
 * propositio_scribe — exitus ddiff
 * ================================================================ */

#define EVENTUS_MAX 4096

int propositio_scribe(const propositum_t *prop, int num)
{
    if (num <= 0)
        return 0;

    prop_eventus_t *eventus = malloc(
        (size_t)num * sizeof(prop_eventus_t)
    );
    int *prop_idx = malloc((size_t)num * sizeof(int));
    if (!eventus || !prop_idx) {
        free(eventus);
        free(prop_idx);
        return 0;
    }
    int nm = 0;

    /* viae unicae pro PLICAE */
    char viae_unicae[256][VIA_MAX];
    int num_viarum = 0;

    for (int i = 0; i < num; i++) {
        const propositum_t *p = &prop[i];
        prop_eventus_t ev;
        memset(&ev, 0, sizeof(ev));

        if (!propone_mutationem(p, &ev))
            continue;

        /* deduplica: si iam habemus mutationem pro eisdem lineis */
        int dup = 0;
        for (int j = 0; j < nm; j++) {
            const propositum_t *pj = &prop[prop_idx[j]];
            if (strcmp(pj->via, p->via) != 0)
                continue;
            int a_ab = p->fen[ev.idx_ab].numerus;
            int a_ad = p->fen[ev.idx_ad].numerus;
            int b_ab = pj->fen[eventus[j].idx_ab].numerus;
            int b_ad = pj->fen[eventus[j].idx_ad].numerus;
            if (a_ab <= b_ad && a_ad >= b_ab) {
                dup = 1;
                break;
            }
        }
        if (dup)
            continue;

        eventus[nm]  = ev;
        prop_idx[nm] = i;

        /* adde viam ad unicae si nondum */
        int inventa = 0;
        for (int j = 0; j < num_viarum; j++) {
            if (strcmp(viae_unicae[j], p->via) == 0) {
                inventa = 1;
                break;
            }
        }
        if (!inventa && num_viarum < 256)
            snprintf(
                viae_unicae[num_viarum++],
                VIA_MAX, "%s", p->via
            );

        nm++;
    }

    if (nm == 0) {
        free(eventus);
        free(prop_idx);
        return 0;
    }

    /* scribe ddiff */
    printf("ddiff versio I\n\n");

    /* PLICAE */
    printf("PLICAE\n");
    for (int i = 0; i < num_viarum; i++)
        printf("M %s\n", viae_unicae[i]);
    printf("\n");

    /* TRANSLATIONES (vacuae — propositiones intra plicam) */
    printf("TRANSLATIONES\n\n");

    /* MUTATIONES */
    printf("MUTATIONES\n");
    for (int v = 0; v < num_viarum; v++) {
        printf("M %s\n", viae_unicae[v]);
        for (int i = 0; i < nm; i++) {
            const propositum_t *p    = &prop[prop_idx[i]];
            const prop_eventus_t *ev = &eventus[i];
            if (strcmp(p->via, viae_unicae[v]) != 0)
                continue;

            /* lineae veteres */
            for (int k = ev->idx_ab; k <= ev->idx_ad; k++) {
                if (k >= 0 && k < p->num_fen)
                    printf(
                        "  -%d %s\n",
                        p->fen[k].numerus,
                        p->fen[k].textus
                    );
            }
            /* lineae novae */
            for (int k = 0; k < ev->num_novi; k++)
                printf(
                    "  +%d %s\n",
                    ev->novi[k].numerus,
                    ev->novi[k].textus
                );
        }
        printf(".\n");
    }

    free(eventus);
    free(prop_idx);
    return nm;
}
