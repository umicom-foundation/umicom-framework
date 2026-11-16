/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/comfy_nodes/numeric.c
 *
 * PURPOSE:
 *   Inspect typed numeric node metadata through an installed Framework SDK without executing a custom node.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_numeric_input.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    UmiProviderConnection connection = {0};
    strcpy(connection.id, "numeric-node-example");
    strcpy(connection.provider_id, "comfyui");
    strcpy(connection.label, "Captured node example");
    strcpy(connection.endpoint, "http://127.0.0.1:8188");
    connection.route = UMI_PROVIDER_CONNECTION_LOOPBACK;
    connection.timeout_ms = 30000;
    connection.enabled = true;
    const char response[] =
        "{\"ExampleSampler\":{\"input\":{\"required\":{"
        "\"seed\":[\"INT\",{\"default\":9007199254740993,\"min\":0,\"max\":9223372036854775807}],"
        "\"guidance\":[\"FLOAT\",{\"default\":7.5,\"min\":0,\"max\":30.0,\"step\":0.5}],"
        "\"custom\":[\"MY_TENSOR\",{\"forceInput\":true}]"
        "}},\"output\":[\"IMAGE\"]}}";
    UmiComfyNodeCatalogue *catalogue = NULL;
    UmiStatus status = UmiComfyNodeCatalogueCreate(&connection, 1U, response,
        sizeof(response) - 1U, NULL, &catalogue);
    UmiComfyNumericInput seed = {0}, guidance = {0}, custom = {0};
    size_t index = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiComfyNodeCatalogueFind(catalogue, "ExampleSampler", &index);
    if (status == UMI_STATUS_OK)
        status = UmiComfyNodeCatalogueNumericInput(catalogue, index, UMI_COMFY_INPUT_REQUIRED, 0U, &seed);
    if (status == UMI_STATUS_OK)
        status = UmiComfyNodeCatalogueNumericInput(catalogue, index, UMI_COMFY_INPUT_REQUIRED, 1U, &guidance);
    bool unsupported = status == UMI_STATUS_OK &&
        UmiComfyNodeCatalogueNumericInput(catalogue, index, UMI_COMFY_INPUT_REQUIRED, 2U, &custom)
            == UMI_STATUS_NOT_IMPLEMENTED;
    UmiComfyNodeInput raw = {0};
    bool raw_available = unsupported &&
        UmiComfyNodeCatalogueInputAt(catalogue, index, UMI_COMFY_INPUT_REQUIRED, 2U, &raw) == UMI_STATUS_OK &&
        raw.schema_bytes > 0U;
    /* Reports own their copied values, while raw.schema_json remains borrowed.
     * Destroy the catalogue only after the last borrowed-span use. */
    UmiComfyNodeCatalogueDestroy(catalogue);
    if (status != UMI_STATUS_OK || !raw_available ||
        seed.kind != UMI_COMFY_NUMBER_INTEGER || seed.known_values != 7U ||
        seed.values.integers[UMI_COMFY_NUMBER_DEFAULT] != INT64_C(9007199254740993) ||
        seed.inconsistent_values || guidance.kind != UMI_COMFY_NUMBER_DECIMAL ||
        guidance.known_values != 15U || guidance.values.decimals[UMI_COMFY_NUMBER_DEFAULT] != 7.5 ||
        guidance.values.decimals[UMI_COMFY_NUMBER_STEP] != 0.5 || guidance.inconsistent_values)
        return 1;
    if (printf("Observed exact seed default: %" PRId64 "\n",
        seed.values.integers[UMI_COMFY_NUMBER_DEFAULT]) < 0 ||
        puts("Numeric constraints copied; custom schema retained; no generation requested.") == EOF)
        return 1;
    return fflush(stdout) == 0 ? 0 : 1;
}
