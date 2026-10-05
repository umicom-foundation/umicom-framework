/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/provider_connections/saved_connections.c
 * PURPOSE: Demonstrate reviewed settings without using credentials or contacting a provider.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/provider_connections/connections.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    UmiDataServer *server = NULL;
    UmiProviderConnections *store = NULL;
    UmiProviderConnectionSnapshot *snapshot = calloc(1U, sizeof(*snapshot));
    if (snapshot == NULL) return 1;
    /* This lesson uses memory so it never changes a user's saved settings.
     * A host supplies a SQLite server at an absolute per-user path for reuse
     * after restart. The connection service itself is unchanged. */
    UmiStatus status = umi_data_server_create_memory(&server);
    if (status == UMI_STATUS_OK) status = UmiProviderConnectionsOpen(server, "studio", "learner", &store);
    if (status == UMI_STATUS_OK) status = UmiProviderConnectionsRead(store, snapshot);
    UmiProviderConnection connection = {0};
    strcpy(connection.id, "local-model");
    strcpy(connection.provider_id, "local-chat");
    strcpy(connection.label, "Local model server");
    strcpy(connection.endpoint, "http://127.0.0.1:8080/v1/chat/completions");
    connection.route = UMI_PROVIDER_CONNECTION_LOOPBACK;
    connection.timeout_ms = 30000U;
    /* Saving a disabled connection records a draft. The product should let
     * its user review the chosen model and endpoint before enabling use. */
    uint64_t revision = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiProviderConnectionsPut(store, &connection, false, snapshot->revision, &revision);
    if (status == UMI_STATUS_OK) status = UmiProviderConnectionsRead(store, snapshot);
    if (status == UMI_STATUS_OK && snapshot->count != 1U) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        printf("Saved draft: %s (disabled)\n", snapshot->items[0].label);
    else fprintf(stderr, "Settings operation failed: %s\n", umi_status_text(status));
    UmiProviderConnectionsDestroy(store);
    umi_data_server_destroy(server);
    free(snapshot);
    return status == UMI_STATUS_OK ? 0 : 1;
}
