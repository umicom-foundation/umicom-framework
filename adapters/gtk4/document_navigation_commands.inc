/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_navigation_commands.inc
 *
 * PURPOSE:
 *   Bind source navigation command identifiers to existing inspection reviews.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Fixed identifiers belong to the shared document host. Unknown commands
 * remain unavailable instead of opening a different tool by a prefix match. */
static int NavigationReviewCommandKind(const char *command_id)
{
    if (command_id == NULL) return 0;
    if (strcmp(command_id, "navigate.symbol") == 0) return 1;
    if (strcmp(command_id, "navigate.definition") == 0) return 2;
    if (strcmp(command_id, "navigate.references") == 0) return 3;
    return 0;
}

int UmiGtk4AdapterSourceNavigationCommandKnown(const char *command_id)
{
    return NavigationReviewCommandKind(command_id) != 0;
}

int UmiGtk4AdapterSourceNavigationCommandEnabled(
    UmiGtk4Adapter *adapter, const char *command_id)
{
    if (!UmiGtk4AdapterSourceNavigationCommandKnown(command_id)) return 0;
    UmiDocumentSourceRequest *source = NULL;
    UmiDocumentSourceRequestSummary summary;
    UmiStatus status = CompletionReviewCapture(adapter, 1, 0, &source, &summary);
    UmiDocumentSourceRequestDestroy(source);
    return status == UMI_STATUS_OK;
}

UmiStatus UmiGtk4AdapterSourceNavigationCommand(
    UmiGtk4Adapter *adapter, const char *command_id, const char *workspace_folder,
    const char *application_directory, const char *base_override)
{
    int kind = NavigationReviewCommandKind(command_id);
    if (kind == 0 || application_directory == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Each established entry repeats capture and admission. The earlier
     * availability result is never treated as authority for a later action. */
    if (kind == 1)
        return UmiGtk4AdapterReviewDocumentSymbolsWithSettings(
            adapter, workspace_folder, application_directory, base_override);
    return UmiGtk4AdapterReviewSourceNavigationWithSettings(
        adapter, workspace_folder, kind == 2 ? UMI_LANGUAGE_NAVIGATION_DEFINITION
                                            : UMI_LANGUAGE_NAVIGATION_REFERENCES,
        application_directory, base_override);
}
