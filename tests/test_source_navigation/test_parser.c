/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_source_navigation/test_parser.c
 *
 * PURPOSE:
 *   Exercise real compiler and test-output location syntax without file access.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/failure_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)

static void Parsed(const char *input, const char *path, size_t line, size_t column)
{
    UmiCompilerDiagnosticFields fields;
    OK(UmiTestFailureParseText(input, &fields));
    CHECK(strcmp(fields.path, path) == 0 && fields.line == line && fields.column == column);
}
static void Rejected(const char *input, UmiStatus expected)
{
    UmiCompilerDiagnosticFields fields, before;
    memset(&fields, 0x5a, sizeof(fields)); memcpy(&before, &fields, sizeof(fields));
    CHECK(UmiTestFailureParseText(input, &fields) == expected);
    CHECK(memcmp(&fields, &before, sizeof(fields)) == 0);
}
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1];
    if (strcmp(name, "assertions") == 0) {
        Parsed("src/check.c:42: expected == actual", "src/check.c", 42, 0);
        Parsed("/work/a.c:12:7: REQUIRE(false)", "/work/a.c", 12, 7);
        Parsed("tests/test.c(28): capture != NULL", "tests/test.c", 28, 0);
        Parsed("check.c(4,2): assertion", "check.c", 4, 2);
    } else if (strcmp(name, "windows") == 0) {
        Parsed("C:/umicom/Umicom-Applications/framework/tests/setup_centre/test_process.c:29: strstr(output, text)",
            "C:/umicom/Umicom-Applications/framework/tests/setup_centre/test_process.c", 29, 0);
        Parsed("C:\\Program Files (test)\\case.c(70,9): assertion", "C:\\Program Files (test)\\case.c", 70, 9);
        Parsed("C:/work/caf\xc3\xa9/check.c:8: assertion", "C:/work/caf\xc3\xa9/check.c", 8, 0);
    } else if (strcmp(name, "compiler") == 0) {
        UmiCompilerDiagnosticFields f;
        Parsed("test.c:5:3: error: unknown name", "test.c", 5, 3);
        OK(UmiTestFailureParseText("test.c(7): warning C4100: unused", &f));
        CHECK(f.severity == UMI_DIAGNOSTIC_WARNING && strcmp(f.code, "C4100") == 0);
        Parsed("CMake Error at /work/CMakeLists.txt:91 (message):", "/work/CMakeLists.txt", 91, 0);
    } else if (strcmp(name, "ctest-prefix") == 0) {
        Parsed("205: C:/work/test.c:70: check failed", "C:/work/test.c", 70, 0);
        Parsed("  10842:   tests/capture.c(28): capture != NULL", "tests/capture.c", 28, 0);
        Parsed("7: x.c:3:2: warning: unused", "x.c", 3, 2);
    } else if (strcmp(name, "native-windows") == 0) {
        /* Verify real C runtime spelling with a CTest prefix and a path whose
         * comma is part of the filename. A compiler-like string is expression text. */
        Parsed("Assertion failed: count == 4, file tests\\check.c, line 51", "tests\\check.c", 51, 0);
        UmiCompilerDiagnosticFields fields;
        OK(UmiTestFailureParseText("12: Assertion failed: same(\"error:\"), file C:\\work, line notes\\caf\xc3\xa9.c, line 81\r\n",
            &fields));
        CHECK(strcmp(fields.path, "C:\\work, line notes\\caf\xc3\xa9.c") == 0);
        CHECK(fields.line == 81U && fields.column == 0U && fields.severity == UMI_DIAGNOSTIC_ERROR);
        CHECK(strcmp(fields.message, "same(\"error:\")") == 0 && fields.code[0] == '\0');
    } else if (strcmp(name, "native-glibc") == 0) {
        /* glibc adds an executable and a function ahead of the expression.
         * Neither is part of the stored source path. */
        Parsed("fixture: /work/caf\xc3\xa9/check.c:12: main: Assertion `count == 4' failed.",
            "/work/caf\xc3\xa9/check.c", 12, 0);
        Parsed("19: fixture: tests/check.c:42: Assertion `count == 4' failed.", "tests/check.c", 42, 0);
        UmiCompilerDiagnosticFields fields;
        OK(UmiTestFailureParseText("fixture suite: tests/check.cpp:15: void tool::suite(): Assertion `count == 4' failed.",
            &fields));
        CHECK(strcmp(fields.path, "tests/check.cpp") == 0 && fields.line == 15U);
        CHECK(strcmp(fields.message, "count == 4") == 0 && fields.severity == UMI_DIAGNOSTIC_ERROR);
    } else if (strcmp(name, "native-malformed") == 0) {
        /* Incomplete, ambiguous and non-local messages must not replace the
         * previous output or turn a guessed filename into a navigation target. */
        const char *inputs[] = {
            "Assertion failed: x, file tests/check.c, line 0",
            "Assertion failed: x, file tests/check.c, line 8 extra",
            "Assertion failed: x, file /work, file notes/check.c, line 8",
            "Assertion failed: x, file https://demo.test/remote.c, line 42",
            "fixture: tests/check.c:29: main: Assertion `count == 4'",
            "fixture: https://demo.test/check.c:29: main: Assertion `count == 4' failed.",
            "fixture: tests/check.c:29: main: Assertion `' failed.",
            "fixture: tests/check.c:29:4: main: Assertion `x' failed."
        };
        for (size_t i = 0U; i < sizeof inputs / sizeof inputs[0]; ++i)
            Rejected(inputs[i], UMI_STATUS_NOT_FOUND);
    } else if (strcmp(name, "colours") == 0) {
        Parsed("\x1b[31mtest.c:8: assertion\x1b[0m\x1b[K\r\nignored.c:99: later", "test.c", 8, 0);
        Rejected("\x1b[2Jtest.c:8: assertion", UMI_STATUS_NOT_FOUND);
        Rejected("test.c:8: \x1b[", UMI_STATUS_NOT_FOUND);
    } else if (strcmp(name, "unknown") == 0) {
        const char *texts[] = {"check failed at line 70", "line 28: capture != NULL", "8181/10896 Test #7092: ***Failed", "https://example.test/x.c:5: nope", "C:relative/test.c:5: nope", "", "error: no source"};
        for (size_t i=0; i<sizeof(texts)/sizeof(texts[0]); ++i) Rejected(texts[i], UMI_STATUS_NOT_FOUND);
    } else if (strcmp(name, "invalid-positions") == 0) {
        const char *texts[] = {"test.c:0: assertion", "test.c:-1: assertion", "test.c:+1: assertion", "test.c:2:0: assertion", "test.c(1,-2): assertion", "test.c:9999999999999999999999999999: assertion"};
        for (size_t i=0; i<sizeof(texts)/sizeof(texts[0]); ++i) Rejected(texts[i], UMI_STATUS_NOT_FOUND);
    } else if (strcmp(name, "capacity") == 0) {
        char text[10000]; memset(text, 'x', 2050); strcpy(text+2050, ".c:2: check");
        Rejected(text, UMI_STATUS_CAPACITY_EXCEEDED);
        strcpy(text, "test.c:2: "); memset(text+10, 'x', 1100); text[1110]='\0';
        Rejected(text, UMI_STATUS_CAPACITY_EXCEEDED);
        memset(text, 'x', sizeof(text)-1); text[sizeof(text)-1]='\0';
        Rejected(text, UMI_STATUS_CAPACITY_EXCEEDED);
        /* The CRT grammar applies the same all-or-nothing field bounds. */
        strcpy(text, "Assertion failed: ");
        size_t count = strlen(text);
        memset(text + count, 'x', 1100U);
        strcpy(text + count + 1100U, ", file tests/check.c, line 12");
        Rejected(text, UMI_STATUS_CAPACITY_EXCEEDED);
    } else if (strcmp(name, "arguments") == 0) {
        Rejected(NULL, UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTestFailureParseText("test.c:2: check", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    return 0;
}
