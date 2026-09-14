/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/source_contracts/presets.c
 *
 * PURPOSE:
 *   Resolve preset inheritance using the existing Framework JSON reader, with explicit failure boundaries.
 *
 * Author: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/language_runtime/json.h"
#include <stdlib.h>
#include <string.h>

#define SC_MAX_PRESETS 256U
#define SC_PRESET_NAME 256U

typedef struct ScPreset {
    char name[SC_PRESET_NAME];
    int object;
    unsigned char visited;
} ScPreset;
typedef struct ScPresetPlan {
    UmiLanguageRuntimeJsonDocument document;
    ScPreset entries[SC_MAX_PRESETS];
    size_t count;
} ScPresetPlan;

static int get(const ScPresetPlan *plan, int object, const char *key)
{ return umi_language_runtime_json_object_get(&plan->document, object, key); }
static bool typeIs(const ScPresetPlan *plan, int token, UmiLanguageRuntimeJsonTokenType type)
{ return token >= 0 && (size_t)token < plan->document.token_count && plan->document.tokens[token].type == type; }
/* Embedded NUL values cannot be represented by this bounded C-string API.
 * Inspect real escape sequences rather than rejecting a literal backslash-u.
 * The general Framework JSON reader remains unchanged for its other clients. */
static int stringAt(const ScPresetPlan *plan, int token, char *out, size_t capacity)
{
    if (!typeIs(plan, token, UMI_LANGUAGE_RUNTIME_JSON_STRING)) return 2;
    const UmiLanguageRuntimeJsonToken *t = &plan->document.tokens[token];
    for (int p = t->start; p < t->end; ++p) {
        if (plan->document.json[p] != '\\') continue;
        if (++p >= t->end) return 2;
        if (plan->document.json[p] == 'u' && t->end - p >= 5 &&
            memcmp(plan->document.json + p, "u0000", 5U) == 0) return 2;
    }
    return umi_language_runtime_json_string(&plan->document, token, out, capacity) == UMI_STATUS_OK ? 0 : 2;
}
static int findPreset(const ScPresetPlan *plan, const char *name)
{
    for (size_t i = 0U; i < plan->count; ++i) if (strcmp(plan->entries[i].name, name) == 0) return (int)i;
    return -1;
}

/* The existing JSON reader supplies tokens. This guard bounds recursive input
 * before that reader is entered; quoted braces never affect the depth. */
static bool boundedJson(const char *json)
{
    if (json == NULL || strlen(json) >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY) return false;
    size_t depth = 0U;
    bool quoted = false, escaped = false;
    for (const char *p = json; *p != '\0'; ++p) {
        if (quoted) {
            if (escaped) escaped = false;
            else if (*p == '\\') escaped = true;
            else if (*p == '"') quoted = false;
        } else if (*p == '"') quoted = true;
        else if (*p == '{' || *p == '[') { if (++depth > SC_MAX_DEPTH) return false; }
        else if (*p == '}' || *p == ']') { if (depth == 0U) return false; --depth; }
    }
    return !quoted && depth == 0U;
}

static int scalarValue(const ScPresetPlan *plan, int token, char *out, size_t capacity)
{
    if (token < 0 || umi_language_runtime_json_is_null(&plan->document, token)) return 1;
    if (typeIs(plan, token, UMI_LANGUAGE_RUNTIME_JSON_OBJECT)) {
        int value = get(plan, token, "value");
        if (value < 0) return 2;
        token = value;
    }
    if (umi_language_runtime_json_is_null(&plan->document, token)) return 1;
    if (typeIs(plan, token, UMI_LANGUAGE_RUNTIME_JSON_STRING)) return stringAt(plan, token, out, capacity);
    int value = 0;
    if (umi_language_runtime_json_bool(&plan->document, token, &value) != UMI_STATUS_OK) return 2;
    const char *literal = value ? "ON" : "OFF";
    if (strlen(literal) >= capacity) return 2;
    memcpy(out, literal, strlen(literal) + 1U); return 0;
}

