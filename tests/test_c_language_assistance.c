/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_c_language_assistance.c
 *
 * PURPOSE:
 *   Verify the offline C provider filters prefixes, distinguishes include
 *   directives, protects replacement ranges and seeds quick documentation.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/providers/c_language_assistance.h"

typedef struct Capture {
    UmiEditorCompletionCandidate items[8];
    size_t count;
} Capture;

/* Copy emitted values into bounded test storage so no provider-owned pointer escapes. */
static UmiStatus capture_candidate(
    const UmiEditorCompletionCandidate *candidate,
    void *user_data)
{
    Capture *capture = (Capture *)user_data;

    if (candidate == NULL || capture == NULL ||
        capture->count >= sizeof(capture->items) / sizeof(capture->items[0])) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    capture->items[capture->count++] = *candidate;
    return UMI_STATUS_OK;
}

/* Exercise the public provider through the same registry used by editor clients. */
int main(void)
{
    UmiEditorCompletionProviderRegistry *registry = NULL;
    UmiEditorQuickDocumentation *documentation = NULL;
    UmiEditorSignatureHelpModel *signatures = NULL;
    UmiEditorCompletionProviderRegistrySnapshot registry_snapshot;
    UmiEditorQuickDocumentationSnapshot documentation_snapshot;
    UmiEditorSignatureHelpSnapshot signature_snapshot;
    UmiEditorCompletionProviderResponse response;
    UmiEditorCompletionRequest request;
    UmiEditorQuickDocumentationSection section;
    Capture capture = {0};

    assert(umi_editor_c_language_assistance_count() >= 20U);
    assert(umi_editor_c_language_assistance_find("c.function.snprintf") != NULL);
    assert(umi_editor_c_language_assistance_at(
               umi_editor_c_language_assistance_count()) == NULL);

    assert(umi_editor_completion_provider_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_editor_c_language_assistance_register(registry) == UMI_STATUS_OK);
    assert(umi_editor_completion_provider_registry_snapshot(
               registry, &registry_snapshot) == UMI_STATUS_OK);
    assert(registry_snapshot.provider_count == 2U);

    request = umi_editor_completion_request_default("main.c", "c", 12U);
    (void)memcpy(request.prefix, "snpr", sizeof("snpr"));
    request.cursor_offset = 14U;
    request.document_revision = 3U;
    assert(umi_editor_completion_provider_registry_invoke(
               registry, UMI_EDITOR_C_COMPLETION_PROVIDER_ID, &request,
               capture_candidate, &capture, &response) == UMI_STATUS_OK);
    assert(response.emitted_candidate_count == 1U);
    assert(capture.count == 1U);
    assert(strcmp(capture.items[0].item.label, "snprintf") == 0);
    assert(capture.items[0].replace_start_offset == 10U);
    assert(capture.items[0].insert_format ==
           UMI_EDITOR_COMPLETION_INSERT_SNIPPET);
    assert(strstr(capture.items[0].documentation, "truncation") != NULL);

    /* An include directive must offer headers instead of ordinary C keywords. */
    (void)memset(&capture, 0, sizeof(capture));
    request = umi_editor_completion_request_default("main.c", "c", 13U);
    (void)memcpy(request.prefix, "std", sizeof("std"));
    (void)memcpy(request.line_prefix, "# include <std",
                 sizeof("# include <std"));
    request.cursor_offset = 13U;
    assert(umi_editor_completion_provider_registry_invoke(
               registry, UMI_EDITOR_C_COMPLETION_PROVIDER_ID, &request,
               capture_candidate, &capture, &response) == UMI_STATUS_OK);
    assert(capture.count == 4U);
    assert(strcmp(capture.items[0].item.kind, "header") == 0);

    assert(umi_editor_quick_documentation_create(&documentation) ==
           UMI_STATUS_OK);
    assert(umi_editor_c_language_assistance_populate_documentation(
               documentation) == UMI_STATUS_OK);
    assert(umi_editor_quick_documentation_snapshot(
               documentation, &documentation_snapshot) == UMI_STATUS_OK);
    assert(documentation_snapshot.section_count ==
           umi_editor_c_language_assistance_count());
    assert(umi_editor_quick_documentation_select_symbol(
               documentation, "snprintf") == UMI_STATUS_OK);
    assert(umi_editor_quick_documentation_selected_at(
               documentation, 0U, &section) == UMI_STATUS_OK);
    assert(section.trusted == 1);
    assert(strcmp(section.language_id, "c") == 0);

    /* Function entries also become offline signature help with parameter data. */
    assert(umi_editor_signature_help_model_create(&signatures) ==
           UMI_STATUS_OK);
    assert(umi_editor_c_language_assistance_populate_signatures(signatures) ==
           UMI_STATUS_OK);
    assert(umi_editor_signature_help_model_snapshot(
               signatures, &signature_snapshot) == UMI_STATUS_OK);
    assert(signature_snapshot.signature_count == 10U);
    assert(signature_snapshot.visible_signature_count == 10U);

    umi_editor_signature_help_model_destroy(signatures);
    umi_editor_quick_documentation_destroy(documentation);
    umi_editor_completion_provider_registry_destroy(registry);
    return 0;
}
