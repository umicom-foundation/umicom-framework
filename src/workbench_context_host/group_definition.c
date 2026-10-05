/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/group_definition.c
 *
 * PURPOSE:
 *   Implement product-composition group-definition initialisation and validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/group_definition.h"
#include "../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise workbench context host group definition from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_group_definition_init(
    UmiWorkbenchContextHostGroupDefinition *definition,
    const char *group_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL) return;
    memset(definition, 0, sizeof(*definition));
    definition->structure_size = (uint32_t)sizeof(*definition);
    definition->colour = UMI_CONTEXT_COLOUR_NONE;
    definition->allowed_kinds_mask = UMI_WORKBENCH_CONTEXT_LINK_ALL_KINDS_MASK;
    definition->default_mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    definition->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (group_id != NULL) {
        (void)umi_workbench_context_host_copy_text(
            definition->group_id, sizeof(definition->group_id), group_id);
    }
}

/*
 * Check that workbench context host group definition satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_host_group_definition_validate(
    const UmiWorkbenchContextHostGroupDefinition *definition)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (definition == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(definition->group_id, '\0', sizeof(definition->group_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(definition->title, '\0', sizeof(definition->title)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL || definition->structure_size != sizeof(*definition)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_context_host_text_is_valid(
            definition->group_id, sizeof(definition->group_id)) ||
        definition->group_id[0] == '\0' ||
        !umi_workbench_context_host_text_is_valid(
            definition->title, sizeof(definition->title))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (definition->colour < UMI_CONTEXT_COLOUR_NONE ||
        definition->colour > UMI_CONTEXT_COLOUR_MAGENTA) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (definition->default_mode < UMI_WORKBENCH_CONTEXT_LINK_MODE_NONE ||
        definition->default_mode > UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiWorkbenchContextHostGroupDefinitionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd5fed53ccd8853b3);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextHostGroupDefinition *)0)->group_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextHostGroupDefinition *)0)->title)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiWorkbenchContextHostGroupDefinitionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiWorkbenchContextHostGroupDefinition *)0)->group_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextHostGroupDefinition *)0)->title) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiWorkbenchContextHostGroupDefinitionArchiveWrite(UmiArchiveWriter *writer, const UmiWorkbenchContextHostGroupDefinition *value)
{
    UmiArchiveWriteText(writer, value->group_id, sizeof(value->group_id));
    UmiArchiveWriteText(writer, value->title, sizeof(value->title));
    UmiArchiveWriteSigned(writer, (int64_t)value->colour);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->allowed_kinds_mask);
    UmiArchiveWriteSigned(writer, (int64_t)value->default_mode);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->default_active);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiWorkbenchContextHostGroupDefinitionArchiveRead(UmiArchiveReader *reader, UmiWorkbenchContextHostGroupDefinition *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->group_id, sizeof(value->group_id));
    UmiArchiveReadText(reader, value->title, sizeof(value->title));
    value->colour = (UmiContextChannelColour)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->allowed_kinds_mask = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->default_mode = (UmiWorkbenchContextLinkMode)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->default_active = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiWorkbenchContextHostGroupDefinitionArchiveValidate(const UmiWorkbenchContextHostGroupDefinition *value)
{
    return umi_workbench_context_host_group_definition_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_workbench_context_host_group_definition_archive_encode, umi_workbench_context_host_group_definition_archive_decode,
    UmiWorkbenchContextHostGroupDefinition, UmiWorkbenchContextHostGroupDefinitionArchiveSchema, UmiWorkbenchContextHostGroupDefinitionArchiveBound, UmiWorkbenchContextHostGroupDefinitionArchiveWrite, UmiWorkbenchContextHostGroupDefinitionArchiveRead, UmiWorkbenchContextHostGroupDefinitionArchiveValidate)