static int checkGraph(ScPresetPlan *plan, int index)
{
    ScPreset *entry = &plan->entries[index];
    if (entry->visited == 1U) return 2;
    if (entry->visited == 2U) return 0;
    entry->visited = 1U;
    int inherits = get(plan, entry->object, "inherits");
    if (inherits >= 0) {
        size_t count;
        bool string = typeIs(plan, inherits, UMI_LANGUAGE_RUNTIME_JSON_STRING);
        if (string) count = 1U;
        else if (typeIs(plan, inherits, UMI_LANGUAGE_RUNTIME_JSON_ARRAY)) count = umi_language_runtime_json_array_count(&plan->document, inherits);
        else return 2;
        for (size_t i = 0U; i < count; ++i) {
            char name[SC_PRESET_NAME];
            int parentToken = string ? inherits : umi_language_runtime_json_array_at(&plan->document, inherits, i);
            if (stringAt(plan, parentToken, name, sizeof(name)) != 0) return 2;
            int parent = findPreset(plan, name);
            if (parent < 0 || checkGraph(plan, parent) != 0) return 2;
        }
    }
    entry->visited = 2U;
    return 0;
}
static int planRead(ScPresetPlan *plan, const char *json)
{
    if (!boundedJson(json)) return 2;
    if (umi_language_runtime_json_parse(json, &plan->document) != UMI_STATUS_OK) return 2;
    int presets;
    if (typeIs(plan, 0, UMI_LANGUAGE_RUNTIME_JSON_ARRAY)) presets = 0;
    else if (typeIs(plan, 0, UMI_LANGUAGE_RUNTIME_JSON_OBJECT)) {
        int include = get(plan, 0, "include");
        if (include >= 0 && (!typeIs(plan, include, UMI_LANGUAGE_RUNTIME_JSON_ARRAY) ||
            umi_language_runtime_json_array_count(&plan->document, include) != 0U)) return 2;
        presets = get(plan, 0, "configurePresets");
    } else return 2;
    if (!typeIs(plan, presets, UMI_LANGUAGE_RUNTIME_JSON_ARRAY)) return 2;
    plan->count = umi_language_runtime_json_array_count(&plan->document, presets);
    if (plan->count == 0U || plan->count > SC_MAX_PRESETS) return 2;
    /* Reject duplicate JSON keys, rather than silently inheriting the first
     * spelling accepted by a particular parser. */
    for (size_t t = 0U; t < plan->document.token_count; ++t) {
        if (!typeIs(plan, (int)t, UMI_LANGUAGE_RUNTIME_JSON_OBJECT)) continue;
        size_t n = umi_language_runtime_json_object_count(&plan->document, (int)t);
        for (size_t i = 0U; i < n; ++i) {
            int key, value;
            char first[SC_MAX_TOKEN_BYTES];
            if (umi_language_runtime_json_object_entry_at(&plan->document, (int)t, i, &key, &value) != UMI_STATUS_OK ||
                stringAt(plan, key, first, sizeof(first)) != 0) return 2;
            for (size_t j = 0U; j < i; ++j) {
                char second[SC_MAX_TOKEN_BYTES];
                if (umi_language_runtime_json_object_entry_at(&plan->document, (int)t, j, &key, &value) != UMI_STATUS_OK ||
                    stringAt(plan, key, second, sizeof(second)) != 0 || strcmp(first, second) == 0) return 2;
            }
        }
    }
    for (size_t i = 0U; i < plan->count; ++i) {
        int object = umi_language_runtime_json_array_at(&plan->document, presets, i);
        if (!typeIs(plan, object, UMI_LANGUAGE_RUNTIME_JSON_OBJECT)) return 2;
        plan->entries[i].object = object;
        if (stringAt(plan, get(plan, object, "name"), plan->entries[i].name, SC_PRESET_NAME) != 0 || plan->entries[i].name[0] == '\0') return 2;
        for (size_t j = 0U; j < i; ++j) if (strcmp(plan->entries[i].name, plan->entries[j].name) == 0) return 2;
        int cache = get(plan, object, "cacheVariables");
        if (cache >= 0) {
            if (!typeIs(plan, cache, UMI_LANGUAGE_RUNTIME_JSON_OBJECT)) return 2;
            size_t n = umi_language_runtime_json_object_count(&plan->document, cache);
            for (size_t j = 0U; j < n; ++j) {
                int key, value; char decoded[SC_MAX_TOKEN_BYTES];
                if (umi_language_runtime_json_object_entry_at(&plan->document, cache, j, &key, &value) != UMI_STATUS_OK ||
                    scalarValue(plan, value, decoded, sizeof(decoded)) == 2) return 2;
            }
        }
    }
    for (size_t i = 0U; i < plan->count; ++i) if (checkGraph(plan, (int)i) != 0) return 2;
    return 0;
}

