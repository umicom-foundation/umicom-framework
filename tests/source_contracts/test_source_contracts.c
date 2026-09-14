/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/source_contracts/test_source_contracts.c
 *
 * PURPOSE:
 *   Exercise the native qualification engine with valid and deliberately
 *   damaged attribution, C syntax, window identity, preset graphs and files.
 *   Fixtures are written only under the explicit CTest-owned directory.
 *
 * Author: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#endif

static size_t assertions;
#define CHECK(condition) do { ++assertions; if (!(condition)) { \
    (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)

/* Return from a failed check, never from a production helper or a callback. */
static int metadata(void)
{
    const char *valid[] = {
        "/* File: a.c\nPurpose: Explain.\nAuthor: Alice\nLicence: MIT\n*/",
        "/* File: a.c\nPurpose: Explain.\nOrganisation: Example team\nLicence: BSD-2-Clause\n*/",
        "/* File: a.c\nPurpose: Explain.\nAuthor: Sammy Hegab\nOrganisation: Umicom Foundation\nLicence: MIT\n*/",
        "/*-----------------------------------------------------------------------------\n * Umicom Desk Module\n * File: tests/test_productisation_contribution.c\n *\n * PURPOSE:\n *   Verify canonical Framework product surfaces.\n *\n * AUTHOR AND ORGANISATION:\n * Sammy Hegab\n * Umicom Foundation\n *\n * LICENCE:\n * MIT\n *---------------------------------------------------------------------------*/",
        "\xEF\xBB\xBF /* File : a.c\r\n * PuRpOsE : Explain.\r\n * Author :\r\n * Alice\r\n * Licence : MIT\r\n */",
        "/* File: a.c\nPurpose: Explain.\n AUTHOR  AND  ORGANISATION : Alice\nLicence: MIT\n*/",
        "/* File: a.c\nPurpose: Explain.\nOrganization: Team\nLicense: MIT\n*/",
        "/* File: a.c\nPurpose: Explain.\nCreated by: Contributor\nSPDX-License-Identifier: MIT\n*/",
        "/* File: a.c\nPurpose: Explain.\nAuthor:\nOrganisation: Team\nLicence: MIT\n*/"
    };
    for (size_t i = 0U; i < sizeof(valid) / sizeof(valid[0]); ++i) {
        UmiSourceAttribution a = {0}; UmiSourceContractReport report = {0};
        CHECK(UmiSourceContractReadAttribution(valid[i], strlen(valid[i]), &a) == 0);
        CHECK(a.leading_comment && a.file && a.purpose && a.licence);
        CHECK(a.author || a.organisation || a.combined);
        CHECK(ScMetadata(valid[i], strlen(valid[i]), "fixture.c", &report) == 0);
        CHECK(report.findings == 0U);
    }
    const char *invalid[] = {
        "/* File: a.c\nPurpose: Explain.\nAuthor:\nLicence: MIT\n*/",
        "/* File: a.c\nPurpose: Explain.\nAuthor:\n * ----\nOrganisation:\nLicence: MIT\n*/",
        "/* File: a.c\nPurpose: Explain.\nAuthor:\nUnknown heading: Not an author\nLicence: MIT\n*/",
        "/* File: a.c\nPurpose: Explain.\nAUTHOR AND ORGANISATION:\nLicence: MIT\n*/",
        "/* File: a.c\nPurpose: Explain.\nLicence: MIT\n*/\nconst char *author = \"Author: Alice\";"
    };
    for (size_t i = 0U; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        UmiSourceAttribution a = {0}; UmiSourceContractReport report = {0};
        CHECK(UmiSourceContractReadAttribution(invalid[i], strlen(invalid[i]), &a) == 0);
        CHECK(!a.author && !a.organisation && !a.combined);
        CHECK(ScMetadata(invalid[i], strlen(invalid[i]), "fixture.c", &report) == 0);
        CHECK(report.findings == 1U);
    }
    UmiSourceAttribution a = {0};
    CHECK(UmiSourceContractReadAttribution("int a; /* Author: Alice */", 25U, &a) == 1);
    CHECK(UmiSourceContractReadAttribution("/* Author: Alice", 16U, &a) == 2);
    CHECK(UmiSourceContractReadAttribution("/*x*/\0", 6U, &a) == 2);
    CHECK(UmiSourceContractReadAttribution(NULL, 0U, &a) == 2);
    CHECK(UmiSourceContractReadAttribution("", 0U, NULL) == 2);
    char *guard = NULL; UmiSourceContractReport report = {0};
    CHECK(ScGuard("#ifndef EXAMPLE_H\n#define EXAMPLE_H\n#endif", "x.h", &guard, &report) == 0);
    CHECK(guard != NULL && strcmp(guard, "EXAMPLE_H") == 0); free(guard);
    guard = NULL; report = (UmiSourceContractReport){0};
    CHECK(ScGuard("#ifndef EXAMPLE_H\n#define DIFFERENT_H\n#endif", "x.h", &guard, &report) == 1);
    CHECK(report.findings == 1U); free(guard);
    guard = NULL; report = (UmiSourceContractReport){0};
    CHECK(ScGuard("/* #ifndef FAKE_H\n#define FAKE_H */\nint x;", "x.h", &guard, &report) == 1);
    CHECK(guard == NULL);
    return 0;
}

