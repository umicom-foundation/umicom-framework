/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_page_navigation.c
 * PURPOSE: Exercise diagnostic page boundaries, shrinking queries and immutable evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/diagnostic_ui/page_navigation.h"
#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "%d: %s\n", __LINE__, #expr); result = 1; goto done; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *cases[] = {"empty", "first", "middle", "last", "exact-boundary", "unaligned", "huge-offset", "default-limit", "single-row", "filter", "resolved", "severity", "shrink", "clear", "retained", "invalid-limit", "invalid-query", "arguments", "arbitrary-offset"};
    int known = 0, result = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) if (strcmp(argv[1], cases[i]) == 0) known = 1;
    if (!known) return 2;
    const char *mode = argv[1];
    UmiDiagnosticModel *model = NULL;
    UmiDiagnosticPage *page = NULL, *replacement = NULL;
    UmiDiagnosticModelConfig config = {1024U};
    UmiDiagnosticQuery query;
    UmiDiagnosticPageNavigation navigation;
    CHECK(umi_diagnostic_model_create(&config, &model) == UMI_STATUS_OK);
    umi_diagnostic_query_init(&query); query.limit = 300U;
    query.sort = UMI_DIAGNOSTIC_SORT_SEQUENCE_ASCENDING;
    size_t total = strcmp(mode, "empty") == 0 ? 0U : strcmp(mode, "exact-boundary") == 0 ? 600U : 907U;
    for (size_t i = 0U; i < total; ++i) {
        UmiDiagnosticSnapshot item;
        char id[64]; snprintf(id, sizeof id, "problem.%04zu", i);
        CHECK(umi_diagnostic_snapshot_init(&item, id, (UmiDiagnosticSeverity)(i % 5U),
            UMI_DIAGNOSTIC_KIND_COMPILER, "cc", i % 2U == 0U ? "alpha" : "beta") == UMI_STATUS_OK);
        CHECK(umi_diagnostic_model_upsert(model, &item) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "middle") == 0) query.offset = 300U;
    if (strcmp(mode, "last") == 0 || strcmp(mode, "exact-boundary") == 0 ||
        strcmp(mode, "shrink") == 0 || strcmp(mode, "clear") == 0) query.offset = 900U;
    if (strcmp(mode, "unaligned") == 0) query.offset = 578U;
    if (strcmp(mode, "huge-offset") == 0) query.offset = SIZE_MAX;
    if (strcmp(mode, "default-limit") == 0) query.limit = 0U;
    if (strcmp(mode, "single-row") == 0) { query.limit = 1U; query.offset = SIZE_MAX; }
    if (strcmp(mode, "filter") == 0) { strcpy(query.filter.text, "beta"); query.offset = 900U; }
    if (strcmp(mode, "resolved") == 0) {
        for (size_t i = 0U; i < 900U; ++i) {
            char id[64]; snprintf(id, sizeof id, "problem.%04zu", i);
            CHECK(umi_diagnostic_model_resolve(model, id, 1) == UMI_STATUS_OK);
        }
        query.offset = 900U;
    }
    if (strcmp(mode, "severity") == 0) query.sort = UMI_DIAGNOSTIC_SORT_SEVERITY_DESCENDING;
    CHECK(UmiDiagnosticPageCreateWindow(model, &query, &page, &navigation) == UMI_STATUS_OK);
    CHECK(UmiDiagnosticPageValidate(page, model, &navigation.query) == UMI_STATUS_OK);
    CHECK(navigation.summary.count <= navigation.query.limit);
    if (strcmp(mode, "empty") == 0) {
        CHECK(navigation.page_count == 0U && navigation.page_index == 0U && navigation.query.offset == 0U);
        CHECK(navigation.summary.count == 0U && !navigation.has_previous && !navigation.has_next);
    } else if (strcmp(mode, "middle") == 0 || strcmp(mode, "unaligned") == 0) {
        CHECK(navigation.query.offset == 300U && navigation.page_index == 1U && navigation.page_count == 4U);
        CHECK(navigation.has_previous && navigation.previous_offset == 0U && navigation.has_next && navigation.next_offset == 600U);
        CHECK(strcmp(UmiDiagnosticPageItem(page, 0U)->id, "problem.0300") == 0);
    } else if (strcmp(mode, "last") == 0 || strcmp(mode, "huge-offset") == 0) {
        CHECK(navigation.query.offset == 900U && navigation.summary.count == 7U && navigation.page_index == 3U);
        CHECK(navigation.has_previous && navigation.previous_offset == 600U && !navigation.has_next && navigation.next_offset == 900U);
        CHECK(strcmp(UmiDiagnosticPageItem(page, 6U)->id, "problem.0906") == 0);
    } else if (strcmp(mode, "exact-boundary") == 0) {
        CHECK(navigation.query.offset == 300U && navigation.page_count == 2U && navigation.summary.count == 300U);
        CHECK(!navigation.has_next && strcmp(UmiDiagnosticPageItem(page, 299U)->id, "problem.0599") == 0);
    } else if (strcmp(mode, "single-row") == 0) {
        CHECK(navigation.page_count == 907U && navigation.query.offset == 906U && navigation.summary.count == 1U);
        CHECK(navigation.previous_offset == 905U && !navigation.has_next);
    } else if (strcmp(mode, "filter") == 0) {
        CHECK(navigation.query.offset == 300U && navigation.page_count == 2U && navigation.summary.count == 153U);
        CHECK(strcmp(UmiDiagnosticPageItem(page, 0U)->id, "problem.0601") == 0);
        for (size_t i = 0U; i < navigation.summary.count; ++i) CHECK(strcmp(UmiDiagnosticPageItem(page, i)->message, "beta") == 0);
    } else if (strcmp(mode, "resolved") == 0) {
        CHECK(navigation.query.offset == 0U && navigation.summary.count == 7U && navigation.summary.resolved == 900U);
        query.filter.include_resolved = 1;
        CHECK(UmiDiagnosticPageCreateWindow(model, &query, &replacement, &navigation) == UMI_STATUS_OK);
        CHECK(navigation.query.offset == 900U && navigation.page_count == 4U && navigation.summary.total_matches == 907U);
    } else if (strcmp(mode, "severity") == 0) {
        for (size_t i = 1U; i < navigation.summary.count; ++i)
            CHECK(UmiDiagnosticPageItem(page, i - 1U)->severity >= UmiDiagnosticPageItem(page, i)->severity);
    } else if (strcmp(mode, "shrink") == 0 || strcmp(mode, "clear") == 0 || strcmp(mode, "retained") == 0) {
        const UmiDiagnosticSnapshot *retained = UmiDiagnosticPageItem(page, 0U);
        char before[UMI_DIAGNOSTIC_ID_CAPACITY]; strcpy(before, retained->id);
        if (strcmp(mode, "shrink") == 0) {
            for (size_t i = 301U; i < 907U; ++i) {
                char id[64]; snprintf(id, sizeof id, "problem.%04zu", i);
                CHECK(umi_diagnostic_model_remove(model, id) == UMI_STATUS_OK);
            }
        } else umi_diagnostic_model_clear(model);
        CHECK(UmiDiagnosticPageValidate(page, model, &navigation.query) == UMI_STATUS_INVALID_STATE);
        CHECK(strcmp(retained->id, before) == 0);
        CHECK(UmiDiagnosticPageCreateWindow(model, &query, &replacement, &navigation) == UMI_STATUS_OK);
        if (strcmp(mode, "shrink") == 0) CHECK(navigation.query.offset == 300U && navigation.summary.count == 1U);
        else CHECK(navigation.query.offset == 0U && navigation.page_count == 0U);
        if (strcmp(mode, "retained") == 0) {
            umi_diagnostic_model_destroy(model); model = NULL;
            CHECK(strcmp(retained->id, before) == 0); /* Reading owned evidence needs no model. */
        }
    } else if (strcmp(mode, "invalid-limit") == 0 || strcmp(mode, "invalid-query") == 0 || strcmp(mode, "arguments") == 0) {
        UmiDiagnosticPage *untouched = page;
        UmiDiagnosticPageNavigation before;
        memcpy(&before, &navigation, sizeof before);
        if (strcmp(mode, "invalid-limit") == 0) {
            query.limit = 301U;
            CHECK(UmiDiagnosticPageCreateWindow(model, &query, &untouched, &navigation) == UMI_STATUS_CAPACITY_EXCEEDED);
        } else if (strcmp(mode, "invalid-query") == 0) {
            memset(query.filter.text, 'x', sizeof query.filter.text);
            CHECK(UmiDiagnosticPageCreateWindow(model, &query, &untouched, &navigation) == UMI_STATUS_INVALID_ARGUMENT);
        } else {
            CHECK(UmiDiagnosticPageCreateWindow(NULL, &query, &untouched, &navigation) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiDiagnosticPageCreateWindow(model, NULL, &untouched, &navigation) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiDiagnosticPageCreateWindow(model, &query, NULL, &navigation) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiDiagnosticPageCreateWindow(model, &query, &untouched, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        }
        CHECK(untouched == page && memcmp(&before, &navigation, sizeof before) == 0);
    } else if (strcmp(mode, "arbitrary-offset") == 0) {
        query.offset = 301U;
        CHECK(UmiDiagnosticPageCreate(model, &query, &replacement) == UMI_STATUS_OK);
        CHECK(strcmp(UmiDiagnosticPageItem(replacement, 0U)->id, "problem.0301") == 0);
    } else {
        CHECK(navigation.query.limit == 300U && navigation.query.offset == 0U && navigation.page_count == 4U);
        CHECK(navigation.summary.count == 300U && !navigation.has_previous && navigation.previous_offset == 0U);
        CHECK(navigation.has_next && navigation.next_offset == 300U);
    }
done:
    UmiDiagnosticPageDestroy(replacement); UmiDiagnosticPageDestroy(page);
    umi_diagnostic_model_destroy(model);
    return result;
}
