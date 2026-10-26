/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/source_contracts/test_literals.c
 *
 * PURPOSE:
 *   Distinguish malformed C literals from valid comments, splices and separators.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); return 1; } } while (0)
typedef struct LiteralDiagnostic { size_t calls, line; } LiteralDiagnostic;
static void Diagnostic(void *context, const char *path, size_t line,
                        const char *rule, const char *message) {
    LiteralDiagnostic *d = context;
    if (!strcmp(path, "sample.c") && !strcmp(rule, "source.lexical") && message[0] != '\0') {
        ++d->calls; d->line = line;
    }
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    const char *mode = argv[1], *source = NULL;
    int expected = 0;
    size_t expectedLine = 1U;
    if (!strcmp(mode, "escaped"))
        source = "const char *x = \"first\\nsecond\\\"quote\\\\slash\"; int y = '\\'';";
    else if (!strcmp(mode, "splice-lf"))
        source = "const char *x = \"first\\\nsecond\"; /\\\n/ continued comment\\\n\"unclosed\n";
    else if (!strcmp(mode, "splice-crlf"))
        source = "const char *x = \"first\\\r\nsecond\"; /\\\r\n* \"unclosed\r\n*/";
    else if (!strcmp(mode, "numeric-separators"))
        source = "int x = 1'000; int y = 0xAB'CD; int z = 0b10'01; double f = .1'2e+3;";
    else if (!strcmp(mode, "comments"))
        source = "/* original \"broken\n literal */\n// 'broken\nconst char *s = \"/* not comment */\";";
    else if (!strcmp(mode, "adjacent"))
        source = "const char *x = u8\"first\" \n \"second\"; int c = L'c';";
    else if (!strcmp(mode, "raw-string")) { source = "const char *x = \"broken\nliteral\";"; expected = 1; }
    else if (!strcmp(mode, "raw-character")) { source = "int x = '\n';"; expected = 1; }
    else if (!strcmp(mode, "raw-crlf")) { source = "const char *x = \"broken\r\nliteral\";"; expected = 1; }
    else if (!strcmp(mode, "raw-cr")) { source = "const char *x = \"broken\rliteral\";"; expected = 1; }
    else if (!strcmp(mode, "unterminated-string")) { source = "const char *x = \"broken"; expected = 1; }
    else if (!strcmp(mode, "unterminated-character")) { source = "int x = 'a"; expected = 1; }
    else if (!strcmp(mode, "unterminated-comment")) { source = "/* never closed"; expected = 1; }
    else if (!strcmp(mode, "disabled-malformed")) {
        source = "#if 0\nconst char *x = \"broken\nliteral\";\n#endif";
        expected = 1; expectedLine = 2U;
    } else if (!strcmp(mode, "physical-location")) {
        source = "#define M \\\n  1\nconst char *x = \"broken\nliteral\";";
        expected = 1; expectedLine = 3U;
    } else if (!strcmp(mode, "invalid-api")) {
        UmiSourceContractReport report = {0};
        CHECK(ScLiterals(NULL, "sample.c", &report) == 2);
        CHECK(ScLiterals("", NULL, &report) == 2);
        CHECK(ScLiterals("", "sample.c", NULL) == 2);
        CHECK(ScLiterals("", "sample.c", &report) == 0 && report.findings == 0U);
        return 0;
    } else CHECK(false);
    LiteralDiagnostic d = {0};
    UmiSourceContractReport report = {.diagnostic = Diagnostic, .context = &d};
    CHECK(ScLiterals(source, "sample.c", &report) == expected);
    CHECK(report.input_errors == 0U);
    CHECK(report.findings == (expected == 0 ? 0U : 1U));
    CHECK(d.calls == report.findings);
    if (expected != 0) CHECK(d.line == expectedLine);
    return 0;
}