/* Balance real nested expressions, never parentheses inside comments/literals. */
static int lexer(void)
{
    ScText text = {0}, body = {0};
    const char *input = "void target(int x);\n/* target(){fake();} */\n"
        "const char *s=\"target(){fake();}\";\n"
        "void target(int x) { call( GTK_WINDOW ( (state->window) ), \"a ) }\", '\\''); /* } */ }";
    CHECK(ScLex(&text, input, false) == 0);
    CHECK(ScBody(&text, "target", &body) == 0);
    CHECK(ScContains(&body, "call(GTK_WINDOW((state->window)),"));
    CHECK(!ScContains(&body, "fake("));
    CHECK(ScCount(&body, "call(") == 1U);
    size_t call = ScIndex(&body, "call(", 0U);
    CHECK(call != SIZE_MAX && ScMatching(&body, call + 1U) != SIZE_MAX);
    ScTextFree(&body); ScTextFree(&text);
    CHECK(ScLex(&text, "const char *x = \"invoke(real)\"; // invoke(fake)\n invoke(real);", false) == 0);
    CHECK(ScCount(&text, "invoke(") == 1U); ScTextFree(&text);
    CHECK(ScLex(&text, "// hidden \\\n still_hidden();\nvisible();", false) == 0);
    CHECK(!ScContains(&text, "still_hidden(")); CHECK(ScContains(&text, "visible(")); ScTextFree(&text);
    CHECK(ScLex(&text, "#[=[ ignore(add_subdirectory(fake)) ]=]\n# line\n target_sources(x PRIVATE \"a.c\")", true) == 0);
    CHECK(!ScContains(&text, "add_subdirectory(")); CHECK(ScContains(&text, "target_sources(")); ScTextFree(&text);
    CHECK(ScLex(&text, "\"unfinished", false) == 2);
    CHECK(ScLex(&text, "/* unfinished", false) == 2);
    CHECK(ScLex(&text, "#[=[ unfinished", true) == 2);
    CHECK(ScLex(&text, "([)]", false) == 0); CHECK(ScMatching(&text, 0U) == SIZE_MAX); ScTextFree(&text);
    CHECK(ScLex(&text, "void declared(void);", false) == 0);
    CHECK(ScBody(&text, "declared", &body) == 1); ScTextFree(&text);
    CHECK(ScLex(&text, "f(\"\\\" ))\", g(a[0],(b)));", false) == 0);
    CHECK(ScMatching(&text, 1U) == text.count - 2U); ScTextFree(&text);
    return 0;
}

