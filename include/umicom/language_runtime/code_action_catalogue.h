/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/code_action_catalogue.h
 * PURPOSE: Own complete code-action proposals without executing commands or losing edit metadata.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_CODE_ACTION_CATALOGUE_H
#define UMICOM_LANGUAGE_RUNTIME_CODE_ACTION_CATALOGUE_H
#include "umicom/language_runtime/workspace_edit_catalogue.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageCodeAction
    {
        const char *title, *kind, *disabled_reason, *command;
        int preferred, disabled, has_edit, has_command, has_data;
    } UmiLanguageCodeAction;
    typedef struct UmiLanguageCodeActionCatalogue UmiLanguageCodeActionCatalogue;
    /* Own a null or mixed Command/CodeAction array. Titles and known metadata are
 * validated; unknown kinds are retained for ordinary presentation. Preserve
 * each complete original item, including command arguments, diagnostics and
 * opaque data, in owned JSON. Diagnostics are not interpreted by this reader.
 * No command, URL, edit, resource operation or deferred resolve is executed.
 *
 * Limits: 1 MiB JSON, 512 actions, 65536 bytes per title/disabled reason and
 * 4096 bytes per kind/command identifier. Edit objects are retained and decoded
 * only when selected, avoiding an allocation per unselected workspace edit.
 * Failure clears out_catalogue; string views live until Destroy. */
    UmiStatus UmiLanguageCodeActionCatalogueCreate(const void *json, size_t bytes,
                                                   const UmiCancellationToken *cancel,
                                                   UmiLanguageCodeActionCatalogue **out_catalogue);
    UmiStatus UmiLanguageCodeActionCatalogueReadResponse(const void *json, size_t bytes,
                                                         uint64_t expected_request_id,
                                                         const UmiCancellationToken *cancel,
                                                         UmiLanguageCodeActionCatalogue **out_catalogue);
    void UmiLanguageCodeActionCatalogueDestroy(UmiLanguageCodeActionCatalogue *catalogue);
    size_t UmiLanguageCodeActionCatalogueCount(const UmiLanguageCodeActionCatalogue *catalogue);
    UmiStatus UmiLanguageCodeActionCatalogueAt(const UmiLanguageCodeActionCatalogue *catalogue, size_t index,
                                               UmiLanguageCodeAction *out_action);
    /* Borrow the original item's JSON span, not necessarily NUL-terminated. Its
 * presence is not permission to run a command or resolve an action. Outputs
 * clear on error; the span lives until catalogue destruction. */
    UmiStatus UmiLanguageCodeActionCatalogueItemJson(const UmiLanguageCodeActionCatalogue *catalogue,
                                                     size_t index, const char **out_json, size_t *out_bytes);
    /* Return a separately owned, fully decoded edit for review. Disabled actions
 * return PERMISSION_DENIED. Any required command, or an action without an edit,
 * returns NOT_IMPLEMENTED. In particular, edit-plus-command is not projected
 * into a partial edit-only operation. Workspace resource operations and other
 * unsupported edit forms are refused by the shared workspace reader.
 *
 * Success does not approve, apply or save the edit. Resolve all document
 * versions and exact source ranges, display annotations, collect confirmations
 * and use the document owner to apply the reviewed result. Failure clears out. */
    UmiStatus UmiLanguageCodeActionCatalogueReadEdits(const UmiLanguageCodeActionCatalogue *catalogue,
                                                      size_t index, const UmiCancellationToken *cancel,
                                                      UmiLanguageWorkspaceEditCatalogue **out_edits);
    /* Standard action families used by explicit editor requests. ALL preserves
 * every action, including command-only and unclassified rows for inspection. */
    typedef enum UmiLanguageCodeActionFilter
    {
        UMI_LANGUAGE_ACTION_FILTER_ALL,
        UMI_LANGUAGE_ACTION_FILTER_QUICK_FIX,
        UMI_LANGUAGE_ACTION_FILTER_REFACTOR,
        UMI_LANGUAGE_ACTION_FILTER_ORGANIZE_IMPORTS,
        UMI_LANGUAGE_ACTION_FILTER_FIX_ALL
    } UmiLanguageCodeActionFilter;
    /* Borrow a static protocol kind. ALL yields an empty string. Unknown values
 * return INVALID_ARGUMENT and clear the output pointer. */
    UmiStatus UmiLanguageCodeActionFilterName(UmiLanguageCodeActionFilter filter, const char **out_kind);
    /* Create an independent catalogue of matching complete actions, in source
 * order. A family matches itself and dot-delimited descendants, never a plain
 * prefix such as refactorOther. Missing/empty kinds match only ALL. Preserve
 * disabled reasons, commands, diagnostics and opaque data without executing
 * them. The source owner and its borrowed views are unchanged. Existing
 * catalogue limits apply; cancellation or failure publishes no partial list. */
    UmiStatus UmiLanguageCodeActionCatalogueFilter(const UmiLanguageCodeActionCatalogue *catalogue,
                                                   UmiLanguageCodeActionFilter filter,
                                                   const UmiCancellationToken *cancel,
                                                   UmiLanguageCodeActionCatalogue **out_catalogue);
    /* Replace one enabled deferred text-action row with a correlated resolve reply
 * in an independently owned catalogue. The source catalogue is unchanged.
 * The reply must add an edit object and preserve every other original field,
 * including unknown metadata, diagnostics and opaque data. Object order and
 * escaped string spelling may differ; numeric spelling must remain identical.
 * Added, omitted, duplicate or changed immutable fields are refused.
 *
 * Disabled rows return PERMISSION_DENIED; rows requiring commands return
 * NOT_IMPLEMENTED; rows already carrying an edit return INVALID_STATE.
 * Resolution also bounds action fields to 256 and field names to 4096 UTF-8
 * bytes. Existing catalogue bounds apply to the complete replacement catalogue.
 * No process, command, edit or save is performed. Decode the selected edit
 * separately and pass it through complete source review. Failure clears out. */
    UmiStatus UmiLanguageCodeActionCatalogueWithResolvedResponse(
        const UmiLanguageCodeActionCatalogue *catalogue, size_t index, const void *json, size_t bytes,
        uint64_t expected_request_id, const UmiCancellationToken *cancel,
        UmiLanguageCodeActionCatalogue **out_catalogue);
#ifdef __cplusplus
}
#endif
#endif
