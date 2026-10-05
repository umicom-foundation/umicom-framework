/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_source/test_definition.c
 *
 * PURPOSE:
 *   Verify configured sources accept only matching identity and typed context samples.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>

#include "umicom/workbench_context_source/definition.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_source/definition.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextSourceDefinitionTransferEqual(const UmiWorkbenchContextSourceDefinition *a, const UmiWorkbenchContextSourceDefinition *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->source_id, b->source_id) == 0 &&
        strcmp(a->application_id, b->application_id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        strcmp(a->display_name, b->display_name) == 0 &&
        strcmp(a->preferred_group_id, b->preferred_group_id) == 0 &&
        a->source_kind == b->source_kind &&
        a->trigger == b->trigger &&
        a->context_kind == b->context_kind &&
        a->coalescing_mode == b->coalescing_mode &&
        a->coalescing_window_ms == b->coalescing_window_ms &&
        a->minimum_interval_ms == b->minimum_interval_ms &&
        a->accepted_kinds_mask == b->accepted_kinds_mask &&
        a->enabled == b->enabled &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiWorkbenchContextSourceDefinitionTransferTails(UmiWorkbenchContextSourceDefinition *value)
{
    (void)value;
    {
        size_t used = strlen(value->source_id) + 1U;
        memset(value->source_id + used, 0xa5, sizeof(value->source_id) - used);
    }
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
    {
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
    }
    {
        size_t used = strlen(value->display_name) + 1U;
        memset(value->display_name + used, 0xa5, sizeof(value->display_name) - used);
    }
    {
        size_t used = strlen(value->preferred_group_id) + 1U;
        memset(value->preferred_group_id + used, 0xa5, sizeof(value->preferred_group_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextSourceDefinitionTransferMalformed(const UmiWorkbenchContextSourceDefinition *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextSourceDefinition invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_id, 'x', sizeof(invalid.source_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_source_definition_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_source_definition_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextSourceDefinition invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_source_definition_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_source_definition_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextSourceDefinition invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_source_definition_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_source_definition_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextSourceDefinition invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.display_name, 'x', sizeof(invalid.display_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_source_definition_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_source_definition_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated display_name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextSourceDefinition invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.preferred_group_id, 'x', sizeof(invalid.preferred_group_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_source_definition_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_source_definition_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated preferred_group_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextSourceDefinitionTransferCases, UmiWorkbenchContextSourceDefinition,
    umi_workbench_context_source_definition_archive_encode, umi_workbench_context_source_definition_archive_decode,
    UmiWorkbenchContextSourceDefinitionTransferEqual, UmiWorkbenchContextSourceDefinitionTransferTails, UmiWorkbenchContextSourceDefinitionTransferMalformed)

int main(void)
{
    UmiWorkbenchContextSourceDefinition definition;
    UmiWorkbenchContextSourceSample sample;
    umi_workbench_context_source_definition_init(
        &definition, "studio.editor.location");
    assert(umi_workbench_context_source_definition_set_identity(
        &definition, "org.umicom.studio",
        "studio.editor", "Editor") == UMI_STATUS_OK);
    definition.source_kind = UMI_WORKBENCH_CONTEXT_SOURCE_EDITOR;
    definition.trigger = UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_CARET;
    definition.context_kind = UMI_CONTEXT_KIND_SOURCE_LOCATION;
    definition.accepted_kinds_mask =
        UINT64_C(1) << ((unsigned)UMI_CONTEXT_KIND_SOURCE_LOCATION - 1U);
    assert(umi_workbench_context_source_definition_validate(
        &definition) == UMI_STATUS_OK);
    if (UmiWorkbenchContextSourceDefinitionTransferCases(&definition) != 0) return 1;


    umi_workbench_context_source_sample_init(
        &sample,
        UMI_WORKBENCH_CONTEXT_SOURCE_EDITOR,
        UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_CARET,
        UMI_CONTEXT_KIND_SOURCE_LOCATION,
        "sample");
    assert(umi_workbench_context_source_sample_set_identity(
        &sample, "studio.editor.location",
        "org.umicom.studio", "studio.editor",
        "workspace") == UMI_STATUS_OK);
    assert(umi_workbench_context_source_definition_accepts(
        &definition, &sample));

    return 0;
}