static int identityResult(const char *source, bool studio, int expected)
{
    UmiSourceContractReport report = {0};
    int result = UmiSourceContractWindowIdentity(source, "identity.c", studio, &report);
    CHECK(result == expected);
    CHECK(expected == 0 ? report.findings == 0U : report.findings > 0U);
    return 0;
}
static int identity(void)
{
    CHECK(identityResult("desk->window=gtk_window_new();\n"
        "(void)umi_gtk4_ws_apply_window_identity(GTK_WINDOW(desk->window));", false, 0) == 0);
    CHECK(identityResult("state->window=GTK_WINDOW(gtk_application_window_new(app));"
        "state->startup_window=GTK_WINDOW(gtk_application_window_new(app));"
        "umi_gtk4_ws_apply_window_identity(state->window);"
        "umi_gtk4_ws_apply_window_identity(GTK_WINDOW((state->startup_window)));", false, 0) == 0);
    CHECK(identityResult("state->window=GTK_WINDOW(gtk_application_window_new(app));"
        "state->startup_window=GTK_WINDOW(gtk_application_window_new(app));"
        "umi_gtk4_ws_apply_window_identity(state->window);", false, 1) == 0);
    CHECK(identityResult("w=gtk_window_new();"
        "/* umi_gtk4_ws_apply_window_identity(w); */"
        "const char *s=\"umi_gtk4_ws_apply_window_identity(w)\";", false, 1) == 0);
    CHECK(identityResult("w=gtk_window_new();"
        "umi_gtk4_ws_apply_window_identity(other);", false, 1) == 0);
    CHECK(identityResult("umi_gtk4_ws_apply_window_identity(runtime->owner->window);", true, 0) == 0);
    CHECK(identityResult("umi_gtk4_ws_apply_window_identity(other);", true, 1) == 0);
    CHECK(identityResult("desk->window=gtk_window_new();"
        "umi_gtk4_ws_apply_window_identity(GTK_WINDOW(GTK_WIDGET((desk->window))));", false, 0) == 0);
    CHECK(identityResult("void umi_gtk4_ws_apply_window_identity(GtkWindow *w); w=gtk_window_new();", false, 1) == 0);
    CHECK(identityResult("/* broken", false, 2) == 0);
    return 0;
}

/* The seven original inheritance concerns remain, alongside malformed and
 * shadowing cases. The real Framework JSON reader is linked, not mocked. */
