/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_source/definition.c
 *
 * PURPOSE:
 *   Implement live source definition configuration, validation and sample acceptance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_source/definition.h"
#include "../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise workbench context source definition from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_definition_init(
    UmiWorkbenchContextSourceDefinition *definition,
    const char *source_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL) return;
    memset(definition, 0, sizeof(*definition));
    definition->structure_size = (uint32_t)sizeof(*definition);
    definition->source_kind = UMI_WORKBENCH_CONTEXT_SOURCE_GENERIC;
    definition->trigger = UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_SELECT;
    definition->context_kind = UMI_CONTEXT_KIND_SELECTION;
    definition->coalescing_mode =
        UMI_WORKBENCH_CONTEXT_EVENT_COALESCE_BY_SUBJECT;
    definition->coalescing_window_ms = 40U;
    definition->minimum_interval_ms = 0U;
    definition->accepted_kinds_mask =
        UMI_WORKBENCH_CONTEXT_SOURCE_ALL_KINDS_MASK;
    definition->enabled = true;
    definition->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source_id != NULL) {
        (void)umi_workbench_context_source_copy_text(
            definition->source_id,
            sizeof(definition->source_id),
            source_id);
    }
}

/*
 * Provide the workbench context source definition set identity operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_definition_set_identity(
    UmiWorkbenchContextSourceDefinition *definition,
    const char *application_id,
    const char *panel_id,
    const char *display_name)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL || application_id == NULL ||
        panel_id == NULL || display_name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_context_source_copy_text(
        definition->application_id,
        sizeof(definition->application_id),
        application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_source_copy_text(
        definition->panel_id,
        sizeof(definition->panel_id),
        panel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_source_copy_text(
        definition->display_name,
        sizeof(definition->display_name),
        display_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++definition->revision;
    return status;
}

/*
 * Provide the workbench context source definition set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_definition_set_group(
    UmiWorkbenchContextSourceDefinition *definition,
    const char *group_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL || group_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_context_source_copy_text(
        definition->preferred_group_id,
        sizeof(definition->preferred_group_id),
        group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++definition->revision;
    return status;
}

/*
 * Check that workbench context source definition satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_definition_validate(
    const UmiWorkbenchContextSourceDefinition *definition)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (definition == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(definition->source_id, '\0', sizeof(definition->source_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(definition->application_id, '\0', sizeof(definition->application_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(definition->panel_id, '\0', sizeof(definition->panel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(definition->display_name, '\0', sizeof(definition->display_name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(definition->preferred_group_id, '\0', sizeof(definition->preferred_group_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL ||
        definition->structure_size != sizeof(*definition) ||
        definition->source_id[0] == '\0' ||
        definition->application_id[0] == '\0' ||
        definition->panel_id[0] == '\0' ||
        definition->source_kind < UMI_WORKBENCH_CONTEXT_SOURCE_GENERIC ||
        definition->source_kind > UMI_WORKBENCH_CONTEXT_SOURCE_MEDIA ||
        definition->trigger < UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_ACTIVATE ||
        definition->trigger > UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_NAVIGATE ||
        definition->context_kind < UMI_CONTEXT_KIND_GENERIC ||
        definition->context_kind > UMI_CONTEXT_KIND_SELECTION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context source definition accepts operation used by this module
 * and its client applications.
 */
bool umi_workbench_context_source_definition_accepts(
    const UmiWorkbenchContextSourceDefinition *definition,
    const UmiWorkbenchContextSourceSample *sample)
{
    uint64_t bit;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL || sample == NULL || !definition->enabled ||
        sample->context_kind < UMI_CONTEXT_KIND_GENERIC ||
        sample->context_kind > UMI_CONTEXT_KIND_SELECTION) {
        return false;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(definition->source_id, sample->source_id) != 0 ||
        strcmp(definition->application_id, sample->application_id) != 0 ||
        strcmp(definition->panel_id, sample->panel_id) != 0) {
        return false;
    }
    bit = UINT64_C(1) << ((unsigned)sample->context_kind - 1U);
    return (definition->accepted_kinds_mask & bit) != 0U;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiWorkbenchContextSourceDefinitionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x561a6035a17c5a92);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextSourceDefinition *)0)->source_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextSourceDefinition *)0)->application_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextSourceDefinition *)0)->panel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextSourceDefinition *)0)->display_name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextSourceDefinition *)0)->preferred_group_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiWorkbenchContextSourceDefinitionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiWorkbenchContextSourceDefinition *)0)->source_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextSourceDefinition *)0)->application_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextSourceDefinition *)0)->panel_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextSourceDefinition *)0)->display_name) - 1U +
        8U + sizeof(((UmiWorkbenchContextSourceDefinition *)0)->preferred_group_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiWorkbenchContextSourceDefinitionArchiveWrite(UmiArchiveWriter *writer, const UmiWorkbenchContextSourceDefinition *value)
{
    UmiArchiveWriteText(writer, value->source_id, sizeof(value->source_id));
    UmiArchiveWriteText(writer, value->application_id, sizeof(value->application_id));
    UmiArchiveWriteText(writer, value->panel_id, sizeof(value->panel_id));
    UmiArchiveWriteText(writer, value->display_name, sizeof(value->display_name));
    UmiArchiveWriteText(writer, value->preferred_group_id, sizeof(value->preferred_group_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->source_kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->trigger);
    UmiArchiveWriteSigned(writer, (int64_t)value->context_kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->coalescing_mode);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->coalescing_window_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->minimum_interval_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->accepted_kinds_mask);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiWorkbenchContextSourceDefinitionArchiveRead(UmiArchiveReader *reader, UmiWorkbenchContextSourceDefinition *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->source_id, sizeof(value->source_id));
    UmiArchiveReadText(reader, value->application_id, sizeof(value->application_id));
    UmiArchiveReadText(reader, value->panel_id, sizeof(value->panel_id));
    UmiArchiveReadText(reader, value->display_name, sizeof(value->display_name));
    UmiArchiveReadText(reader, value->preferred_group_id, sizeof(value->preferred_group_id));
    value->source_kind = (UmiWorkbenchContextSourceKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->trigger = (UmiWorkbenchContextSourceTrigger)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->context_kind = (UmiContextKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->coalescing_mode = (UmiWorkbenchContextEventCoalescingMode)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->coalescing_window_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->minimum_interval_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->accepted_kinds_mask = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiWorkbenchContextSourceDefinitionArchiveValidate(const UmiWorkbenchContextSourceDefinition *value)
{
    return umi_workbench_context_source_definition_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_workbench_context_source_definition_archive_encode, umi_workbench_context_source_definition_archive_decode,
    UmiWorkbenchContextSourceDefinition, UmiWorkbenchContextSourceDefinitionArchiveSchema, UmiWorkbenchContextSourceDefinitionArchiveBound, UmiWorkbenchContextSourceDefinitionArchiveWrite, UmiWorkbenchContextSourceDefinitionArchiveRead, UmiWorkbenchContextSourceDefinitionArchiveValidate)
