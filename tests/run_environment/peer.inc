/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/run_environment/peer.inc
 * PURPOSE: Verify debugger child overrides before the controlled DAP peer accepts launch.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* This peer checks transport and inheritance only. It never debugs a real
 * program, so passing this fixture cannot qualify a user's installed adapter. */
static int CheckLaunchEnvironment(const char *json)
{
    if (strstr(json, "--umicom-environment-check") == NULL)
        return 1;
    const char *value = getenv("UMICOM_RUN_VALUE");
    if (value == NULL || strcmp(value, "debugger inherits selected") != 0)
        return 0;
    UmiLanguageRuntimeJsonDocument *document = malloc(sizeof *document);
    if (document == NULL)
        return 0;
    int valid = umi_language_runtime_json_parse(json, document) == UMI_STATUS_OK;
    int launch = valid ? umi_language_runtime_json_object_get(document, 0, "arguments") : -1;
    /* A partial env object would discard inherited variables under GDB. */
    valid = launch >= 0 && umi_language_runtime_json_object_get(document, launch, "env") < 0;
    free(document);
    return valid;
}