static int presetValue(const char *json, const char *preset, int expected, const char *value)
{
    char out[96] = "sentinel";
    CHECK(UmiSourceContractPresetValue(json, preset, "K", out, sizeof(out)) == expected);
    CHECK(expected != 0 || strcmp(out, value) == 0);
    return 0;
}
static int presets(void)
{
    CHECK(presetValue("[{\"name\":\"base\",\"cacheVariables\":{\"K\":\"OFF\"}},"
        "{\"name\":\"child\",\"inherits\":\"base\"}]", "child", 0, "OFF") == 0);
    CHECK(presetValue("[{\"name\":\"base\",\"cacheVariables\":{\"K\":\"OFF\"}},"
        "{\"name\":\"child\",\"inherits\":\"base\",\"cacheVariables\":{\"K\":\"ON\"}}]", "child", 0, "ON") == 0);
    CHECK(presetValue("[{\"name\":\"a\",\"cacheVariables\":{\"K\":\"FIRST\"}},"
        "{\"name\":\"b\",\"cacheVariables\":{\"K\":\"SECOND\"}},"
        "{\"name\":\"c\",\"inherits\":[\"a\",\"b\"]}]", "c", 0, "FIRST") == 0);
    CHECK(presetValue("[{\"name\":\"a\",\"cacheVariables\":{\"K\":null}},"
        "{\"name\":\"b\",\"cacheVariables\":{\"K\":\"ON\"}},"
        "{\"name\":\"c\",\"inherits\":[\"a\",\"b\"]}]", "c", 1, "") == 0);
    CHECK(presetValue("[{\"name\":\"a\",\"inherits\":\"b\"},{\"name\":\"b\",\"inherits\":\"a\"}]", "a", 2, "") == 0);
    CHECK(presetValue("[{\"name\":\"a\",\"inherits\":\"missing\",\"cacheVariables\":{\"K\":\"OFF\"}}]", "a", 2, "") == 0);
    CHECK(presetValue("[{\"name\":\"a\"},{\"name\":\"a\"}]", "a", 2, "") == 0);
    CHECK(presetValue("[{\"name\":\"a\",\"inherits\":42}]", "a", 2, "") == 0);
    CHECK(presetValue("[{\"name\":\"a\",\"cacheVariables\":{\"K\":false}}]", "a", 0, "OFF") == 0);
    CHECK(presetValue("[{\"name\":\"a\",\"cacheVariables\":{\"K\":{\"type\":\"BOOL\",\"value\":true}}}]", "a", 0, "ON") == 0);
    CHECK(presetValue("[{\"name\":\"a\",\"cacheVariables\":{\"K\":7}}]", "a", 2, "") == 0);
    CHECK(presetValue("[{\"name\":\"a\",\"cacheVariables\":{\"K\":{}}}]", "a", 2, "") == 0);
    CHECK(presetValue("[{\"name\":\"a\",\"cacheVariables\":{\"K\":\"OFF\",\"K\":\"ON\"}}]", "a", 2, "") == 0);
    CHECK(presetValue("[{\"name\":\"a\",\"cacheVariables\":[]}]", "a", 2, "") == 0);
    CHECK(presetValue("{\"include\":[\"external.json\"],\"configurePresets\":[{\"name\":\"a\"}]}", "a", 2, "") == 0);
    CHECK(presetValue("{\"configurePresets\":[{\"name\":\"a\",\"cacheVariables\":{\"K\":\"OFF\"}}]}", "a", 0, "OFF") == 0);
    CHECK(presetValue("[{\"name\":\"a\",\"cacheVariables\":{\"K\":\"OFF\\u0000ON\"}}]", "a", 2, "") == 0);
    CHECK(presetValue("[{\"name\":\"a\"}]", "unknown", 2, "") == 0);
    CHECK(presetValue("[{\"name\":\"a\"}]", "a", 1, "") == 0);
    const char *nested = "[{\"name\":\"base\",\"cacheVariables\":{\"K\":\"OFF\"}},"
        "{\"name\":\"middle\",\"inherits\":\"base\"},{\"name\":\"leaf\",\"inherits\":\"middle\"}]";
    char *unchanged = ScCopy(nested, strlen(nested)); CHECK(unchanged != NULL);
    CHECK(presetValue(unchanged, "leaf", 0, "OFF") == 0);
    CHECK(strcmp(unchanged, nested) == 0); free(unchanged);
    CHECK(presetValue("[{\"name\":\"a\"},{\"name\":\"b\",\"cacheVariables\":{\"K\":\"OFF\"}},"
        "{\"name\":\"c\",\"inherits\":[\"a\",\"b\"]}]", "c", 0, "OFF") == 0);
    CHECK(presetValue("[{\"name\":\"a\",\"cacheVariables\":{\"K\":\"\\\\u0000\"}}]", "a", 0, "\\u0000") == 0);
    UmiSourceContractReport report = {0};
    const char *good = "{\"configurePresets\":[{\"name\":\"base\",\"cacheVariables\":{"
        "\"UMICOM_DESKTOP_BUILD_GTK\":false,\"UMICOM_STUDIO_BUILD_GTK\":\"OFF\","
        "\"UMICOM_TRADER_BUILD_GTK4\":\"OFF\",\"UMICOM_BANK_BUILD_GTK4\":\"OFF\","
        "\"UMICOM_TMS_BUILD_GTK4\":\"OFF\",\"UMICOM_MUSIC_STUDIO_BUILD_GTK4\":\"OFF\","
        "\"UMICOM_APPLICATIONS_BUILD_SHARED_GTK4\":\"OFF\"}},"
        "{\"name\":\"all-headless\",\"inherits\":\"base\"}]}";
    CHECK(UmiSourceContractAuditPresets(good, "presets.json", &report) == 0);
    CHECK(report.checks == 7U && report.findings == 0U);
    char *damaged = ScCopy(good, strlen(good)); CHECK(damaged != NULL);
    char *value = strstr(damaged, "false"); CHECK(value != NULL); memcpy(value, "true ", 5U);
    report = (UmiSourceContractReport){0};
    CHECK(UmiSourceContractAuditPresets(damaged, "presets.json", &report) == 1);
    CHECK(report.findings == 1U); free(damaged);
    report = (UmiSourceContractReport){0};
    CHECK(UmiSourceContractAuditPresets("[]", "presets.json", &report) == 2);
    return 0;
}

