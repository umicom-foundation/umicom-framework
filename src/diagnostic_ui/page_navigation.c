/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostic_ui/page_navigation.c
 * PURPOSE: Provide overflow-safe page boundaries over immutable diagnostic captures.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/diagnostic_ui/page_navigation.h"
#include <string.h>
UmiStatus UmiDiagnosticPageCreateWindow(const UmiDiagnosticModel *model,
    const UmiDiagnosticQuery *query, UmiDiagnosticPage **out_page,
    UmiDiagnosticPageNavigation *out_navigation)
{
    if (model == NULL || query == NULL || out_page == NULL || out_navigation == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDiagnosticPageNavigation navigation;
    memset(&navigation, 0, sizeof navigation);
    navigation.query = *query;
    if (navigation.query.limit > UMI_DIAGNOSTIC_PAGE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (navigation.query.limit == 0U) navigation.query.limit = UMI_DIAGNOSTIC_PAGE_LIMIT;
    navigation.query.offset = (query->offset / navigation.query.limit) * navigation.query.limit;
    UmiDiagnosticPage *page = NULL;
    uint64_t revision = umi_diagnostic_model_revision(model);
    UmiStatus status = UmiDiagnosticPageCreate(model, &navigation.query, &page);
    if (status != UMI_STATUS_OK) return status;
    status = UmiDiagnosticPageReadSummary(page, &navigation.summary);
    if (status != UMI_STATUS_OK) goto failed;
    size_t total = navigation.summary.total_matches;
    navigation.page_count = total == 0U ? 0U : 1U + (total - 1U) / navigation.query.limit;
    size_t last = total == 0U ? 0U : (navigation.page_count - 1U) * navigation.query.limit;
    if (navigation.query.offset > last) {
        UmiDiagnosticPageDestroy(page); page = NULL;
        navigation.query.offset = last;
        status = UmiDiagnosticPageCreate(model, &navigation.query, &page);
        if (status != UMI_STATUS_OK) goto failed;
        status = UmiDiagnosticPageReadSummary(page, &navigation.summary);
        if (status != UMI_STATUS_OK) goto failed;
    }
    if (revision != umi_diagnostic_model_revision(model) || navigation.summary.revision != revision) {
        status = UMI_STATUS_INVALID_STATE; goto failed;
    }
    navigation.page_index = navigation.query.offset / navigation.query.limit;
    navigation.has_previous = navigation.query.offset != 0U;
    navigation.has_next = navigation.summary.has_more;
    navigation.previous_offset = navigation.has_previous ?
        navigation.query.offset - navigation.query.limit : navigation.query.offset;
    navigation.next_offset = navigation.has_next ?
        navigation.query.offset + navigation.summary.count : navigation.query.offset;
    status = UmiDiagnosticPageValidate(page, model, &navigation.query);
    if (status != UMI_STATUS_OK) goto failed;
    *out_page = page; *out_navigation = navigation;
    return UMI_STATUS_OK;
failed:
    UmiDiagnosticPageDestroy(page);
    return status;
}
