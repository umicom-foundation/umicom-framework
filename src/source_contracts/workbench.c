/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/source_contracts/workbench.c
 *
 * PURPOSE:
 *   Evaluate Framework workbench source contracts without a scripting interpreter.
 *
 * Author: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef enum ScRuleKind { SC_REQUIRE, SC_FORBID, SC_BEFORE, SC_COUNT, SC_FILE } ScRuleKind;
typedef struct ScRule { const char *name, *path, *scope, *first, *second; ScRuleKind kind; size_t count; } ScRule;
static const ScRule rules[] = {
#include "workbench_rules.inc"
};

/* CMake target commands use balanced parentheses, so nested generator
 * expressions cannot truncate a source list at its first internal parenthesis. */
static int scopeText(const ScText *source, const char *scope, ScText *out)
{
    if (scope[0] != '@') return ScBody(source, scope, out);
    size_t start = ScIndex(source, scope + 1, 0U);
    if (start == SIZE_MAX) return 1;
    size_t open = start;
    while (open < source->count && !ScTokenEquals(source, open, "(")) ++open;
    size_t end = ScMatching(source, open);
    if (end == SIZE_MAX) return 2;
    size_t begin = source->tokens[open].end;
    char *copy = ScCopy(source->data + begin, source->tokens[end].begin - begin);
    if (copy == NULL) return 2;
    int result = ScLex(out, copy, true);
    if (result == 0) out->line_offset = ScLine(source, open) - 1U;
    free(copy); return result;
}
/* Source lists may spell paths directly or through ${CMAKE_CURRENT_LIST_DIR}.
 * Match only an entire quoted token's suffix with a path boundary, never a
 * substring in arbitrary source text or a CMake comment. */
static size_t fileReference(const ScText *text, const char *relative)
{
    size_t n = strlen(relative);
    for (size_t i = 0U; i < text->count; ++i) {
        const ScToken *t = &text->tokens[i];
        if (t->kind != '"' || t->end - t->begin < n + 2U) continue;
        size_t start = t->end - 1U - n;
        if (memcmp(text->data + start, relative, n) == 0 &&
            (start == t->begin + 1U || text->data[start - 1U] == '/')) return i;
    }
    return ScIndex(text, relative, 0U);
}
static bool cmakeFile(const char *path)
{
    const char *ext = strrchr(path, '.');
    return (ext != NULL && strcmp(ext, ".cmake") == 0) || strstr(path, "CMakeLists.txt") != NULL;
}
static int workbenchRules(const char *root, const char *group, UmiSourceContractReport *report)
{
    size_t before = report->findings;
    /* Keep only the current source cached; rule order groups adjacent owners.
     * No global cache survives into another scan of a changed checkout. */
    const char *last = NULL;
    ScText source = {0};
    bool available = false;
    for (size_t i = 0U; i < sizeof(rules) / sizeof(rules[0]); ++i) {
        const ScRule *rule = &rules[i];
        if (group != NULL && strcmp(group, rule->name) != 0) continue;
        if (last == NULL || strcmp(last, rule->path) != 0) {
            ScTextFree(&source); last = rule->path;
            char *path = ScJoin(root, last);
            char *content = path != NULL ? ScRead(path) : NULL;
            available = content != NULL && ScLex(&source, content, cmakeFile(last)) == 0;
            if (!available) ScFinding(report, last, 1U, "workbench.input", "Cannot read/tokenise the required source", true);
            else ++report->files_checked;
            free(content); free(path);
        }
        if (!available) continue;
        ScText scoped = {0};
        const ScText *text = &source;
        if (rule->scope[0] != '\0') {
            if (scopeText(&source, rule->scope, &scoped) != 0) {
                ScFinding(report, rule->path, 1U, rule->name, rule->scope, false);
                continue;
            }
            text = &scoped;
        }
        ++report->checks;
        size_t first = rule->kind == SC_FILE ? fileReference(text, rule->first) : ScIndex(text, rule->first, 0U);
        bool passed = false;
        switch (rule->kind) {
        case SC_FILE:
        case SC_REQUIRE: passed = first != SIZE_MAX; break;
        case SC_FORBID: passed = first == SIZE_MAX; break;
        case SC_COUNT: passed = ScCount(text, rule->first) == rule->count; break;
        case SC_BEFORE: {
            size_t second = ScIndex(text, rule->second, 0U);
            passed = first != SIZE_MAX && second != SIZE_MAX && first < second;
            break;
        }
        }
        if (!passed) {
            char message[2048];
            int n = snprintf(message, sizeof(message), "%s: %s%s%s", rule->scope,
                rule->first, rule->kind == SC_BEFORE ? " must precede " : "",
                rule->kind == SC_BEFORE ? rule->second : "");
            ScFinding(report, rule->path, ScLine(text, first), rule->name,
                n >= 0 && (size_t)n < sizeof(message) ? message : "Contract fragment mismatch", false);
        }
        ScTextFree(&scoped);
    }
    ScTextFree(&source);
    return report->findings != before ? 1 : 0;
}
int ScWorkbenchRules(const char *root, UmiSourceContractReport *report)
{ return workbenchRules(root, NULL, report); }