static int directory(const char *path)
{
#ifdef _WIN32
    int result = _mkdir(path);
#else
    int result = mkdir(path, 0700);
#endif
    return result == 0 || errno == EEXIST ? 0 : 1;
}
static int writeText(const char *path, const char *text)
{
    FILE *file = fopen(path, "wb");
    if (file == NULL) return 1;
    size_t n = strlen(text); bool written = fwrite(text, 1U, n, file) == n;
    return fclose(file) == 0 && written ? 0 : 1;
}
static int filesystem(const char *base)
{
    CHECK(base != NULL && base[0] != '\0');
    char *valid = ScJoin(base, "valid"); char *invalid = ScJoin(base, "invalid");
    char *empty = ScJoin(base, "empty"); char *missing = ScJoin(base, "missing-not-created");
    CHECK(valid && invalid && empty && missing);
    CHECK(directory(valid) == 0 && directory(invalid) == 0 && directory(empty) == 0);
    const char *banner = "/* File: fixture\nPurpose: Test fixture.\nOrganisation: Test team\nLicence: MIT\n*/\n";
    char content[512];
    CHECK(snprintf(content, sizeof(content), "%s#ifndef FIXTURE_H\n#define FIXTURE_H\n#endif\n", banner) > 0);
    char *header = ScJoin(valid, "one.h"); char *cfile = ScJoin(valid, "one.c");
    char *header2 = ScJoin(invalid, "two.h"); CHECK(header && cfile && header2);
    CHECK(writeText(header, content) == 0 && writeText(header2, content) == 0);
    CHECK(writeText(cfile, banner) == 0);
    const char *roots[3] = {valid, valid, invalid}; UmiSourceContractReport report = {0};
    CHECK(UmiSourceContractAudit(roots, 2U, true, &report) == 0);
    CHECK(report.files_checked == 1U && report.findings == 0U);
    report = (UmiSourceContractReport){0};
    CHECK(UmiSourceContractAudit(roots, 3U, true, &report) == 1);
    CHECK(report.files_checked == 2U && report.findings == 1U);
    report = (UmiSourceContractReport){0};
    CHECK(UmiSourceContractAudit(roots, 1U, false, &report) == 0);
    CHECK(report.files_checked == 1U);
    roots[0] = missing; report = (UmiSourceContractReport){0};
    CHECK(UmiSourceContractAudit(roots, 1U, false, &report) == 2);
    CHECK(report.input_errors > 0U);
    roots[0] = empty; report = (UmiSourceContractReport){0};
    CHECK(UmiSourceContractAudit(roots, 1U, true, &report) == 2);
    roots[0] = valid; roots[1] = invalid;
    CHECK(writeText(header2, "/* File: f.h\nPurpose: test\nAuthor:\nLicence: MIT\n*/\n#ifndef OTHER_H\n#define OTHER_H\n#endif") == 0);
    report = (UmiSourceContractReport){0};
    CHECK(UmiSourceContractAudit(roots, 2U, true, &report) == 1);
    CHECK(report.findings == 1U);
    char *ignored = ScJoin(valid, "build-fixture"); CHECK(ignored != NULL && directory(ignored) == 0);
    char *ignoredFile = ScJoin(ignored, "bad.h"); CHECK(ignoredFile != NULL);
    CHECK(writeText(ignoredFile, "no comment or guard") == 0);
    report = (UmiSourceContractReport){0};
    CHECK(UmiSourceContractAudit(roots, 1U, true, &report) == 0);
    free(ignoredFile); free(ignored); free(header2); free(cfile); free(header);
    free(missing); free(empty); free(invalid); free(valid);
    return 0;
}

