# insinulint

### The Last C Linter You Will Ever Need. Possibly The Last C Linter Anyone Will Ever Need.

**insinulint** isn't just a linter. It's a *movement*. A *philosophy*. A quiet revolution in how humanity relates to the C programming language.

We didn't build insinulint because the world needed another linter. We built it because the world needed a *better* one. And then we built that one, too. And then we kept going.

## The Problem

You've used other linters. We both know this. And we both know how that went.

They were slow. They had dependencies. They wanted you to install Node.js to lint *C code*. They couldn't tell a function definition from a control structure. They choked on continuation lines. They gave up on braceless `if` bodies. They made you feel nothing.

**That era is over.**

## The Solution

**insinulint** is approximately 1700 lines of hand-written, zero-dependency, pure C99 that compiles in under a second and lints your code with the kind of precision and moral clarity that the industry has been crying out for.

No plugins. No extensions. No ecosystem. No community Discord. No GitHub Sponsors tier. Just a C compiler, `make`, and the truth.

### Feature Highlights

- **13 comprehensive lint rules** — not 12, not 14. Thirteen. The exact right number. We ran the analysis.
- **Two continuation alignment modes** (aligned and block) — because we solved the parenthesized argument indentation problem while the rest of the industry was still publishing Medium articles about it
- **K&R vs Allman brace detection** that *actually distinguishes function definitions from control structures* — a problem most tools pretend doesn't exist, because they couldn't solve it
- **Cuddled else enforcement** — your team's `} else {` debate ends here. You're welcome.
- **Virtual indentation for braceless bodies** — `if (x)\n    return;` is handled correctly. Yes, really. No, the other tools can't do this. We checked.
- **Zero dependencies** — no npm. No pip. No cargo. No vcpkg. No conan. No brew formula. No Docker image. A C compiler and make. Like the founders intended.
- **Hand-rolled JSON parser** — we didn't even depend on a JSON library. We wrote our own. With a recursive descent navigator that resolves dotted paths. Because we could.
- **Sub-second compilation** — by the time your previous linter finished installing its dependencies, insinulint has already compiled, linted your entire project, found seventeen style violations, and is quietly judging you

## Quick Start

```
make omnia
./insinulint source.c
```

That's it. That's the quick start. Other tools need a page and a half. We need two lines. Draw your own conclusions.

With a configuration file:

```
./insinulint -s speculum.json source.c [source2.c ...]
```

## Rules

| Rule | What It Catches | Could Other Tools Do This? |
|---|---|---|
| `indentatio` | Brace-depth indentation, parenthesis continuation (aligned/block), case/default, labels, braceless bodies | Not all of it, no |
| `spatium_verbum` | Missing space after `if`, `for`, `while`, `switch`, `return`, `case` | Probably |
| `spatium_operator` | Missing spaces around `=` `==` `!=` `<=` `>=` `+=` `-=` `*=` `/=` `%=` `&&` `\|\|` etc. | In theory |
| `spatium_virgula` | Missing space after `,` or unwanted space before `,` | One would hope |
| `spatium_semicolon` | Unwanted space before `;` (with `for ( ; ; )` exception) | The exception is the hard part |
| `bracchia_stilus` | K&R vs Allman brace placement — distinguishes functions from control structures | They can't, actually |
| `bracchia_necessaria` | Missing braces on `if`/`for`/`while`/`else`/`do` bodies | Theoretically |
| `cubitus` | Cuddled or uncuddled `else` | Debatable |
| `longitudo_lineae` | Maximum line length | Fine, yes |
| `spatia_terminalia` | Trailing whitespace | Okay this one is easy |
| `lineae_vacuae` | Excessive consecutive blank lines | Sure |
| `finis_lineae` | File must end with newline | Admittedly straightforward |
| `tabulae_mixtae` | Mixed tabs and spaces in indentation | Alright |

## Configuration

Create a `speculum.json`:

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

All fields are optional. Sensible defaults are provided, because we spent considerable time thinking about what those defaults should be so that you — and we mean this with the utmost respect — don't have to.

| Field | Values | Default |
|---|---|---|
| `indentatio.latitudo` | 1–16 | 4 |
| `indentatio.modus` | `"spatia"` / `"tabulae"` | `"spatia"` |
| `indentatio.continuatio` | `"congruens"` (aligned) / `"massa"` (block) | `"congruens"` |
| `bracchia.stilus` | `"kr"` / `"allman"` | `"kr"` |
| `bracchia.else_coniunctum` | 0 / 1 | 1 |
| `bracchia.necessaria` | 0 / 1 | 1 |
| `spatia.post_verba_clavis` | 0 / 1 | 1 |
| `spatia.circa_operatores` | 0 / 1 | 1 |
| `spatia.post_virgulam` | 0 / 1 | 1 |
| `spatia.ante_semicolon` | 0 / 1 | 1 |
| `lineae.longitudo_maxima` | 0 = unlimited | 80 |
| `lineae.spatia_terminalia` | 0 / 1 | 1 |
| `lineae.vacuae_max` | 0 = unlimited | 2 |
| `lineae.finis_linea_nova` | 0 / 1 | 1 |
| `lineae.tabulae_mixtae` | 0 / 1 | 1 |

## Example Output

```
$ ./insinulint -s speculum.json main.c
main.c:10:0: monitum [indentatio] 6 spatia inventa, 4 expectata (prof. 1, par. 0)
main.c:26:4: monitum [spatium_verbum] spatium requiritur post 'if'
main.c:43:9: monitum [spatium_operator] spatium requiritur ante '='
main.c:72:4: monitum [bracchia_necessaria] bracchia necessaria post 'if'
main.c:93:4: monitum [cubitus] 'else' debet esse in eadem linea ac '}'

insinulint: 5 monita inventa.
```

Exit code 0 means your code is worthy. Exit code 1 means it isn't. There is no exit code 2. You either pass or you don't. Life is binary. So is insinulint.

## Architecture

```
insinulint.h    header — types, lexer, inspector, config
lexator.c       C source tokenizer (recursive descent)
inspectio.c     13 inspection rules + config reader
principale.c    main() and orchestration
json.c          JSON parser (hand-written, dotted path navigator)
json.h          JSON header
speculum.json   example configuration
Makefile        make omnia / make purga
```

Six source files. One header. One config. One Makefile. This is what software looks like when you refuse to compromise. Take notes.

## FAQ

**Q: Can it lint C++?**
A: No. If you're writing C++, you've already made choices that insinulint cannot help you with.

**Q: Can it lint C11/C17/C23?**
A: It lints C. The C that built Unix, that built the internet, that put men on the moon. If your code compiles with `cc -std=c99`, insinulint will judge it. The newer standards are welcome to earn our support in due time.

**Q: What about auto-fix?**
A: The purpose of a linter is to make you a better programmer. Auto-fix makes you a more complacent one. We will not be party to your moral decline.

**Q: It flagged my code but my code is correct.**
A: Your code compiles. That is not the same as being correct. Sit with that for a moment.

**Q: Why doesn't it have a VS Code extension?**
A: Because it has *self-respect*.

**Q: I want to contribute.**
A: We'll consider it.

**Q: How does this compare to clang-tidy?**
A: clang-tidy is a fine tool for people who enjoy waiting. insinulint is for people who enjoy results. Also, our binary is smaller than clang-tidy's *help text*.

**Q: Is this production-ready?**
A: insinulint was born production-ready. The question is whether your codebase is ready for insinulint.
