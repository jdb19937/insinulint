# insinulint

Lintor linguae C, Latine scriptus. Solum C purum (K&R, C89, C99) tractat — sine C++ sive extensionibus.

Pendet ab ison (ut submodulum). Nullae dependentiae externae praeterea. Solum compilator C.

> **Nota:** README.md ignorandum est. Anglice scriptum est ad barbaros ineruditos pecunia emungendos.

## Aedificatio

```
face omnia
```

## Usus

```
./insinulint [-s insinulint.ison] fasciculus.c [fasciculus2.c ...]
```

Si `-s` non datur, `./insinulint.ison` in directorio currenti quaeritur. Si non invenitur, defalta applicantur (4 spatia, K&R, 80 columnae).

## Regulae

| Regula | Quid inspicit |
|---|---|
| `indentatio` | Gradus bracchiorum `{}`, continuatio per `()`, case/default, labels, corpus sine bracchiis |
| `spatium_verbum` | Spatium post `if`, `for`, `while`, `switch`, `return`, `case` |
| `spatium_operator` | Spatia circa `=` `==` `!=` `<=` `>=` `+=` `-=` `*=` `/=` `%=` `&&` `||` etc. |
| `spatium_virgula` | Spatium post `,` necessarium, ante `,` vetitum |
| `spatium_semicolon` | Spatium ante `;` vetitum (excepto `for ( ; ; )`) |
| `bracchia_stilus` | K&R vel Allman — functiones a structuris controli distinguit |
| `bracchia_necessaria` | Bracchia post `if`/`for`/`while`/`else`/`do` |
| `cubitus` | `} else {` coniunctum vel separatum |
| `longitudo_lineae` | Longitudo maxima lineae |
| `spatia_terminalia` | Spatia ad finem lineae |
| `lineae_vacuae` | Lineae vacuae consecutivae |
| `finis_lineae` | Fasciculus finit cum `\n` |
| `tabulae_mixtae` | Tabulae et spatia mixta in indentatione |

## Continuatio parenthesium

Duo modi sustentantur per `indentatio.continuatio`:

```
/* "congruens" (aligned) — defaltum */
functio(primus_argumentum,
        secundum_argumentum,
        tertium_argumentum);

/* "massa" (block) */
functio(
    primus_argumentum,
    secundum_argumentum,
    tertium_argumentum
);
```

## Speculum

Configuratio per plicam ISON. Omnes campi optionales sunt — defalta supplentur.

```
{
    "indentatio": {
        "latitudo": 4,
        "modus": "spatia",
        "continuatio": "congruens"
    },
    "bracchia": {
        "stilus": "kr",
        "else_coniunctum": 1,
        "necessaria": 1
    },
    "spatia": {
        "post_verba_clavis": 1,
        "circa_operatores": 1,
        "post_virgulam": 1,
        "ante_semicolon": 1
    },
    "lineae": {
        "longitudo_maxima": 80,
        "spatia_terminalia": 1,
        "vacuae_max": 2,
        "finis_linea_nova": 1,
        "tabulae_mixtae": 1
    }
}
```

### Valores

| Campus | Valores | Defaltum |
|---|---|---|
| `indentatio.latitudo` | 1–16 | 4 |
| `indentatio.modus` | `"spatia"` `"tabulae"` | `"spatia"` |
| `indentatio.continuatio` | `"congruens"` `"massa"` | `"congruens"` |
| `bracchia.stilus` | `"kr"` `"allman"` | `"kr"` |
| `bracchia.else_coniunctum` | 0/1 | 1 |
| `bracchia.necessaria` | 0/1 | 1 |
| `spatia.post_verba_clavis` | 0/1 | 1 |
| `spatia.circa_operatores` | 0/1 | 1 |
| `spatia.post_virgulam` | 0/1 | 1 |
| `spatia.ante_semicolon` | 0/1 | 1 |
| `lineae.longitudo_maxima` | 0 = sine limite | 80 |
| `lineae.spatia_terminalia` | 0/1 | 1 |
| `lineae.vacuae_max` | 0 = sine limite | 2 |
| `lineae.finis_linea_nova` | 0/1 | 1 |
| `lineae.tabulae_mixtae` | 0/1 | 1 |

## Exemplum

```
$ ./insinulint -s insinulint.ison malus.c
malus.c:10:0: monitum [indentatio] 6 spatia inventa, 4 expectata (prof. 1, par. 0)
malus.c:26:4: monitum [spatium_verbum] spatium requiritur post 'if'
malus.c:43:9: monitum [spatium_operator] spatium requiritur ante '='
malus.c:56:25: monitum [spatium_virgula] spatium requiritur post virgulam
malus.c:72:4: monitum [bracchia_necessaria] bracchia necessaria post 'if'
malus.c:93:4: monitum [cubitus] 'else' debet esse in eadem linea ac '}'
malus.c:102:4: monitum [bracchia_stilus] '{' debet esse in eadem linea (stilus K&R)

insinulint: 7 monita inventa.
```

Exitus 0 si nulla monita, 1 si monita inventa.

## Plicae

```
insinulint.h    caput principale — typi, lexator, inspector, speculum
lexator.c       dissector fontis C in signa (tokens)
inspectio.c     regulae inspectionis et lector speculi
principale.c    main() et integrator
insinulint.ison configuratio exemplaris
Faceplica       face omnia / face purga
```