/* These are isolated source-shape fixtures, not working application mocks.
 * They test recipe membership and mutation rejection across all 24 entries. */
static int fixture(const char *base, const char *relative, const char *text)
{
    char *path = ScJoin(base, relative);
    if (path == NULL) return 1;
    size_t offset = strlen(base) + 1U;
    for (size_t i = offset; path[i] != '\0'; ++i) {
        if (path[i] != '/') continue;
        path[i] = '\0'; int result = directory(path); path[i] = '/';
        if (result != 0) { free(path); return 1; }
    }
    int result = writeText(path, text); free(path); return result;
}
static int portfolio(const char *base)
{
    static const char *const names[] = {
        "desktop","studio","trader","bank","tms","music", "accountant",
        "cad","creator","database-studio","education","exchange","games",
        "integration-studio","kitchen","llm","marketplace","media",
        "mobile-studio","operations","os","rag","security-centre","web-studio"
    };
    char catalogue[8192] = "static Getter GETTERS[] = {\n";
    char includes[8192] = "", records[8192] = "", modules[8192] = "";
    for (size_t i = 0U; i < sizeof(names)/sizeof(names[0]); ++i) {
        char text[2048], path[512], fragment[512], symbol[64];
        CHECK(snprintf(symbol, sizeof(symbol), "product%zu", i) > 0);
        CHECK(snprintf(fragment, sizeof(fragment), "umi_application_experience_%s,\n", symbol) > 0);
        CHECK(strlen(catalogue) + strlen(fragment) < sizeof(catalogue)); strcat(catalogue, fragment);
        CHECK(snprintf(fragment, sizeof(fragment), "#include \"%s/standard.inc\"\n", names[i]) > 0);
        strcat(includes, fragment);
        CHECK(snprintf(fragment, sizeof(fragment), "&recipe%zu,\n", i) > 0); strcat(records, fragment);
        CHECK(snprintf(fragment, sizeof(fragment), "add_subdirectory(\"applications/%s\")\n", names[i]) > 0); strcat(modules, fragment);
        CHECK(snprintf(path, sizeof(path), "framework/src/application/experiences/%s.c", symbol) > 0);
        CHECK(snprintf(text, sizeof(text), "UmiApplicationExperienceDefinition DEFINITION = {\"org.umicom.%s\",\"Fixture\",\"default\"};\n"
            "Layout L[] = {{sizeof(UmiExperienceLayoutDefinition),\"default\"}};\n"
            "Panel P[] = {{sizeof(UmiExperiencePanelDefinition)}};\n", names[i]) > 0);
        CHECK(fixture(base, path, text) == 0);
        CHECK(snprintf(path, sizeof(path), "framework/src/application/component/recipes/%s/standard.inc", names[i]) > 0);
        CHECK(snprintf(text, sizeof(text), "UmiApplicationComponentRecipe recipe%zu = {\"org.umicom.%s\",UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD};\n", i, names[i]) > 0);
        CHECK(fixture(base, path, text) == 0);
        CHECK(snprintf(path, sizeof(path), "applications/%s/application.umicom.yaml", names[i]) > 0);
        CHECK(snprintf(text, sizeof(text), "schema: umicom.application.v1\napplication:\n  id: org.umicom.%s\n  name: Fixture\n  executable: umicom-fixture\n", names[i]) > 0);
        CHECK(fixture(base, path, text) == 0);
        CHECK(snprintf(path, sizeof(path), "applications/%s/CMakeLists.txt", names[i]) > 0);
        CHECK(snprintf(text, sizeof(text), "add_library(umicom_product%zu_module STATIC module.c)\n", i) > 0);
        CHECK(fixture(base, path, text) == 0);
    }
    strcat(catalogue, "};\n");
    CHECK(fixture(base, "framework/src/application/experience_catalogue.c", catalogue) == 0);
    CHECK(fixture(base, "framework/src/application/component/recipes/recipes.inc", includes) == 0);
    CHECK(fixture(base, "framework/src/application/component/recipes/recipe_records.inc", records) == 0);
    CHECK(fixture(base, "CMakeLists.txt", modules) == 0);
    CHECK(fixture(base, "cmake/UmicomExtendedApplicationModules.cmake", "# No extra fixture modules\n") == 0);
    const char *group = "all_24_products_have_catalogued_experiences_and_standard_recipes";
    UmiSourceContractReport report = {0};
    CHECK(UmiSourceContractWorkbenchGroup(base, group, &report) == 0);
    CHECK(report.files_checked == 95U && report.findings == 0U);
    CHECK(fixture(base, "framework/src/application/component/recipes/studio/standard.inc",
        "UmiApplicationComponentRecipe recipe1 = {\"org.umicom.studio\",WRONG_AUDIENCE};") == 0);
    report = (UmiSourceContractReport){0};
    CHECK(UmiSourceContractWorkbenchGroup(base, group, &report) == 1);
    CHECK(report.findings == 1U);
    report = (UmiSourceContractReport){0};
    CHECK(UmiSourceContractWorkbenchGroup(base, "misspelled-check", &report) == 2);
    return 0;
}