/* A direct null entry is present: it shadows later parents while representing
 * an explicit unset. Lookup order is child, first parent, later parents. */
static int valueTokenCached(const ScPresetPlan *plan, int index, const char *key, int *memo)
{
    if (memo[index] != -3) return memo[index];
    int object = plan->entries[index].object;
    int cache = get(plan, object, "cacheVariables");
    int own = cache >= 0 ? get(plan, cache, key) : -1;
    if (own >= 0) return memo[index] = own;
    int inherits = get(plan, object, "inherits");
    if (inherits < 0) return memo[index] = -1;
    bool string = typeIs(plan, inherits, UMI_LANGUAGE_RUNTIME_JSON_STRING);
    size_t n = string ? 1U : umi_language_runtime_json_array_count(&plan->document, inherits);
    for (size_t i = 0U; i < n; ++i) {
        char name[SC_PRESET_NAME];
        int parentToken = string ? inherits : umi_language_runtime_json_array_at(&plan->document, inherits, i);
        if (stringAt(plan, parentToken, name, sizeof(name)) != 0) return -2;
        int parent = findPreset(plan, name);
        if (parent < 0) return -2;
        int result = valueTokenCached(plan, parent, key, memo);
        if (result != -1) return memo[index] = result;
    }
    return memo[index] = -1;
}
/* Memoise per-key results: a diamond-shaped inheritance DAG must not cause
 * exponentially repeated traversal when the requested field is absent. */
static int valueToken(const ScPresetPlan *plan, int index, const char *key)
{
    int memo[SC_MAX_PRESETS];
    for (size_t i = 0U; i < SC_MAX_PRESETS; ++i) memo[i] = -3;
    return valueTokenCached(plan, index, key, memo);
}
int UmiSourceContractPresetValue(const char *json, const char *preset,
    const char *key, char *out, size_t capacity)
{
    if (json == NULL || preset == NULL || key == NULL || out == NULL || capacity == 0U) return 2;
    ScPresetPlan *plan = calloc(1U, sizeof(*plan));
    if (plan == NULL) return 2;
    int result = planRead(plan, json), index = -1;
    if (result == 0) { index = findPreset(plan, preset); if (index < 0) result = 2; }
    if (result == 0) {
        int token = valueToken(plan, index, key);
        result = token == -2 ? 2 : scalarValue(plan, token, out, capacity);
    }
    free(plan); return result;
}
int UmiSourceContractAuditPresets(const char *json, const char *path,
    UmiSourceContractReport *report)
{
    if (report == NULL || json == NULL || path == NULL) return 2;
    ScPresetPlan *plan = calloc(1U, sizeof(*plan));
    if (plan == NULL) return 2;
    size_t before = report->findings;
    if (planRead(plan, json) != 0) {
        ScFinding(report, path, 1U, "presets.graph", "Invalid in-file preset graph, unsupported include or malformed cache value", true);
        free(plan); return 2;
    }
    static const char *const switches[] = {
        "UMICOM_DESKTOP_BUILD_GTK", "UMICOM_STUDIO_BUILD_GTK",
        "UMICOM_TRADER_BUILD_GTK4", "UMICOM_BANK_BUILD_GTK4",
        "UMICOM_TMS_BUILD_GTK4", "UMICOM_MUSIC_STUDIO_BUILD_GTK4",
        "UMICOM_APPLICATIONS_BUILD_SHARED_GTK4"
    };
    size_t headless = 0U;
    for (size_t i = 0U; i < plan->count; ++i) {
        if (strstr(plan->entries[i].name, "headless") == NULL) continue;
        ++headless;
        for (size_t j = 0U; j < sizeof(switches) / sizeof(switches[0]); ++j) {
            char value[SC_MAX_TOKEN_BYTES];
            int token = valueToken(plan, (int)i, switches[j]);
            int result = token == -2 ? 2 : scalarValue(plan, token, value, sizeof(value));
            ++report->checks;
            if (result != 0 || strcmp(value, "OFF") != 0) {
                char message[1024];
                (void)snprintf(message, sizeof(message), "%s: %s must resolve to OFF", plan->entries[i].name, switches[j]);
                ScFinding(report, path, 1U, "headless_presets_explicitly_disable_all_gui_switches", message, false);
            }
        }
    }
    if (headless == 0U) ScFinding(report, path, 1U, "presets.headless", "No headless configuration found", false);
    free(plan); return before == report->findings ? 0 : 1;
}
