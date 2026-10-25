/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/data_history/record_catalogue.c
 *
 * PURPOSE:
 *   A headless host uses the same catalogue and checked open as native widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A headless host uses the same catalogue and checked open as native widgets. */
#include "umicom/data/record_catalogue.h"
#include "umicom/data/record_document.h"
#include <stdio.h>
int main(void)
{
    UmiDataServer *server = NULL;
    if (umi_data_server_create_memory(&server) != UMI_STATUS_OK) return 1;
    UmiRecordAmendment author = {.entity_id="notes.fictional", .source="SDK example",
        .actor="Example reviewer", .reason="Create an administrative note", .recorded_at_nanoseconds=1U};
    UmiRecordRevision saved;
    UmiStatus status = umi_record_document_append(server, &author, "Example note", 12U, 0U, &saved);
    UmiRecordCatalogueQuery query = {.entity_prefix="notes.", .text="reviewer", .limit=16U};
    UmiRecordCataloguePage page;
    if (status == UMI_STATUS_OK) status = umi_record_catalogue_page(server, &query, &page);
    if (status == UMI_STATUS_OK && page.count != 1U) status = UMI_STATUS_INVALID_STATE;
    UmiRecordDocument document;
    if (status == UMI_STATUS_OK) status = umi_record_document_open_reviewed(server, &page.rows[0], &document);
    if (status == UMI_STATUS_OK) printf("Opened %s: %zu verified content bytes\n", document.record.amendment.entity_id, document.length);
    umi_data_server_destroy(server);
    return status == UMI_STATUS_OK ? 0 : 1;
}