/* The same rule evaluator used by CTest inspects these tiny independent
 * source-list and sizing fixtures. Missing wiring must produce a finding. */
static int rules(const char *base)
{
    CHECK(fixture(base, "framework/include/umicom/ui/gtk4/workstation/window_fit.h",
        "#include \"umicom/base/status.h\"\nUmiStatus umi_gtk4_ws_window_fit(GtkWindow *w);\n") == 0);
    const char *valid = "# discarded: target_sources(fake)\n"
        "target_sources(umicom_ui_gtk4 PRIVATE \"${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/workstation/window_fit_gtk4.c\")\n"
        "set_source_files_properties(\"adapters/gtk4/workstation/window_fit_gtk4.c\" PROPERTIES LANGUAGE C)\n";
    CHECK(fixture(base, "framework/cmake/UmicomGtk4WorkstationPlatform.cmake", valid) == 0);
    UmiSourceContractReport report = {0};
    const char *group = "shared_sizing_header_and_both_renderer_source_lists";
    CHECK(UmiSourceContractWorkbenchGroup(base, group, &report) == 0);
    CHECK(report.checks == 4U && report.findings == 0U);
    CHECK(fixture(base, "framework/cmake/UmicomGtk4WorkstationPlatform.cmake",
        "# target_sources(umicom_ui_gtk4 PRIVATE adapters/gtk4/workstation/window_fit_gtk4.c)\n"
        "target_sources(umicom_ui_gtk4 PRIVATE wrong.c)\n"
        "set_source_files_properties(\"adapters/gtk4/workstation/window_fit_gtk4.c\" PROPERTIES LANGUAGE C)\n") == 0);
    report = (UmiSourceContractReport){0};
    CHECK(UmiSourceContractWorkbenchGroup(base, group, &report) == 1);
    CHECK(report.findings == 1U);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 2 || argc > 3) return 2;
    int result = 2;
    if (strcmp(argv[1], "metadata") == 0) result = metadata();
    else if (strcmp(argv[1], "lexer") == 0) result = lexer();
    else if (strcmp(argv[1], "identity") == 0) result = identity();
    else if (strcmp(argv[1], "presets") == 0) result = presets();
    else if (strcmp(argv[1], "filesystem") == 0 && argc == 3) result = filesystem(argv[2]);
    else if (strcmp(argv[1], "portfolio") == 0 && argc == 3) result = portfolio(argv[2]);
    else if (strcmp(argv[1], "rules") == 0 && argc == 3) result = rules(argv[2]);
    (void)printf("%s: %zu assertions; %s\n", argv[1], assertions, result == 0 ? "PASS" : "FAIL");
    return result;
}
