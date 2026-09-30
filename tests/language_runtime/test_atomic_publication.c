/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_atomic_publication.c
 * PURPOSE: Verify provider conversion and editor publication preserve good data.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language_runtime/service_bridge.h"
#include "umicom/language_runtime/editor_bridge.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); return 1; \
} } while (0)

/* Decoder arrays are heap fixtures so the tests also fit Windows thread stacks.
 * Every case uses the actual service and bridge APIs, with checks under NDEBUG. */
static int Case_completion(UmiLanguageRuntimeServiceBridge *b)
{
    UmiLanguageRuntimeCompletionResult *r = calloc(1U, sizeof(*r));
    CHECK(r != NULL);
    UmiLanguageCompletionRegistry *registry = umi_language_service_completion(b->language);
    UmiLanguageCompletionSnapshot before, after, foreign;
    const char *doc = "a";
    r->count = 1U;
    memcpy(r->items[0].label, "old", sizeof("old"));
    CHECK(umi_language_runtime_publish_completion(b, doc, 3U, 4U, r) == UMI_STATUS_OK);
    CHECK(umi_language_completion_registry_at(registry, 0U, &before) == UMI_STATUS_OK);
    doc = "b";
    CHECK(umi_language_runtime_publish_completion(b, doc, 3U, 4U, r) == UMI_STATUS_OK);
    CHECK(umi_language_completion_registry_at(registry, 1U, &foreign) == UMI_STATUS_OK);
    uint64_t revision = umi_language_completion_registry_revision(registry), bridge_revision = b->revision;
    doc = "a";
    r->count = 2U;
    memcpy(r->items[0].label, "new", sizeof("new"));
    memset(r->items[1].label, 'x', sizeof(r->items[1].label));
    CHECK(umi_language_runtime_publish_completion(b, doc, 3U, 4U, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_completion_registry_count(registry) == 2U);
    CHECK(umi_language_completion_registry_revision(registry) == revision && b->revision == bridge_revision);
    CHECK(umi_language_completion_registry_find(registry, before.id, &after) == UMI_STATUS_OK);
    CHECK(after.revision == before.revision && strcmp(after.label, before.label) == 0);
    r->count = sizeof(r->items) / sizeof(r->items[0]) + 1U;
    CHECK(umi_language_runtime_publish_completion(b, doc, 3U, 4U, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_completion_registry_revision(registry) == revision && b->revision == bridge_revision);
    r->count = 1U;
    char too_long[128]; memset(too_long, 'x', sizeof(too_long)); too_long[127] = '\0';
    doc = too_long;
    CHECK(umi_language_runtime_publish_completion(b, doc, 3U, 4U, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_completion_registry_revision(registry) == revision);
    doc = "a";
    CHECK(umi_language_runtime_publish_completion(b, doc, 3U, 4U, r) == UMI_STATUS_OK);
    CHECK(umi_language_completion_registry_count(registry) == 2U);
    CHECK(umi_language_completion_registry_find(registry, foreign.id, &after) == UMI_STATUS_OK && after.revision == foreign.revision);
    r->count = 0U;
    CHECK(umi_language_runtime_publish_completion(b, doc, 3U, 4U, r) == UMI_STATUS_OK);
    CHECK(umi_language_completion_registry_count(registry) == 1U);
    CHECK(umi_language_completion_registry_find(registry, foreign.id, &after) == UMI_STATUS_OK && after.revision == foreign.revision);
    revision = umi_language_completion_registry_revision(registry);
    b->revision = UINT64_MAX;
    CHECK(umi_language_runtime_publish_completion(b, doc, 3U, 4U, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_completion_registry_revision(registry) == revision && b->revision == UINT64_MAX);
    free(r);
    return 0;
}

static int Case_reference(UmiLanguageRuntimeServiceBridge *b)
{
    UmiLanguageRuntimeLocationList *r = calloc(1U, sizeof(*r));
    CHECK(r != NULL);
    UmiLanguageReferenceRegistry *registry = umi_language_service_reference(b->language);
    UmiLanguageReferenceSnapshot before, after, foreign;
    const char *doc = "a";
    r->count = 1U;
    memcpy(r->items[0].uri, "old", sizeof("old"));
    CHECK(umi_language_runtime_publish_locations(b, doc, "symbol", 0, r) == UMI_STATUS_OK);
    CHECK(umi_language_reference_registry_at(registry, 0U, &before) == UMI_STATUS_OK);
    doc = "b";
    CHECK(umi_language_runtime_publish_locations(b, doc, "symbol", 0, r) == UMI_STATUS_OK);
    CHECK(umi_language_reference_registry_at(registry, 1U, &foreign) == UMI_STATUS_OK);
    uint64_t revision = umi_language_reference_registry_revision(registry), bridge_revision = b->revision;
    doc = "a";
    r->count = 2U;
    memcpy(r->items[0].uri, "new", sizeof("new"));
    memset(r->items[1].uri, 'x', sizeof(r->items[1].uri));
    CHECK(umi_language_runtime_publish_locations(b, doc, "symbol", 0, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_reference_registry_count(registry) == 2U);
    CHECK(umi_language_reference_registry_revision(registry) == revision && b->revision == bridge_revision);
    CHECK(umi_language_reference_registry_find(registry, before.id, &after) == UMI_STATUS_OK);
    CHECK(after.revision == before.revision && strcmp(after.uri, before.uri) == 0);
    r->count = sizeof(r->items) / sizeof(r->items[0]) + 1U;
    CHECK(umi_language_runtime_publish_locations(b, doc, "symbol", 0, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_reference_registry_revision(registry) == revision && b->revision == bridge_revision);
    r->count = 1U;
    char too_long[128]; memset(too_long, 'x', sizeof(too_long)); too_long[127] = '\0';
    doc = too_long;
    CHECK(umi_language_runtime_publish_locations(b, doc, "symbol", 0, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_reference_registry_revision(registry) == revision);
    doc = "a";
    CHECK(umi_language_runtime_publish_locations(b, doc, "symbol", 0, r) == UMI_STATUS_OK);
    CHECK(umi_language_reference_registry_count(registry) == 2U);
    CHECK(umi_language_reference_registry_find(registry, foreign.id, &after) == UMI_STATUS_OK && after.revision == foreign.revision);
    r->count = 0U;
    CHECK(umi_language_runtime_publish_locations(b, doc, "symbol", 0, r) == UMI_STATUS_OK);
    CHECK(umi_language_reference_registry_count(registry) == 1U);
    CHECK(umi_language_reference_registry_find(registry, foreign.id, &after) == UMI_STATUS_OK && after.revision == foreign.revision);
    revision = umi_language_reference_registry_revision(registry);
    b->revision = UINT64_MAX;
    CHECK(umi_language_runtime_publish_locations(b, doc, "symbol", 0, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_reference_registry_revision(registry) == revision && b->revision == UINT64_MAX);
    free(r);
    return 0;
}

static int Case_symbol(UmiLanguageRuntimeServiceBridge *b)
{
    UmiLanguageRuntimeSymbolList *r = calloc(1U, sizeof(*r));
    CHECK(r != NULL);
    UmiLanguageSymbolRegistry *registry = umi_language_service_symbol(b->language);
    UmiLanguageSymbolSnapshot before, after, foreign;
    const char *doc = "a";
    r->count = 1U;
    memcpy(r->items[0].name, "old", sizeof("old"));
    CHECK(umi_language_runtime_publish_symbols(b, doc, r) == UMI_STATUS_OK);
    CHECK(umi_language_symbol_registry_at(registry, 0U, &before) == UMI_STATUS_OK);
    doc = "b";
    CHECK(umi_language_runtime_publish_symbols(b, doc, r) == UMI_STATUS_OK);
    CHECK(umi_language_symbol_registry_at(registry, 1U, &foreign) == UMI_STATUS_OK);
    uint64_t revision = umi_language_symbol_registry_revision(registry), bridge_revision = b->revision;
    doc = "a";
    r->count = 2U;
    memcpy(r->items[0].name, "new", sizeof("new"));
    memset(r->items[1].name, 'x', sizeof(r->items[1].name));
    CHECK(umi_language_runtime_publish_symbols(b, doc, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_symbol_registry_count(registry) == 2U);
    CHECK(umi_language_symbol_registry_revision(registry) == revision && b->revision == bridge_revision);
    CHECK(umi_language_symbol_registry_find(registry, before.id, &after) == UMI_STATUS_OK);
    CHECK(after.revision == before.revision && strcmp(after.name, before.name) == 0);
    r->count = sizeof(r->items) / sizeof(r->items[0]) + 1U;
    CHECK(umi_language_runtime_publish_symbols(b, doc, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_symbol_registry_revision(registry) == revision && b->revision == bridge_revision);
    r->count = 1U;
    char too_long[128]; memset(too_long, 'x', sizeof(too_long)); too_long[127] = '\0';
    doc = too_long;
    CHECK(umi_language_runtime_publish_symbols(b, doc, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_symbol_registry_revision(registry) == revision);
    doc = "a";
    CHECK(umi_language_runtime_publish_symbols(b, doc, r) == UMI_STATUS_OK);
    CHECK(umi_language_symbol_registry_count(registry) == 2U);
    CHECK(umi_language_symbol_registry_find(registry, foreign.id, &after) == UMI_STATUS_OK && after.revision == foreign.revision);
    r->count = 0U;
    CHECK(umi_language_runtime_publish_symbols(b, doc, r) == UMI_STATUS_OK);
    CHECK(umi_language_symbol_registry_count(registry) == 1U);
    CHECK(umi_language_symbol_registry_find(registry, foreign.id, &after) == UMI_STATUS_OK && after.revision == foreign.revision);
    revision = umi_language_symbol_registry_revision(registry);
    b->revision = UINT64_MAX;
    CHECK(umi_language_runtime_publish_symbols(b, doc, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_symbol_registry_revision(registry) == revision && b->revision == UINT64_MAX);
    free(r);
    return 0;
}

static int Case_diagnostic(UmiLanguageRuntimeServiceBridge *b)
{
    UmiLanguageRuntimeDiagnosticList *r = calloc(1U, sizeof(*r));
    CHECK(r != NULL);
    UmiLanguageDiagnosticRegistry *registry = umi_language_service_diagnostic(b->language);
    UmiLanguageDiagnosticSnapshot before, after, foreign;
    const char *doc = "a";
    r->count = 1U;
    memcpy(r->items[0].message, "old", sizeof("old"));
    CHECK(umi_language_runtime_publish_diagnostics(b, doc, r) == UMI_STATUS_OK);
    CHECK(umi_language_diagnostic_registry_at(registry, 0U, &before) == UMI_STATUS_OK);
    doc = "b";
    CHECK(umi_language_runtime_publish_diagnostics(b, doc, r) == UMI_STATUS_OK);
    CHECK(umi_language_diagnostic_registry_at(registry, 1U, &foreign) == UMI_STATUS_OK);
    uint64_t revision = umi_language_diagnostic_registry_revision(registry), bridge_revision = b->revision;
    doc = "a";
    r->count = 2U;
    memcpy(r->items[0].message, "new", sizeof("new"));
    memset(r->items[1].message, 'x', sizeof(r->items[1].message));
    CHECK(umi_language_runtime_publish_diagnostics(b, doc, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_diagnostic_registry_count(registry) == 2U);
    CHECK(umi_language_diagnostic_registry_revision(registry) == revision && b->revision == bridge_revision);
    CHECK(umi_language_diagnostic_registry_find(registry, before.id, &after) == UMI_STATUS_OK);
    CHECK(after.revision == before.revision && strcmp(after.message, before.message) == 0);
    r->count = sizeof(r->items) / sizeof(r->items[0]) + 1U;
    CHECK(umi_language_runtime_publish_diagnostics(b, doc, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_diagnostic_registry_revision(registry) == revision && b->revision == bridge_revision);
    r->count = 1U;
    char too_long[128]; memset(too_long, 'x', sizeof(too_long)); too_long[127] = '\0';
    doc = too_long;
    CHECK(umi_language_runtime_publish_diagnostics(b, doc, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_diagnostic_registry_revision(registry) == revision);
    doc = "a";
    CHECK(umi_language_runtime_publish_diagnostics(b, doc, r) == UMI_STATUS_OK);
    CHECK(umi_language_diagnostic_registry_count(registry) == 2U);
    CHECK(umi_language_diagnostic_registry_find(registry, foreign.id, &after) == UMI_STATUS_OK && after.revision == foreign.revision);
    r->count = 0U;
    CHECK(umi_language_runtime_publish_diagnostics(b, doc, r) == UMI_STATUS_OK);
    CHECK(umi_language_diagnostic_registry_count(registry) == 1U);
    CHECK(umi_language_diagnostic_registry_find(registry, foreign.id, &after) == UMI_STATUS_OK && after.revision == foreign.revision);
    revision = umi_language_diagnostic_registry_revision(registry);
    b->revision = UINT64_MAX;
    CHECK(umi_language_runtime_publish_diagnostics(b, doc, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_diagnostic_registry_revision(registry) == revision && b->revision == UINT64_MAX);
    free(r);
    return 0;
}

static int Case_code_action(UmiLanguageRuntimeServiceBridge *b)
{
    UmiLanguageRuntimeCodeActionList *r = calloc(1U, sizeof(*r));
    CHECK(r != NULL);
    UmiLanguageCodeActionRegistry *registry = umi_language_service_code_action(b->language);
    UmiLanguageCodeActionSnapshot before, after, foreign;
    const char *doc = "a";
    r->count = 1U;
    memcpy(r->items[0].title, "old", sizeof("old"));
    CHECK(umi_language_runtime_publish_code_actions(b, doc, r) == UMI_STATUS_OK);
    CHECK(umi_language_code_action_registry_at(registry, 0U, &before) == UMI_STATUS_OK);
    doc = "b";
    CHECK(umi_language_runtime_publish_code_actions(b, doc, r) == UMI_STATUS_OK);
    CHECK(umi_language_code_action_registry_at(registry, 1U, &foreign) == UMI_STATUS_OK);
    uint64_t revision = umi_language_code_action_registry_revision(registry), bridge_revision = b->revision;
    doc = "a";
    r->count = 2U;
    memcpy(r->items[0].title, "new", sizeof("new"));
    memset(r->items[1].title, 'x', sizeof(r->items[1].title));
    CHECK(umi_language_runtime_publish_code_actions(b, doc, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_code_action_registry_count(registry) == 2U);
    CHECK(umi_language_code_action_registry_revision(registry) == revision && b->revision == bridge_revision);
    CHECK(umi_language_code_action_registry_find(registry, before.id, &after) == UMI_STATUS_OK);
    CHECK(after.revision == before.revision && strcmp(after.title, before.title) == 0);
    r->count = sizeof(r->items) / sizeof(r->items[0]) + 1U;
    CHECK(umi_language_runtime_publish_code_actions(b, doc, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_code_action_registry_revision(registry) == revision && b->revision == bridge_revision);
    r->count = 1U;
    char too_long[128]; memset(too_long, 'x', sizeof(too_long)); too_long[127] = '\0';
    doc = too_long;
    CHECK(umi_language_runtime_publish_code_actions(b, doc, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_code_action_registry_revision(registry) == revision);
    doc = "a";
    CHECK(umi_language_runtime_publish_code_actions(b, doc, r) == UMI_STATUS_OK);
    CHECK(umi_language_code_action_registry_count(registry) == 2U);
    CHECK(umi_language_code_action_registry_find(registry, foreign.id, &after) == UMI_STATUS_OK && after.revision == foreign.revision);
    r->count = 0U;
    CHECK(umi_language_runtime_publish_code_actions(b, doc, r) == UMI_STATUS_OK);
    CHECK(umi_language_code_action_registry_count(registry) == 1U);
    CHECK(umi_language_code_action_registry_find(registry, foreign.id, &after) == UMI_STATUS_OK && after.revision == foreign.revision);
    revision = umi_language_code_action_registry_revision(registry);
    b->revision = UINT64_MAX;
    CHECK(umi_language_runtime_publish_code_actions(b, doc, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_code_action_registry_revision(registry) == revision && b->revision == UINT64_MAX);
    free(r);
    return 0;
}

static int Case_inlay_hint(UmiLanguageRuntimeServiceBridge *b)
{
    UmiLanguageRuntimeInlayHintList *r = calloc(1U, sizeof(*r));
    CHECK(r != NULL);
    UmiLanguageInlayHintRegistry *registry = umi_language_service_inlay_hint(b->language);
    UmiLanguageInlayHintSnapshot before, after, foreign;
    const char *doc = "a";
    r->count = 1U;
    memcpy(r->items[0].label, "old", sizeof("old"));
    CHECK(umi_language_runtime_publish_inlay_hints(b, doc, r) == UMI_STATUS_OK);
    CHECK(umi_language_inlay_hint_registry_at(registry, 0U, &before) == UMI_STATUS_OK);
    doc = "b";
    CHECK(umi_language_runtime_publish_inlay_hints(b, doc, r) == UMI_STATUS_OK);
    CHECK(umi_language_inlay_hint_registry_at(registry, 1U, &foreign) == UMI_STATUS_OK);
    uint64_t revision = umi_language_inlay_hint_registry_revision(registry), bridge_revision = b->revision;
    doc = "a";
    r->count = 2U;
    memcpy(r->items[0].label, "new", sizeof("new"));
    memset(r->items[1].label, 'x', sizeof(r->items[1].label));
    CHECK(umi_language_runtime_publish_inlay_hints(b, doc, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_inlay_hint_registry_count(registry) == 2U);
    CHECK(umi_language_inlay_hint_registry_revision(registry) == revision && b->revision == bridge_revision);
    CHECK(umi_language_inlay_hint_registry_find(registry, before.id, &after) == UMI_STATUS_OK);
    CHECK(after.revision == before.revision && strcmp(after.label, before.label) == 0);
    r->count = sizeof(r->items) / sizeof(r->items[0]) + 1U;
    CHECK(umi_language_runtime_publish_inlay_hints(b, doc, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_inlay_hint_registry_revision(registry) == revision && b->revision == bridge_revision);
    r->count = 1U;
    char too_long[128]; memset(too_long, 'x', sizeof(too_long)); too_long[127] = '\0';
    doc = too_long;
    CHECK(umi_language_runtime_publish_inlay_hints(b, doc, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_inlay_hint_registry_revision(registry) == revision);
    doc = "a";
    CHECK(umi_language_runtime_publish_inlay_hints(b, doc, r) == UMI_STATUS_OK);
    CHECK(umi_language_inlay_hint_registry_count(registry) == 2U);
    CHECK(umi_language_inlay_hint_registry_find(registry, foreign.id, &after) == UMI_STATUS_OK && after.revision == foreign.revision);
    r->count = 0U;
    CHECK(umi_language_runtime_publish_inlay_hints(b, doc, r) == UMI_STATUS_OK);
    CHECK(umi_language_inlay_hint_registry_count(registry) == 1U);
    CHECK(umi_language_inlay_hint_registry_find(registry, foreign.id, &after) == UMI_STATUS_OK && after.revision == foreign.revision);
    revision = umi_language_inlay_hint_registry_revision(registry);
    b->revision = UINT64_MAX;
    CHECK(umi_language_runtime_publish_inlay_hints(b, doc, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_inlay_hint_registry_revision(registry) == revision && b->revision == UINT64_MAX);
    free(r);
    return 0;
}

static int Case_folding_range(UmiLanguageRuntimeServiceBridge *b)
{
    UmiLanguageRuntimeFoldingRangeList *r = calloc(1U, sizeof(*r));
    CHECK(r != NULL);
    UmiLanguageFoldingRangeRegistry *registry = umi_language_service_folding_range(b->language);
    UmiLanguageFoldingRangeSnapshot before, after, foreign;
    const char *doc = "a";
    r->count = 1U;
    memcpy(r->items[0].kind, "old", sizeof("old"));
    CHECK(umi_language_runtime_publish_folding_ranges(b, doc, r) == UMI_STATUS_OK);
    CHECK(umi_language_folding_range_registry_at(registry, 0U, &before) == UMI_STATUS_OK);
    doc = "b";
    CHECK(umi_language_runtime_publish_folding_ranges(b, doc, r) == UMI_STATUS_OK);
    CHECK(umi_language_folding_range_registry_at(registry, 1U, &foreign) == UMI_STATUS_OK);
    uint64_t revision = umi_language_folding_range_registry_revision(registry), bridge_revision = b->revision;
    doc = "a";
    r->count = 2U;
    memcpy(r->items[0].kind, "new", sizeof("new"));
    memset(r->items[1].kind, 'x', sizeof(r->items[1].kind));
    CHECK(umi_language_runtime_publish_folding_ranges(b, doc, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_folding_range_registry_count(registry) == 2U);
    CHECK(umi_language_folding_range_registry_revision(registry) == revision && b->revision == bridge_revision);
    CHECK(umi_language_folding_range_registry_find(registry, before.id, &after) == UMI_STATUS_OK);
    CHECK(after.revision == before.revision && strcmp(after.kind, before.kind) == 0);
    r->count = sizeof(r->items) / sizeof(r->items[0]) + 1U;
    CHECK(umi_language_runtime_publish_folding_ranges(b, doc, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_folding_range_registry_revision(registry) == revision && b->revision == bridge_revision);
    r->count = 1U;
    char too_long[128]; memset(too_long, 'x', sizeof(too_long)); too_long[127] = '\0';
    doc = too_long;
    CHECK(umi_language_runtime_publish_folding_ranges(b, doc, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_folding_range_registry_revision(registry) == revision);
    doc = "a";
    CHECK(umi_language_runtime_publish_folding_ranges(b, doc, r) == UMI_STATUS_OK);
    CHECK(umi_language_folding_range_registry_count(registry) == 2U);
    CHECK(umi_language_folding_range_registry_find(registry, foreign.id, &after) == UMI_STATUS_OK && after.revision == foreign.revision);
    r->count = 0U;
    CHECK(umi_language_runtime_publish_folding_ranges(b, doc, r) == UMI_STATUS_OK);
    CHECK(umi_language_folding_range_registry_count(registry) == 1U);
    CHECK(umi_language_folding_range_registry_find(registry, foreign.id, &after) == UMI_STATUS_OK && after.revision == foreign.revision);
    revision = umi_language_folding_range_registry_revision(registry);
    b->revision = UINT64_MAX;
    CHECK(umi_language_runtime_publish_folding_ranges(b, doc, r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_folding_range_registry_revision(registry) == revision && b->revision == UINT64_MAX);
    free(r);
    return 0;
}

static int Case_semantic(UmiLanguageRuntimeServiceBridge *b)
{
    UmiLanguageRuntimeSemanticTokens r = {{0}, 5U};
    UmiLanguageSemanticTokenRegistry *registry = umi_language_service_semantic_token(b->language);
    UmiLanguageSemanticTokenSnapshot before, after;
    r.data[2] = 1U;
    CHECK(umi_language_runtime_publish_semantic_tokens(b, "a", &r) == UMI_STATUS_OK);
    CHECK(umi_language_semantic_token_registry_at(registry, 0U, &before) == UMI_STATUS_OK);
    const uint64_t revision = umi_language_semantic_token_registry_revision(registry), bridge = b->revision;
    r.count = 6U;
    CHECK(umi_language_runtime_publish_semantic_tokens(b, "a", &r) == UMI_STATUS_INVALID_ARGUMENT);
    r.count = 10U; r.data[0] = UINT32_MAX; r.data[5] = 1U; r.data[7] = 1U;
    CHECK(umi_language_runtime_publish_semantic_tokens(b, "a", &r) == UMI_STATUS_CAPACITY_EXCEEDED);
    r.data[0] = 0U; r.data[1] = UINT32_MAX; r.data[2] = 0U; r.data[5] = 0U; r.data[6] = 1U;
    CHECK(umi_language_runtime_publish_semantic_tokens(b, "a", &r) == UMI_STATUS_CAPACITY_EXCEEDED);
    r.count = 5U; r.data[2] = 1U;
    CHECK(umi_language_runtime_publish_semantic_tokens(b, "a", &r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_semantic_token_registry_revision(registry) == revision && b->revision == bridge);
    CHECK(umi_language_semantic_token_registry_at(registry, 0U, &after) == UMI_STATUS_OK);
    CHECK(after.revision == before.revision && after.length == before.length && after.line == before.line);
    r.data[1] = 0U;
    CHECK(umi_language_runtime_publish_semantic_tokens(b, "b", &r) == UMI_STATUS_OK);
    r.count = 0U;
    CHECK(umi_language_runtime_publish_semantic_tokens(b, "a", &r) == UMI_STATUS_OK);
    CHECK(umi_language_semantic_token_registry_count(registry) == 1U);
    return 0;
}

static int Case_hover_signature(UmiLanguageRuntimeServiceBridge *b)
{
    UmiLanguageRuntimeHoverResult hover = {0};
    UmiLanguageRuntimeSignatureResult signature = {0};
    memcpy(hover.contents, "help", sizeof("help"));
    memcpy(signature.label, "function()", sizeof("function()")); signature.available = 1;
    CHECK(umi_language_runtime_publish_hover(b, "a", 0U, 0U, &hover) == UMI_STATUS_OK);
    CHECK(umi_language_runtime_publish_hover(b, "b", 0U, 0U, &hover) == UMI_STATUS_OK);
    CHECK(umi_language_runtime_publish_signature(b, "a", 0U, 0U, &signature) == UMI_STATUS_OK);
    CHECK(umi_language_runtime_publish_signature(b, "b", 0U, 0U, &signature) == UMI_STATUS_OK);
    uint64_t revision = b->revision;
    memset(hover.contents, 'x', sizeof(hover.contents));
    memset(signature.label, 'x', sizeof(signature.label));
    CHECK(umi_language_runtime_publish_hover(b, "a", 0U, 0U, &hover) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_runtime_publish_signature(b, "a", 0U, 0U, &signature) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(b->revision == revision);
    CHECK(umi_language_hover_registry_count(umi_language_service_hover(b->language)) == 2U);
    CHECK(umi_language_signature_registry_count(umi_language_service_signature(b->language)) == 2U);
    hover.contents[0] = '\0'; signature.available = 0;
    CHECK(umi_language_runtime_publish_hover(b, "a", 0U, 0U, &hover) == UMI_STATUS_OK);
    CHECK(umi_language_runtime_publish_signature(b, "a", 0U, 0U, &signature) == UMI_STATUS_OK);
    CHECK(b->revision == revision);
    CHECK(umi_language_hover_registry_count(umi_language_service_hover(b->language)) == 1U);
    CHECK(umi_language_signature_registry_count(umi_language_service_signature(b->language)) == 1U);
    return 0;
}

static int Case_editor_completion(UmiLanguageRuntimeServiceBridge *b)
{
    UmiEditorSession *editor = NULL;
    UmiLanguageRuntimeEditorBridge bridge;
    CHECK(umi_editor_session_create(&editor) == UMI_STATUS_OK);
    CHECK(umi_language_runtime_editor_bridge_init(&bridge, b->language, editor) == UMI_STATUS_OK);
    UmiLanguageCompletionRegistry *source = umi_language_service_completion(b->language);
    UmiEditorCompletionRegistry *target = umi_editor_session_completion(editor);
    UmiLanguageCompletionSnapshot input = {0};
    UmiEditorCompletionSnapshot old = {0}, foreign = {0}, after;
    memcpy(old.id, "old", sizeof("old")); memcpy(old.document_id, "a", sizeof("a"));
    memcpy(foreign.id, "collision", sizeof("collision")); memcpy(foreign.document_id, "b", sizeof("b"));
    CHECK(umi_editor_completion_registry_upsert(target, &old) == UMI_STATUS_OK);
    CHECK(umi_editor_completion_registry_upsert(target, &foreign) == UMI_STATUS_OK);
    CHECK(umi_editor_completion_registry_find(target, "old", &old) == UMI_STATUS_OK);
    memcpy(input.id, "first", sizeof("first")); memcpy(input.document_id, "a", sizeof("a"));
    memcpy(input.label, "value", sizeof("value"));
    CHECK(umi_language_completion_registry_upsert(source, &input) == UMI_STATUS_OK);
    memcpy(input.id, "collision", sizeof("collision"));
    CHECK(umi_language_completion_registry_upsert(source, &input) == UMI_STATUS_OK);
    const uint64_t revision = umi_editor_completion_registry_revision(target), before_bridge = bridge.revision;
    CHECK(umi_language_runtime_editor_bridge_sync_completion(&bridge, "a") == UMI_STATUS_PERMISSION_DENIED);
    CHECK(umi_editor_completion_registry_revision(target) == revision && bridge.revision == before_bridge);
    CHECK(umi_editor_completion_registry_count(target) == 2U);
    CHECK(umi_editor_completion_registry_find(target, "old", &after) == UMI_STATUS_OK && after.revision == old.revision);
    CHECK(umi_editor_completion_registry_remove(target, "collision") == UMI_STATUS_OK);
    CHECK(umi_language_runtime_editor_bridge_sync_completion(&bridge, "a") == UMI_STATUS_OK);
    CHECK(umi_editor_completion_registry_count(target) == 2U);
    CHECK(umi_editor_completion_registry_find(target, "first", &after) == UMI_STATUS_OK && strcmp(after.label, "value") == 0);
    CHECK(umi_language_completion_registry_remove(source, "first") == UMI_STATUS_OK);
    CHECK(umi_language_completion_registry_remove(source, "collision") == UMI_STATUS_OK);
    CHECK(umi_language_runtime_editor_bridge_sync_completion(&bridge, "a") == UMI_STATUS_OK);
    CHECK(umi_editor_completion_registry_count(target) == 0U);
    bridge.revision = UINT64_MAX;
    CHECK(umi_language_runtime_editor_bridge_sync_completion(&bridge, "a") == UMI_STATUS_CAPACITY_EXCEEDED);
    umi_editor_session_destroy(editor);
    return 0;
}

static int Case_editor_diagnostic(UmiLanguageRuntimeServiceBridge *b)
{
    UmiEditorSession *editor = NULL;
    UmiLanguageRuntimeEditorBridge bridge;
    CHECK(umi_editor_session_create(&editor) == UMI_STATUS_OK);
    CHECK(umi_language_runtime_editor_bridge_init(&bridge, b->language, editor) == UMI_STATUS_OK);
    UmiLanguageDiagnosticRegistry *source = umi_language_service_diagnostic(b->language);
    UmiEditorDiagnosticRegistry *target = umi_editor_session_diagnostic(editor);
    UmiLanguageDiagnosticSnapshot input = {0};
    UmiEditorDiagnosticSnapshot old = {0}, foreign = {0}, after;
    memcpy(old.id, "old", sizeof("old")); memcpy(old.document_id, "a", sizeof("a"));
    memcpy(foreign.id, "collision", sizeof("collision")); memcpy(foreign.document_id, "b", sizeof("b"));
    CHECK(umi_editor_diagnostic_registry_upsert(target, &old) == UMI_STATUS_OK);
    CHECK(umi_editor_diagnostic_registry_upsert(target, &foreign) == UMI_STATUS_OK);
    CHECK(umi_editor_diagnostic_registry_find(target, "old", &old) == UMI_STATUS_OK);
    memcpy(input.id, "first", sizeof("first")); memcpy(input.document_id, "a", sizeof("a"));
    memcpy(input.message, "value", sizeof("value"));
    CHECK(umi_language_diagnostic_registry_upsert(source, &input) == UMI_STATUS_OK);
    memcpy(input.id, "collision", sizeof("collision"));
    CHECK(umi_language_diagnostic_registry_upsert(source, &input) == UMI_STATUS_OK);
    const uint64_t revision = umi_editor_diagnostic_registry_revision(target), before_bridge = bridge.revision;
    CHECK(umi_language_runtime_editor_bridge_sync_diagnostics(&bridge, "a") == UMI_STATUS_PERMISSION_DENIED);
    CHECK(umi_editor_diagnostic_registry_revision(target) == revision && bridge.revision == before_bridge);
    CHECK(umi_editor_diagnostic_registry_count(target) == 2U);
    CHECK(umi_editor_diagnostic_registry_find(target, "old", &after) == UMI_STATUS_OK && after.revision == old.revision);
    CHECK(umi_editor_diagnostic_registry_remove(target, "collision") == UMI_STATUS_OK);
    CHECK(umi_language_runtime_editor_bridge_sync_diagnostics(&bridge, "a") == UMI_STATUS_OK);
    CHECK(umi_editor_diagnostic_registry_count(target) == 2U);
    CHECK(umi_editor_diagnostic_registry_find(target, "first", &after) == UMI_STATUS_OK && strcmp(after.message, "value") == 0);
    CHECK(umi_language_diagnostic_registry_remove(source, "first") == UMI_STATUS_OK);
    CHECK(umi_language_diagnostic_registry_remove(source, "collision") == UMI_STATUS_OK);
    CHECK(umi_language_runtime_editor_bridge_sync_diagnostics(&bridge, "a") == UMI_STATUS_OK);
    CHECK(umi_editor_diagnostic_registry_count(target) == 0U);
    bridge.revision = UINT64_MAX;
    CHECK(umi_language_runtime_editor_bridge_sync_diagnostics(&bridge, "a") == UMI_STATUS_CAPACITY_EXCEEDED);
    umi_editor_session_destroy(editor);
    return 0;
}

static int Case_editor_symbol(UmiLanguageRuntimeServiceBridge *b)
{
    UmiEditorSession *editor = NULL;
    UmiLanguageRuntimeEditorBridge bridge;
    CHECK(umi_editor_session_create(&editor) == UMI_STATUS_OK);
    CHECK(umi_language_runtime_editor_bridge_init(&bridge, b->language, editor) == UMI_STATUS_OK);
    UmiLanguageSymbolRegistry *source = umi_language_service_symbol(b->language);
    UmiEditorSymbolRegistry *target = umi_editor_session_symbol(editor);
    UmiLanguageSymbolSnapshot input = {0};
    UmiEditorSymbolSnapshot old = {0}, foreign = {0}, after;
    memcpy(old.id, "old", sizeof("old")); memcpy(old.document_id, "a", sizeof("a"));
    memcpy(foreign.id, "collision", sizeof("collision")); memcpy(foreign.document_id, "b", sizeof("b"));
    CHECK(umi_editor_symbol_registry_upsert(target, &old) == UMI_STATUS_OK);
    CHECK(umi_editor_symbol_registry_upsert(target, &foreign) == UMI_STATUS_OK);
    CHECK(umi_editor_symbol_registry_find(target, "old", &old) == UMI_STATUS_OK);
    memcpy(input.id, "first", sizeof("first")); memcpy(input.document_id, "a", sizeof("a"));
    memcpy(input.name, "value", sizeof("value"));
    CHECK(umi_language_symbol_registry_upsert(source, &input) == UMI_STATUS_OK);
    memcpy(input.id, "collision", sizeof("collision"));
    CHECK(umi_language_symbol_registry_upsert(source, &input) == UMI_STATUS_OK);
    const uint64_t revision = umi_editor_symbol_registry_revision(target), before_bridge = bridge.revision;
    CHECK(umi_language_runtime_editor_bridge_sync_symbols(&bridge, "a") == UMI_STATUS_PERMISSION_DENIED);
    CHECK(umi_editor_symbol_registry_revision(target) == revision && bridge.revision == before_bridge);
    CHECK(umi_editor_symbol_registry_count(target) == 2U);
    CHECK(umi_editor_symbol_registry_find(target, "old", &after) == UMI_STATUS_OK && after.revision == old.revision);
    CHECK(umi_editor_symbol_registry_remove(target, "collision") == UMI_STATUS_OK);
    CHECK(umi_language_runtime_editor_bridge_sync_symbols(&bridge, "a") == UMI_STATUS_OK);
    CHECK(umi_editor_symbol_registry_count(target) == 2U);
    CHECK(umi_editor_symbol_registry_find(target, "first", &after) == UMI_STATUS_OK && strcmp(after.name, "value") == 0);
    CHECK(umi_language_symbol_registry_remove(source, "first") == UMI_STATUS_OK);
    CHECK(umi_language_symbol_registry_remove(source, "collision") == UMI_STATUS_OK);
    CHECK(umi_language_runtime_editor_bridge_sync_symbols(&bridge, "a") == UMI_STATUS_OK);
    CHECK(umi_editor_symbol_registry_count(target) == 0U);
    bridge.revision = UINT64_MAX;
    CHECK(umi_language_runtime_editor_bridge_sync_symbols(&bridge, "a") == UMI_STATUS_CAPACITY_EXCEEDED);
    umi_editor_session_destroy(editor);
    return 0;
}

static int Case_editor_code_action(UmiLanguageRuntimeServiceBridge *b)
{
    UmiEditorSession *editor = NULL;
    UmiLanguageRuntimeEditorBridge bridge;
    CHECK(umi_editor_session_create(&editor) == UMI_STATUS_OK);
    CHECK(umi_language_runtime_editor_bridge_init(&bridge, b->language, editor) == UMI_STATUS_OK);
    UmiLanguageCodeActionRegistry *source = umi_language_service_code_action(b->language);
    UmiEditorCodeActionRegistry *target = umi_editor_session_code_action(editor);
    UmiLanguageCodeActionSnapshot input = {0};
    UmiEditorCodeActionSnapshot old = {0}, foreign = {0}, after;
    memcpy(old.id, "old", sizeof("old")); memcpy(old.document_id, "a", sizeof("a"));
    memcpy(foreign.id, "collision", sizeof("collision")); memcpy(foreign.document_id, "b", sizeof("b"));
    CHECK(umi_editor_code_action_registry_upsert(target, &old) == UMI_STATUS_OK);
    CHECK(umi_editor_code_action_registry_upsert(target, &foreign) == UMI_STATUS_OK);
    CHECK(umi_editor_code_action_registry_find(target, "old", &old) == UMI_STATUS_OK);
    memcpy(input.id, "first", sizeof("first")); memcpy(input.document_id, "a", sizeof("a"));
    memcpy(input.title, "value", sizeof("value"));
    CHECK(umi_language_code_action_registry_upsert(source, &input) == UMI_STATUS_OK);
    memcpy(input.id, "collision", sizeof("collision"));
    CHECK(umi_language_code_action_registry_upsert(source, &input) == UMI_STATUS_OK);
    const uint64_t revision = umi_editor_code_action_registry_revision(target), before_bridge = bridge.revision;
    CHECK(umi_language_runtime_editor_bridge_sync_code_actions(&bridge, "a") == UMI_STATUS_PERMISSION_DENIED);
    CHECK(umi_editor_code_action_registry_revision(target) == revision && bridge.revision == before_bridge);
    CHECK(umi_editor_code_action_registry_count(target) == 2U);
    CHECK(umi_editor_code_action_registry_find(target, "old", &after) == UMI_STATUS_OK && after.revision == old.revision);
    CHECK(umi_editor_code_action_registry_remove(target, "collision") == UMI_STATUS_OK);
    CHECK(umi_language_runtime_editor_bridge_sync_code_actions(&bridge, "a") == UMI_STATUS_OK);
    CHECK(umi_editor_code_action_registry_count(target) == 2U);
    CHECK(umi_editor_code_action_registry_find(target, "first", &after) == UMI_STATUS_OK && strcmp(after.title, "value") == 0);
    CHECK(umi_language_code_action_registry_remove(source, "first") == UMI_STATUS_OK);
    CHECK(umi_language_code_action_registry_remove(source, "collision") == UMI_STATUS_OK);
    CHECK(umi_language_runtime_editor_bridge_sync_code_actions(&bridge, "a") == UMI_STATUS_OK);
    CHECK(umi_editor_code_action_registry_count(target) == 0U);
    bridge.revision = UINT64_MAX;
    CHECK(umi_language_runtime_editor_bridge_sync_code_actions(&bridge, "a") == UMI_STATUS_CAPACITY_EXCEEDED);
    umi_editor_session_destroy(editor);
    return 0;
}

static int Case_single_records(UmiLanguageRuntimeServiceBridge *b)
{
    UmiLanguageFormattingRegistry *formatting = umi_language_service_formatting(b->language);
    UmiLanguageRenameRegistry *renames = umi_language_service_rename(b->language);
    UmiLanguageFormattingSnapshot format_before, format_after;
    UmiLanguageRenameSnapshot rename_before, rename_after;
    UmiLanguageRuntimeWorkspaceEdit *edit = calloc(1U, sizeof(*edit));
    CHECK(edit != NULL);
    CHECK(umi_language_runtime_publish_formatting_available(b, "a", "provider", 4U, 1) == UMI_STATUS_OK);
    CHECK(umi_language_formatting_registry_at(formatting, 0U, &format_before) == UMI_STATUS_OK);
    edit->count = 1U;
    CHECK(umi_language_runtime_publish_rename(b, "a", "symbol", "before", "after", edit) == UMI_STATUS_OK);
    CHECK(umi_language_rename_registry_at(renames, 0U, &rename_before) == UMI_STATUS_OK);
    const uint64_t bridge_revision = b->revision;
    char provider[sizeof(format_before.provider_id) + 1U];
    memset(provider, 'x', sizeof(provider)); provider[sizeof(provider) - 1U] = '\0';
    CHECK(umi_language_runtime_publish_formatting_available(b, "a", provider, 4U, 1) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_language_runtime_publish_formatting_available(b, "a", "provider", 0U, 1) == UMI_STATUS_INVALID_ARGUMENT);
    edit->count = sizeof(edit->items) / sizeof(edit->items[0]) + 1U;
    CHECK(umi_language_runtime_publish_rename(b, "a", "symbol", "before", "after", edit) == UMI_STATUS_CAPACITY_EXCEEDED);
    edit->count = 1U;
    char name[sizeof(rename_before.new_name) + 1U];
    memset(name, 'x', sizeof(name)); name[sizeof(name) - 1U] = '\0';
    CHECK(umi_language_runtime_publish_rename(b, "a", "symbol", "before", name, edit) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(b->revision == bridge_revision);
    CHECK(umi_language_formatting_registry_revision(formatting) == format_before.revision);
    CHECK(umi_language_rename_registry_revision(renames) == rename_before.revision);
    CHECK(umi_language_formatting_registry_at(formatting, 0U, &format_after) == UMI_STATUS_OK);
    CHECK(strcmp(format_after.provider_id, format_before.provider_id) == 0 && format_after.tab_size == 4U);
    CHECK(umi_language_rename_registry_at(renames, 0U, &rename_after) == UMI_STATUS_OK);
    CHECK(strcmp(rename_after.new_name, rename_before.new_name) == 0);
    free(edit);
    return 0;
}

static int Case_editor_document(UmiLanguageRuntimeServiceBridge *b)
{
    UmiEditorSession *editor = NULL;
    UmiLanguageRuntimeEditorBridge bridge;
    UmiLanguageDocumentSnapshot source = {0};
    UmiEditorDocumentSnapshot before, after;
    CHECK(umi_editor_session_create(&editor) == UMI_STATUS_OK);
    CHECK(umi_language_runtime_editor_bridge_init(&bridge, b->language, editor) == UMI_STATUS_OK);
    memcpy(source.id, "a", sizeof("a"));
    memcpy(source.uri, "file:///main.c", sizeof("file:///main.c"));
    memcpy(source.language_id, "c", sizeof("c"));
    source.version = 7U; source.line_count = 4U; source.dirty = 1;
    CHECK(umi_language_document_registry_upsert(umi_language_service_document(b->language), &source) == UMI_STATUS_OK);
    CHECK(umi_language_runtime_editor_bridge_sync_document(&bridge, "a", "main.c", 100U) == UMI_STATUS_OK);
    UmiEditorDocumentRegistry *target = umi_editor_session_document(editor);
    CHECK(umi_editor_document_registry_find(target, "a", &before) == UMI_STATUS_OK);
    CHECK(before.version == 7U && before.byte_count == 100U && before.line_count == 4U && before.dirty == 1);
    const uint64_t revision = umi_editor_document_registry_revision(target), bridge_revision = bridge.revision;
    char title[sizeof(before.title) + 1U];
    memset(title, 'x', sizeof(title)); title[sizeof(title) - 1U] = '\0';
    CHECK(umi_language_runtime_editor_bridge_sync_document(&bridge, "a", title, 200U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_editor_document_registry_revision(target) == revision && bridge.revision == bridge_revision);
    CHECK(umi_editor_document_registry_find(target, "a", &after) == UMI_STATUS_OK);
    CHECK(strcmp(after.title, "main.c") == 0 && after.byte_count == 100U && after.revision == before.revision);
    CHECK(umi_language_runtime_editor_bridge_sync_document(&bridge, "missing", "missing.c", 0U) == UMI_STATUS_NOT_FOUND);
    CHECK(umi_editor_document_registry_revision(target) == revision);
    umi_editor_session_destroy(editor);
    return 0;
}

int main(int argc, char **argv)
{
    UmiLanguageService *language = NULL;
    UmiLanguageRuntimeServiceBridge bridge;
    int result = 2;
    if (argc != 2) return 2;
    CHECK(umi_language_service_create(&language) == UMI_STATUS_OK);
    CHECK(umi_language_runtime_service_bridge_init(&bridge, language) == UMI_STATUS_OK);
    if (strcmp(argv[1], "completion") == 0) result = Case_completion(&bridge);
    if (strcmp(argv[1], "reference") == 0) result = Case_reference(&bridge);
    if (strcmp(argv[1], "symbol") == 0) result = Case_symbol(&bridge);
    if (strcmp(argv[1], "diagnostic") == 0) result = Case_diagnostic(&bridge);
    if (strcmp(argv[1], "code_action") == 0) result = Case_code_action(&bridge);
    if (strcmp(argv[1], "inlay_hint") == 0) result = Case_inlay_hint(&bridge);
    if (strcmp(argv[1], "folding_range") == 0) result = Case_folding_range(&bridge);
    if (strcmp(argv[1], "semantic") == 0) result = Case_semantic(&bridge);
    if (strcmp(argv[1], "hover_signature") == 0) result = Case_hover_signature(&bridge);
    if (strcmp(argv[1], "editor_completion") == 0) result = Case_editor_completion(&bridge);
    if (strcmp(argv[1], "editor_diagnostic") == 0) result = Case_editor_diagnostic(&bridge);
    if (strcmp(argv[1], "editor_symbol") == 0) result = Case_editor_symbol(&bridge);
    if (strcmp(argv[1], "editor_code_action") == 0) result = Case_editor_code_action(&bridge);
    if (strcmp(argv[1], "single_records") == 0) result = Case_single_records(&bridge);
    if (strcmp(argv[1], "editor_document") == 0) result = Case_editor_document(&bridge);
    umi_language_service_destroy(language);
    return result;
}
