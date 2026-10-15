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
#include "value_archive/construction_cases.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted presentation value. */
static int UmiEditorPresentationSnapshotConstructionCases(void)
{
    UmiEditorPresentationSnapshot *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_editor_presentation_init(value, "workspace.c", "c") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->profile_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_editor_presentation_init(value, oversized, "c") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_editor_presentation_init(value, value->profile_id, "c") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->profile_id, "workspace.c") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->language_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_editor_presentation_init(value, "workspace.c", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_editor_presentation_init(value, "workspace.c", value->language_id) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->language_id, "c") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiEditorPresentationSnapshotConstructionCases() != 0) return 1;

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
