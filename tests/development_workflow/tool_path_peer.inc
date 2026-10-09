/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/development_workflow/tool_path_peer.inc
 * PURPOSE: Verify the tool-folder prefix received by the controlled DAP child.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/process_search_path.h"
/* Other peer modes keep their established replies. This marker asks the child
 * to inspect the PATH it actually received without executing a debuggee. */
static int CheckLaunchToolPath(const char *json)
{
    if (strstr(json, "--umicom-tool-path-check") == NULL)
        return 1;
    UmiLanguageRuntimeJsonDocument *document = malloc(sizeof *document);
    if (document == NULL)
        return 0;
    int valid = umi_language_runtime_json_parse(json, document) == UMI_STATUS_OK;
    int launch = valid ? umi_language_runtime_json_object_get(document, 0, "arguments") : -1;
    int arguments =
        launch >= 0 ? umi_language_runtime_json_object_get(document, launch, "args") : -1;
    char folder[UMI_PATH_CAPACITY];
    if (arguments < 0 || umi_language_runtime_json_array_count(document, arguments) != 2U)
        valid = 0;
    if (valid)
    {
        int item = umi_language_runtime_json_array_at(document, arguments, 1U);
        valid = umi_language_runtime_json_string(document, item, folder, sizeof folder) ==
                UMI_STATUS_OK;
    }
    char *captured = NULL;
    if (valid)
        valid = UmiProcessSearchPathCapture(folder, &captured) == UMI_STATUS_OK;
    if (valid)
    {
        size_t prefix = strlen(folder);
        const char *received = captured + prefix;
        if (*received != '\0')
            ++received;
#ifdef _WIN32
        const char separator = ';';
#else
        const char separator = ':';
#endif
        valid = strncmp(received, folder, prefix) == 0 &&
                (received[prefix] == '\0' || received[prefix] == separator);
    }
    UmiProcessSearchPathFree(captured);
    free(document);
    return valid;
}