int UmiSourceContractWorkbenchGroup(const char *source_root, const char *group,
    UmiSourceContractReport *report)
{
    if (source_root == NULL || report == NULL) return 2;
    size_t before = report->findings, inputBefore = report->input_errors;
    bool known = group == NULL;
    if (group != NULL) {
        for (size_t i = 0U; i < sizeof(rules) / sizeof(rules[0]); ++i)
            if (strcmp(group, rules[i].name) == 0) known = true;
        if (strcmp(group, "every_native_main_window_requests_shared_icon_identity") == 0 ||
            strcmp(group, "headless_presets_explicitly_disable_all_gui_switches") == 0 ||
            strcmp(group, "all_24_products_have_catalogued_experiences_and_standard_recipes") == 0) known = true;
    }
    if (!known) {
        ScFinding(report, source_root, 1U, "workbench.group", "Unknown source contract group", true);
        return 2;
    }
    (void)workbenchRules(source_root, group, report);
    static const char *const identityPaths[] = {
        "applications/bank/src/gtk/main.c", "applications/tms/src/gtk/main.c",
        "applications/music/src/gtk/main.c", "applications/trader/src/gtk/main.c",
        "framework/adapters/gtk4/desk_gtk4.c",
        "framework/adapters/gtk4/application_product_application_gtk4.c",
        "applications/studio/src/gui/workbench/runtime/runtime_shell.inc"
    };
    if (group == NULL || strcmp(group, "every_native_main_window_requests_shared_icon_identity") == 0)
    for (size_t i = 0U; i < sizeof(identityPaths) / sizeof(identityPaths[0]); ++i) {
        char *path = ScJoin(source_root, identityPaths[i]);
        char *text = path != NULL ? ScRead(path) : NULL;
        if (text == NULL) ScFinding(report, identityPaths[i], 1U, "identity.input", "Cannot read native composition source", true);
        else {
            ++report->files_checked;
            if (i != 6U) {
                ScText identitySource = {0};
                if (ScLex(&identitySource, text, false) != 0) {
                    ScFinding(report, identityPaths[i], 1U, "identity.input", "Cannot tokenise native source", true);
                } else {
                    ++report->checks;
                    if (!ScContains(&identitySource, "#include \"umicom/ui/gtk4/workstation/shell_header.h\""))
                        ScFinding(report, identityPaths[i], 1U, "identity.header", "Missing shared identity declaration include", false);
                    ScTextFree(&identitySource);
                }
            }
            (void)UmiSourceContractWindowIdentity(text, identityPaths[i], i == 6U, report);
        }
        free(text); free(path);
    }
    if (group == NULL || strcmp(group, "headless_presets_explicitly_disable_all_gui_switches") == 0) {
    char *path = ScJoin(source_root, "CMakePresets.json");
    char *presets = path != NULL ? ScRead(path) : NULL;
    if (presets == NULL) ScFinding(report, "CMakePresets.json", 1U, "presets.input", "Cannot read configure presets", true);
    else { ++report->files_checked; (void)UmiSourceContractAuditPresets(presets, "CMakePresets.json", report); }
    free(presets); free(path);
    }
    if (group == NULL || strcmp(group, "all_24_products_have_catalogued_experiences_and_standard_recipes") == 0)
        (void)ScPortfolio(source_root, report);
    return report->input_errors != inputBefore ? 2 : report->findings != before ? 1 : 0;
}

int UmiSourceContractWorkbench(const char *source_root, UmiSourceContractReport *report)
{ return UmiSourceContractWorkbenchGroup(source_root, NULL, report); }
