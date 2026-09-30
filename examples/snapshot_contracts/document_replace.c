/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/snapshot_contracts/document_replace.c
 * PURPOSE: Teach complete document replacement without losing other results.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language/diagnostic.h"
#include <stdio.h>
#include <string.h>

/* Fixed lesson strings fit the arrays. Real producers must check copy lengths. */
static UmiLanguageDiagnosticSnapshot diagnostic(const char *id, const char *document)
{
    UmiLanguageDiagnosticSnapshot item = {0};
    (void)snprintf(item.id, sizeof(item.id), "%s", id);
    (void)snprintf(item.document_id, sizeof(item.document_id), "%s", document);
    (void)snprintf(item.message, sizeof(item.message), "%s", "Example diagnostic");
    return item;
}

int main(void)
{
    UmiLanguageDiagnosticRegistry *registry = NULL;
    UmiLanguageDiagnosticSnapshot seed[2] = {
        diagnostic("main-old", "main.c"), diagnostic("helper-old", "helper.c")};
    UmiLanguageDiagnosticSnapshot incoming = diagnostic("main-new", "wrong.c"), retained;
    UmiSnapshotBatchResult result;
    int failed = 1;
    UmiStatus status = umi_language_diagnostic_registry_create(&registry);
    if (status != UMI_STATUS_OK) return 1;
    status = umi_language_diagnostic_registry_upsert_many(registry, seed, 2U, NULL);
    if (status != UMI_STATUS_OK) goto done;
    uint64_t expected = umi_language_diagnostic_registry_revision(registry);
    status = umi_language_diagnostic_registry_replace_document(registry, "main.c", expected, &incoming, 1U, &result);
    if (status != UMI_STATUS_PERMISSION_DENIED || result.applied != 0U ||
        umi_language_diagnostic_registry_revision(registry) != expected) goto done;
    incoming = diagnostic("main-new", "main.c");
    status = umi_language_diagnostic_registry_replace_document(registry, "main.c", expected, &incoming, 1U, &result);
    if (status != UMI_STATUS_OK || result.applied != 1U) goto done;
    expected = umi_language_diagnostic_registry_revision(registry);
    status = umi_language_diagnostic_registry_replace_document(registry, "main.c", expected, NULL, 0U, NULL);
    if (status != UMI_STATUS_OK || umi_language_diagnostic_registry_count(registry) != 1U) goto done;
    status = umi_language_diagnostic_registry_at(registry, 0U, &retained);
    if (status != UMI_STATUS_OK || strcmp(retained.document_id, "helper.c") != 0) goto done;
    puts("Other document retained; selected document cleared.");
    failed = 0;
done:
    umi_language_diagnostic_registry_destroy(registry);
    return failed;
}
