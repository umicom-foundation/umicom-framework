/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/local_models/sdk_consumer/main.c
 *
 * PURPOSE:
 *   Exercise the installed model catalogue and provider linkage without network or model execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/local_models/manager.h"
#include <stdio.h>
int main(void)
{
    const UmiModelCatalogueEntry *entry = UmiModelCatalogueAt(0U);
    if (entry == NULL || UmiModelSourceValidate(&entry->source) != UMI_STATUS_OK) return 1;
    /* An invalid port exercises the exported factory without HTTP traffic.
     * An explicitly offline SDK reports UNAVAILABLE instead of a mock. */
    UmiAiProvider provider = {0};
    UmiStatus status = UmiModelLocalProviderCreate(0U, 100U, NULL, &provider);
    UmiStatus expected = UmiAiWorkspaceLocalProviderAvailable()
        ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_UNAVAILABLE;
    if (status != expected || provider.instance != NULL) return 1;
    (void)printf("%s; native downloads %s; local chat %s\n", entry->id,
        UmiModelDownloadAvailable() ? "available" : "unavailable",
        UmiAiWorkspaceLocalProviderAvailable() ? "available" : "unavailable");
    return 0;
}
