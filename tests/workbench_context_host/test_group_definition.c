/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_group_definition.c
 *
 * PURPOSE:
 *   Verify group-definition initialization and validation.
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
#include "test_support.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_host/group_definition.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextHostGroupDefinitionTransferEqual(const UmiWorkbenchContextHostGroupDefinition *a, const UmiWorkbenchContextHostGroupDefinition *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->group_id, b->group_id) == 0 &&
        strcmp(a->title, b->title) == 0 &&
        a->colour == b->colour &&
        a->allowed_kinds_mask == b->allowed_kinds_mask &&
        a->default_mode == b->default_mode &&
        a->default_active == b->default_active &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiWorkbenchContextHostGroupDefinitionTransferTails(UmiWorkbenchContextHostGroupDefinition *value)
{
    (void)value;
    {
        size_t used = strlen(value->group_id) + 1U;
        memset(value->group_id + used, 0xa5, sizeof(value->group_id) - used);
    }
    {
        size_t used = strlen(value->title) + 1U;
        memset(value->title + used, 0xa5, sizeof(value->title) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextHostGroupDefinitionTransferMalformed(const UmiWorkbenchContextHostGroupDefinition *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextHostGroupDefinition invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.group_id, 'x', sizeof(invalid.group_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_host_group_definition_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_host_group_definition_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated group_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextHostGroupDefinition invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.title, 'x', sizeof(invalid.title));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_host_group_definition_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_host_group_definition_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated title was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextHostGroupDefinitionTransferCases, UmiWorkbenchContextHostGroupDefinition,
    umi_workbench_context_host_group_definition_archive_encode, umi_workbench_context_host_group_definition_archive_decode,
    UmiWorkbenchContextHostGroupDefinitionTransferEqual, UmiWorkbenchContextHostGroupDefinitionTransferTails, UmiWorkbenchContextHostGroupDefinitionTransferMalformed)

int main(void)
{

    UmiWorkbenchContextHostGroupDefinition group;
    umi_workbench_context_host_group_definition_init(&group, "blue");
    assert(umi_workbench_context_host_copy_text(
        group.title, sizeof(group.title), "Development") == UMI_STATUS_OK);
    group.colour = UMI_CONTEXT_COLOUR_BLUE;
    group.allowed_kinds_mask =
        umi_workbench_context_host_kind_mask(UMI_CONTEXT_KIND_PROJECT);
    group.default_active = true;
    assert(umi_workbench_context_host_group_definition_validate(
        &group) == UMI_STATUS_OK);
    if (UmiWorkbenchContextHostGroupDefinitionTransferCases(&group) != 0) return 1;

    group.group_id[0] = '\0';
    assert(umi_workbench_context_host_group_definition_validate(
        &group) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
