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
    int nl = 0;
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

        int idx = m->linea - 1;
        if (idx < 0)        idx = 0;
        if (idx >= num_lin) idx = num_lin - 1;

        int ab = idx - ala;
        int ad = idx + ala;
        if (ab < 0)        ab = 0;
        if (ad >= num_lin) ad = num_lin - 1;

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

            cur->num_fen    = 0;
            cur->idx_centri = -1;
            np++;
        }
    }

    return np;
}

/* ================================================================
 * dispatch — voca propositio appropriatam pro regula
 * ================================================================ */

/* tabula dispatch: nomen regulae → functio propositionis */
typedef int (*propone_fn)(
    const prop_linea_t *centrum,
    char *dest, int dest_max
);

static const struct {
    const char *regula;
    propone_fn  fn;
} tabula_prop[] = {
    { "spatia_terminalia",  propone_spatia_terminalia },
    { "spatium_virgula",    propone_spatium_virgula },
    { "spatium_operator",   propone_spatium_operator },
    { "spatium_verbum",     propone_spatium_verbum },
    { "spatium_semicolon",  propone_spatium_semicolon },
};
static const int num_prop = 5;

/*
 * propone_lineam — inveni propositionem pro regula, applica.
 * Reddit longitudinem lineae propositae, vel -1 si nulla mutatio.
 */
static int propone_lineam(
    const propositum_t *p,
    char *dest, int dest_max
) {
    if (p->idx_centri < 0)
        return -1;

    const prop_linea_t *cl = &p->fen[p->idx_centri];

    for (int i = 0; i < num_prop; i++) {
        if (strcmp(p->regula, tabula_prop[i].regula) == 0)
            return tabula_prop[i].fn(cl, dest, dest_max);
    }
    return -1;
}

/* ================================================================
 * propositio_scribe — exitus ddiff
 * ================================================================ */

/* mutatio: una linea mutata */
typedef struct {
    int  linea;
    char vetus[PROP_LINEA_MAX];
    int  vet_lon;
    char novus[PROP_LINEA_MAX];
    int  nov_lon;
} mutatio_t;

int propositio_scribe(const propositum_t *prop, int num)
{
    if (num <= 0)
        return 0;

    /* collige mutationes: applica propositiones, retine quae mutantur */
    mutatio_t *mutationes = malloc(
        (size_t)num * sizeof(mutatio_t)
    );
    char **viae = malloc((size_t)num * sizeof(char *));
    if (!mutationes || !viae) {
        free(mutationes);
        free(viae);
        return 0;
    }
    int nm = 0;

    /* indeces viarum unicarum pro PLICAE */
    char viae_unicae[256][VIA_MAX];
    int num_viarum = 0;

    for (int i = 0; i < num; i++) {
        const propositum_t *p = &prop[i];
        char dest[PROP_LINEA_MAX * 2];

        int nov_lon = propone_lineam(p, dest, (int)sizeof(dest));
        if (nov_lon < 0)
            continue;

        /* deduplica: si iam habemus mutationem pro hac linea, praetermitte */
        int dup = 0;
        for (int j = 0; j < nm; j++) {
            if (
                strcmp(viae[j], p->via) == 0 &&
                mutationes[j].linea == p->linea
            )  {
                dup = 1;
                break;
            }
        }
        if (dup)
            continue;

        mutatio_t *m = &mutationes[nm];
        m->linea     = p->linea;

        const prop_linea_t *cl = &p->fen[p->idx_centri];
        m->vet_lon             = cl->lon;
        memcpy(m->vetus, cl->textus, cl->lon);
        m->vetus[cl->lon] = '\0';

        m->nov_lon = nov_lon;
        memcpy(m->novus, dest, nov_lon);
        m->novus[nov_lon] = '\0';

        viae[nm] = (char *)p->via;

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
        free(mutationes);
        free(viae);
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
            if (strcmp(viae[i], viae_unicae[v]) != 0)
                continue;
            printf("  -%d %s\n", mutationes[i].linea, mutationes[i].vetus);
            printf("  +%d %s\n", mutationes[i].linea, mutationes[i].novus);
        }
        printf(".\n");
    }

    free(mutationes);
    free(viae);
    return nm;
}
