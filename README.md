# insinulint

**Thirteen meticulously crafted lint rules for C, implemented in approximately 1,700 lines of zero-dependency C99. Compiles in under a second. Runs faster than your previous linter installs.**

## Why insinulint Exists

Most C linters are either absurdly heavyweight — pulling in Clang's AST machinery for what amounts to whitespace checking — or absurdly crude, matching patterns with no understanding of the language's structure. insinulint occupies the space between these two failures: a purpose-built lexer that understands enough C to enforce style rules correctly, and nothing more.

It distinguishes function definitions from control structures for brace style enforcement. It tracks parenthesization depth for continuation alignment. It handles `case`, `default`, labels, single-statement bodies, `do`/`while`, and every other indentation edge case that naive linters get wrong. And it does all of this in a single, self-contained binary with no runtime dependencies whatsoever.

## Rules

| Rule | What It Enforces |
|---|---|
| `indentatio` | Brace depth, continuation inside `()`, case/default, labels, braceless bodies |
| `spatium_verbum` | Space after `if`, `for`, `while`, `switch`, `return`, `case` |
| `spatium_operator` | Spaces around `=` `==` `!=` `<=` `>=` `+=` `-=` `*=` `/=` `%=` `&&` `\|\|` |
| `spatium_virgula` | Space after `,` required, before `,` forbidden |
| `spatium_semicolon` | Space before `;` forbidden (except `for ( ; ; )`) |
| `bracchia_stilus` | K&R or Allman — with correct function-vs-control distinction |
| `bracchia_necessaria` | Braces required after `if`/`for`/`while`/`else`/`do` |
| `cubitus` | `} else {` joined or separated |
| `longitudo_lineae` | Maximum line length |
| `spatia_terminalia` | Trailing whitespace |
| `lineae_vacuae` | Consecutive blank lines |
| `finis_lineae` | File ends with `\n` |
| `tabulae_mixtae` | Mixed tabs and spaces in indentation |

## Building

```bash
make omnia
```

## Usage

```bash
./insinulint [-s speculum.ison] file.c [file2.c ...]
```

Without a configuration file, sensible defaults apply: 4 spaces, K&R braces, 80 columns.

## Configuration

Fully configurable via a ISON specification file. Every field is optional — defaults are supplied for anything you don't specify:

```json
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

## Continuation Alignment

Two modes, selectable via `indentatio.continuatio`:

```c
/* "congruens" (aligned) — default */
function(first_argument,
         second_argument,
         third_argument);

/* "massa" (block) */
function(
    first_argument,
    second_argument,
    third_argument
);
```

## Example Output

```text
file.c:10:0: warning [indentatio] 6 spaces found, 4 expected (depth 1, paren 0)
file.c:26:4: warning [spatium_verbum] space required after 'if'
file.c:43:9: warning [spatium_operator] space required before '='
file.c:56:25: warning [spatium_virgula] space required after comma
file.c:72:4: warning [bracchia_necessaria] braces required after 'if'
file.c:93:4: warning [cubitus] 'else' must be on same line as '}'
file.c:102:4: warning [bracchia_stilus] '{' must be on same line (K&R style)

insinulint: 7 warnings found.
```

Exit 0 if clean. Exit 1 if warnings found.

## License

Free. Public domain. Use however you like.
