/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_page.c
 * PURPOSE: Check retained pages against real diagnostic model mutations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostic_ui/page.h"
#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "%d: %s\n", __LINE__, #expr); result = 1; goto done; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *cases[] = {"empty", "pagination", "offset", "beyond-end", "filter", "resolved", "severity-order", "summary", "model-change", "model-clear", "model-identity", "query-change", "query-equivalence", "invalid-text", "invalid-limit", "arguments"};
    int known = 0, result = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) if (strcmp(argv[1], cases[i]) == 0) known = 1;
    if (!known) return 2;
    const char *mode = argv[1];
    UmiDiagnosticModel *model = NULL, *other = NULL;
    UmiDiagnosticPage *page = NULL, *second = NULL;
    UmiDiagnosticModelConfig config = {400U};
    UmiDiagnosticQuery query;
    UmiDiagnosticPageSummary summary;
    CHECK(umi_diagnostic_model_create(&config, &model) == UMI_STATUS_OK);
    umi_diagnostic_query_init(&query); query.limit = 0U;
    query.sort = UMI_DIAGNOSTIC_SORT_SEQUENCE_DESCENDING;
    if (strcmp(mode, "empty") != 0) {
        for (unsigned i = 0U; i < 355U; ++i) {
            UmiDiagnosticSnapshot item;
            char id[64]; snprintf(id, sizeof id, "compiler.item.%u", i);
            CHECK(umi_diagnostic_snapshot_init(&item, id, (UmiDiagnosticSeverity)(i % 5U),
                UMI_DIAGNOSTIC_KIND_COMPILER, "cc", i % 2U == 0U ? "alpha message" : "beta message") == UMI_STATUS_OK);
            CHECK(umi_diagnostic_model_upsert(model, &item) == UMI_STATUS_OK);
        }
        CHECK(umi_diagnostic_model_resolve(model, "compiler.item.2", 1) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "offset") == 0) { query.offset = 300U; query.limit = 80U; }
    if (strcmp(mode, "beyond-end") == 0) query.offset = SIZE_MAX;
    if (strcmp(mode, "filter") == 0) strcpy(query.filter.text, "alpha");
    if (strcmp(mode, "resolved") == 0) query.filter.include_resolved = 1;
    if (strcmp(mode, "severity-order") == 0) query.sort = UMI_DIAGNOSTIC_SORT_SEVERITY_DESCENDING;
    CHECK(UmiDiagnosticPageCreate(model, &query, &page) == UMI_STATUS_OK);
    CHECK(UmiDiagnosticPageReadSummary(page, &summary) == UMI_STATUS_OK);
    CHECK(UmiDiagnosticPageValidate(page, model, &query) == UMI_STATUS_OK);
    if (strcmp(mode, "empty") == 0) CHECK(summary.count == 0U && summary.active == 0U && !summary.has_more);
    else if (strcmp(mode, "pagination") == 0) {
        CHECK(summary.count == 300U && summary.total_matches == 354U && summary.has_more);
        CHECK(strcmp(UmiDiagnosticPageItem(page, 0U)->id, "compiler.item.354") == 0);
        CHECK(strcmp(UmiDiagnosticPageItem(page, 299U)->id, "compiler.item.55") == 0);
        CHECK(UmiDiagnosticPageItem(page, 300U) == NULL);
    } else if (strcmp(mode, "offset") == 0) {
        CHECK(summary.count == 54U && !summary.has_more && summary.total_matches == 354U);
        CHECK(strcmp(UmiDiagnosticPageItem(page, 0U)->id, "compiler.item.54") == 0);
    } else if (strcmp(mode, "beyond-end") == 0) CHECK(summary.count == 0U && !summary.has_more);
    else if (strcmp(mode, "filter") == 0) {
        CHECK(summary.total_matches == 177U && summary.count == 177U && !summary.has_more);
        for (size_t i = 0U; i < summary.count; ++i) CHECK(strstr(UmiDiagnosticPageItem(page, i)->message, "alpha") != NULL);
    } else if (strcmp(mode, "resolved") == 0) CHECK(summary.total_matches == 355U && summary.resolved == 1U);
    else if (strcmp(mode, "severity-order") == 0) {
        for (size_t i = 1U; i < summary.count; ++i) {
            const UmiDiagnosticSnapshot *a = UmiDiagnosticPageItem(page, i - 1U), *b = UmiDiagnosticPageItem(page, i);
            CHECK(a->severity >= b->severity);
            CHECK(a->severity != b->severity || a->sequence > b->sequence);
        }
    } else if (strcmp(mode, "summary") == 0) {
        CHECK(summary.active == 354U && summary.resolved == 1U);
        CHECK(summary.warnings == 70U && summary.errors == 71U && summary.fatal == 71U);
    } else if (strcmp(mode, "model-change") == 0 || strcmp(mode, "model-clear") == 0) {
        const UmiDiagnosticSnapshot *before = UmiDiagnosticPageItem(page, 0U);
        CHECK(before != NULL);
        if (strcmp(mode, "model-clear") == 0) umi_diagnostic_model_clear(model);
        else CHECK(umi_diagnostic_model_remove(model, before->id) == UMI_STATUS_OK);
        CHECK(UmiDiagnosticPageValidate(page, model, &query) == UMI_STATUS_INVALID_STATE);
        CHECK(strcmp(before->id, "compiler.item.354") == 0 && strcmp(before->message, "alpha message") == 0);
    } else if (strcmp(mode, "model-identity") == 0) {
        CHECK(umi_diagnostic_model_create(&config, &other) == UMI_STATUS_OK);
        CHECK(UmiDiagnosticPageValidate(page, other, &query) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(mode, "query-change") == 0) {
        for (unsigned i = 0U; i < 10U; ++i) {
            UmiDiagnosticQuery changed = query;
            switch (i) {
            case 0U: changed.offset = 1U; break;
            case 1U: changed.limit = 1U; break;
            case 2U: changed.sort = UMI_DIAGNOSTIC_SORT_SEQUENCE_ASCENDING; break;
            case 3U: changed.filter.severity_mask = 0U; break;
            case 4U: changed.filter.kind_mask = 0U; break;
            case 5U: changed.filter.include_resolved = !query.filter.include_resolved; break;
            case 6U: changed.filter.include_transient = !query.filter.include_transient; break;
            case 7U: strcpy(changed.filter.source, "other"); break;
            case 8U: strcpy(changed.filter.owner, "other"); break;
            case 9U: strcpy(changed.filter.text, "other"); break;
            }
            CHECK(UmiDiagnosticPageValidate(page, model, &changed) == UMI_STATUS_INVALID_STATE);
        }
    } else if (strcmp(mode, "query-equivalence") == 0) {
        UmiDiagnosticQuery equivalent = query;
        equivalent.limit = UMI_DIAGNOSTIC_PAGE_LIMIT;
        equivalent.filter.text[20] = 'x'; /* Bytes after NUL do not alter a query. */
        equivalent.filter.include_transient = query.filter.include_transient ? 8 : 0;
        CHECK(UmiDiagnosticPageValidate(page, model, &equivalent) == UMI_STATUS_OK);
    } else if (strcmp(mode, "invalid-text") == 0 || strcmp(mode, "invalid-limit") == 0) {
        UmiDiagnosticPage *untouched = page;
        if (strcmp(mode, "invalid-limit") == 0) query.limit = UMI_DIAGNOSTIC_PAGE_LIMIT + 1U;
        else memset(query.filter.text, 'x', sizeof query.filter.text);
        UmiStatus expected = strcmp(mode, "invalid-limit") == 0 ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_ARGUMENT;
        CHECK(UmiDiagnosticPageCreate(model, &query, &untouched) == expected && untouched == page);
        CHECK(UmiDiagnosticPageValidate(page, model, &query) == expected);
    } else if (strcmp(mode, "arguments") == 0) {
        CHECK(UmiDiagnosticPageCreate(NULL, &query, &second) == UMI_STATUS_INVALID_ARGUMENT && second == NULL);
        CHECK(UmiDiagnosticPageCreate(model, NULL, &second) == UMI_STATUS_INVALID_ARGUMENT && second == NULL);
        CHECK(UmiDiagnosticPageCreate(model, &query, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDiagnosticPageReadSummary(NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDiagnosticPageReadSummary(page, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDiagnosticPageItem(NULL, 0U) == NULL);
        CHECK(UmiDiagnosticPageValidate(NULL, model, &query) == UMI_STATUS_INVALID_ARGUMENT);
        query.sort = (UmiDiagnosticSort)99;
        CHECK(UmiDiagnosticPageCreate(model, &query, &second) == UMI_STATUS_INVALID_ARGUMENT && second == NULL);
    }
 done:
    UmiDiagnosticPageDestroy(second); UmiDiagnosticPageDestroy(page);
    umi_diagnostic_model_destroy(other); umi_diagnostic_model_destroy(model);
    return result;
}
