/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_editor_presentation.c
 *
 * PURPOSE:
 *   Verify professional editor presentation defaults and language resolution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/presentation.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorPresentationRegistry *registry = NULL;
    UmiEditorPresentationSnapshot presentation;
    char reason[192U];

    assert(umi_editor_presentation_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_editor_presentation_registry_seed_defaults(registry) ==
           UMI_STATUS_OK);
    assert(umi_editor_presentation_registry_count(registry) == 12U);

    assert(umi_editor_presentation_default(
               "c++", &presentation) == UMI_STATUS_OK);
    assert(strcmp(presentation.profile_id, "editor.cpp") == 0);
    assert(umi_editor_presentation_default(
               "md", &presentation) == UMI_STATUS_OK);
    assert(presentation.word_wrap == 1);
    assert(presentation.tab_width == 2U);

    assert(umi_editor_presentation_registry_resolve(
               registry, "c", &presentation) == UMI_STATUS_OK);
    assert(strcmp(presentation.profile_id, "editor.c") == 0);
    assert(presentation.tab_width == 4U);
    assert(presentation.highlight_current_line == 1);
    assert(presentation.code_folding == 1);

    assert(umi_editor_presentation_init(
               &presentation, "workspace.c", "c") == UMI_STATUS_OK);
    presentation.tab_width = 2U;
    presentation.indent_width = 2U;
    assert(umi_editor_presentation_registry_upsert(
               registry, &presentation) == UMI_STATUS_OK);
    assert(umi_editor_presentation_registry_resolve(
               registry, "c", &presentation) == UMI_STATUS_OK);
    assert(strcmp(presentation.profile_id, "workspace.c") == 0);
    assert(presentation.tab_width == 2U);

    assert(umi_editor_presentation_registry_resolve(
               registry, "markdown", &presentation) == UMI_STATUS_OK);
    assert(presentation.word_wrap == 1);
    assert(presentation.tab_width == 2U);

    assert(umi_editor_presentation_registry_resolve(
               registry, "future-language", &presentation) == UMI_STATUS_OK);
    assert(strcmp(presentation.profile_id, "editor.default") == 0);
    assert(umi_editor_presentation_validate(
               &presentation, reason, sizeof(reason)) == UMI_STATUS_OK);

    presentation.tab_width = 0U;
    assert(umi_editor_presentation_validate(
               &presentation, reason, sizeof(reason)) ==
           UMI_STATUS_INVALID_STATE);
    assert(strstr(reason, "dimensions") != NULL);

    umi_editor_presentation_registry_destroy(registry);
    return EXIT_SUCCESS;
}
