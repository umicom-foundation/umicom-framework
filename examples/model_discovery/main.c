/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/model_discovery/main.c
 *
 * PURPOSE:
 *   Demonstrate complete model identities and evidence-aware selection using only the public Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/model_discovery.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    /* Synthetic offline observations make this lesson reproducible without
     * accounts, model downloads, background servers or inferred capabilities. */
    const char available[] = "{\"models\":[{\"model\":\"example:small\",\"size\":1048576},{\"model\":\"example:large\",\"size\":2097152}]}";
    const char loaded[] = "{\"models\":[{\"model\":\"example:small\",\"size_vram\":0,\"context_length\":2048}]}";
    UmiAiDiscoverySnapshot *inventory = NULL, *running = NULL;
    UmiAiDiscoverySnapshot *selection = NULL;
    UmiStatus status = UmiAiDiscoveryDecode(UMI_AI_DISCOVERY_OLLAMA_AVAILABLE,
        available, sizeof(available)-1U, NULL, &inventory);
    if (status != UMI_STATUS_OK) goto done;
    status = UmiAiDiscoveryDecode(UMI_AI_DISCOVERY_OLLAMA_LOADED,
        loaded, sizeof(loaded)-1U, NULL, &running);
    if (status != UMI_STATUS_OK) goto done;
    if (UmiAiDiscoveryCount(inventory) != 2 || UmiAiDiscoveryCount(running) != 1) {
        status = UMI_STATUS_INVALID_STATE; goto done;
    }
    for (size_t i = 0; i < UmiAiDiscoveryCount(inventory); ++i) {
        UmiAiDiscoveredModel listed, active;
        status = UmiAiDiscoveryAt(inventory, i, &listed);
        if (status != UMI_STATUS_OK) goto done;
        UmiStatus lookup = UmiAiDiscoveryFind(running, listed.id, &active);
        if (lookup != UMI_STATUS_OK && lookup != UMI_STATUS_NOT_FOUND) { status = lookup; goto done; }
        printf("%s: listed; %s\n", listed.id,
            lookup == UMI_STATUS_OK ? "reported loaded" : "not in loaded observation");
        if (lookup == UMI_STATUS_OK && (active.known & UMI_AI_DISCOVERY_VRAM) != 0U) {
            /* Zero is a reported value here, not an absent estimate. */
            if (active.vram_bytes != 0 || active.context_tokens != 2048) {
                status = UMI_STATUS_INVALID_STATE; goto done;
            }
        }
    }
    /* Selection copies observed data and retains its endpoint format. A size
     * filter describes reported bytes, not whether a model fits this device. */
    UmiAiDiscoveryFilter filter = {0};
    strcpy(filter.search, "EXAMPLE:");
    filter.size_limit_enabled = 1; filter.maximum_size_bytes = 1048576;
    filter.order = UMI_AI_DISCOVERY_ORDER_SIZE_ASC;
    status = UmiAiDiscoverySelect(inventory, &filter, NULL, &selection);
    if (status != UMI_STATUS_OK) goto done;
    UmiAiDiscoveredModel chosen;
    if (UmiAiDiscoveryCount(selection) != 1 ||
        UmiAiDiscoveryAt(selection, 0, &chosen) != UMI_STATUS_OK ||
        strcmp(chosen.id, "example:small") != 0) {
        status = UMI_STATUS_INVALID_STATE; goto done;
    }
    puts("One model matches the reviewed name and reported-size filter.");
    puts("Synthetic observations only; no inference or service readiness was tested.");
done:
    UmiAiDiscoveryDestroy(selection);
    UmiAiDiscoveryDestroy(running);
    UmiAiDiscoveryDestroy(inventory);
    if (status != UMI_STATUS_OK) fprintf(stderr, "Discovery example: %s\n", umi_status_text(status));
    return status == UMI_STATUS_OK ? 0 : 1;
}
