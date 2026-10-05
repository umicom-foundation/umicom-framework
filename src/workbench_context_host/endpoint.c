/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/endpoint.c
 *
 * PURPOSE:
 *   Implement context-aware endpoint validation, identity, grouping and capability checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/endpoint.h"
#include "../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise workbench context host endpoint from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_endpoint_init(
    UmiWorkbenchContextHostEndpoint *endpoint,
    const char *endpoint_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (endpoint == NULL) return;
    memset(endpoint, 0, sizeof(*endpoint));
    endpoint->structure_size = (uint32_t)sizeof(*endpoint);
    endpoint->role = UMI_WORKBENCH_CONTEXT_HOST_PANEL_GENERIC;
    endpoint->state = UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_READY;
    endpoint->mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_NONE;
    endpoint->accepted_kinds_mask = UMI_WORKBENCH_CONTEXT_LINK_ALL_KINDS_MASK;
    endpoint->published_kinds_mask = UMI_WORKBENCH_CONTEXT_LINK_ALL_KINDS_MASK;
    endpoint->enabled = true;
    endpoint->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (endpoint_id != NULL) {
        (void)umi_workbench_context_host_copy_text(
            endpoint->endpoint_id, sizeof(endpoint->endpoint_id), endpoint_id);
    }
}

/*
 * Check that workbench context host endpoint satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_host_endpoint_validate(
    const UmiWorkbenchContextHostEndpoint *endpoint)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (endpoint == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(endpoint->endpoint_id, '\0', sizeof(endpoint->endpoint_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(endpoint->panel_id, '\0', sizeof(endpoint->panel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(endpoint->application_id, '\0', sizeof(endpoint->application_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(endpoint->display_name, '\0', sizeof(endpoint->display_name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(endpoint->group_id, '\0', sizeof(endpoint->group_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (endpoint == NULL || endpoint->structure_size != sizeof(*endpoint)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_context_host_text_is_valid(
            endpoint->endpoint_id, sizeof(endpoint->endpoint_id)) ||
        endpoint->endpoint_id[0] == '\0' ||
        !umi_workbench_context_host_text_is_valid(
            endpoint->panel_id, sizeof(endpoint->panel_id)) ||
        endpoint->panel_id[0] == '\0' ||
        !umi_workbench_context_host_text_is_valid(
            endpoint->application_id, sizeof(endpoint->application_id)) ||
        endpoint->application_id[0] == '\0' ||
        !umi_workbench_context_host_text_is_valid(
            endpoint->display_name, sizeof(endpoint->display_name)) ||
        !umi_workbench_context_host_text_is_valid(
            endpoint->group_id, sizeof(endpoint->group_id))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (endpoint->role < UMI_WORKBENCH_CONTEXT_HOST_PANEL_GENERIC ||
        endpoint->role > UMI_WORKBENCH_CONTEXT_HOST_PANEL_TREASURY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (endpoint->state < UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_DISABLED ||
        endpoint->state > UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_ERROR) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (endpoint->mode < UMI_WORKBENCH_CONTEXT_LINK_MODE_NONE ||
        endpoint->mode > UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context host endpoint set identity operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_host_endpoint_set_identity(
    UmiWorkbenchContextHostEndpoint *endpoint,
    const char *panel_id,
    const char *application_id,
    const char *display_name)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (endpoint == NULL || panel_id == NULL || application_id == NULL ||
        display_name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_context_host_copy_text(
        endpoint->panel_id, sizeof(endpoint->panel_id), panel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_host_copy_text(
        endpoint->application_id, sizeof(endpoint->application_id), application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_host_copy_text(
        endpoint->display_name, sizeof(endpoint->display_name), display_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++endpoint->revision;
    return status;
}

/*
 * Provide the workbench context host endpoint set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_host_endpoint_set_group(
    UmiWorkbenchContextHostEndpoint *endpoint,
    const char *group_id,
    UmiWorkbenchContextLinkMode mode)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (endpoint == NULL || group_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_host_copy_text(
        endpoint->group_id, sizeof(endpoint->group_id), group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    endpoint->mode = mode;
    ++endpoint->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context host endpoint accepts operation used by this module and
 * its client applications.
 */
bool umi_workbench_context_host_endpoint_accepts(
    const UmiWorkbenchContextHostEndpoint *endpoint,
    UmiContextKind kind)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (endpoint == NULL || !endpoint->enabled ||
        endpoint->state == UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_DISABLED ||
        endpoint->state == UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_SUSPENDED) {
        return false;
    }
    return umi_workbench_context_host_kind_allowed(
        endpoint->accepted_kinds_mask, kind);
}

/*
 * Provide the workbench context host endpoint publishes operation used by this module and
 * its client applications.
 */
bool umi_workbench_context_host_endpoint_publishes(
    const UmiWorkbenchContextHostEndpoint *endpoint,
    UmiContextKind kind)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (endpoint == NULL || !endpoint->enabled ||
        endpoint->state == UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_DISABLED ||
        endpoint->state == UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_SUSPENDED) {
        return false;
    }
    return umi_workbench_context_host_kind_allowed(
        endpoint->published_kinds_mask, kind);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiWorkbenchContextHostEndpointArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd39ab0f71dbfc9f1);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextHostEndpoint *)0)->endpoint_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextHostEndpoint *)0)->panel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextHostEndpoint *)0)->application_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextHostEndpoint *)0)->display_name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextHostEndpoint *)0)->group_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiWorkbenchContextHostEndpointArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiWorkbenchContextHostEndpoint *)0)->endpoint_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextHostEndpoint *)0)->panel_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextHostEndpoint *)0)->application_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextHostEndpoint *)0)->display_name) - 1U +
        8U + sizeof(((UmiWorkbenchContextHostEndpoint *)0)->group_id) - 1U +
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
static void UmiWorkbenchContextHostEndpointArchiveWrite(UmiArchiveWriter *writer, const UmiWorkbenchContextHostEndpoint *value)
{
    UmiArchiveWriteText(writer, value->endpoint_id, sizeof(value->endpoint_id));
    UmiArchiveWriteText(writer, value->panel_id, sizeof(value->panel_id));
    UmiArchiveWriteText(writer, value->application_id, sizeof(value->application_id));
    UmiArchiveWriteText(writer, value->display_name, sizeof(value->display_name));
    UmiArchiveWriteText(writer, value->group_id, sizeof(value->group_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->role);
    UmiArchiveWriteSigned(writer, (int64_t)value->state);
    UmiArchiveWriteSigned(writer, (int64_t)value->mode);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->accepted_kinds_mask);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->published_kinds_mask);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->delivery_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->publish_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiWorkbenchContextHostEndpointArchiveRead(UmiArchiveReader *reader, UmiWorkbenchContextHostEndpoint *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->endpoint_id, sizeof(value->endpoint_id));
    UmiArchiveReadText(reader, value->panel_id, sizeof(value->panel_id));
    UmiArchiveReadText(reader, value->application_id, sizeof(value->application_id));
    UmiArchiveReadText(reader, value->display_name, sizeof(value->display_name));
    UmiArchiveReadText(reader, value->group_id, sizeof(value->group_id));
    value->role = (UmiWorkbenchContextHostPanelRole)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->state = (UmiWorkbenchContextHostEndpointState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->mode = (UmiWorkbenchContextLinkMode)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->accepted_kinds_mask = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->published_kinds_mask = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->delivery_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->publish_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiWorkbenchContextHostEndpointArchiveValidate(const UmiWorkbenchContextHostEndpoint *value)
{
    return umi_workbench_context_host_endpoint_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_workbench_context_host_endpoint_archive_encode, umi_workbench_context_host_endpoint_archive_decode,
    UmiWorkbenchContextHostEndpoint, UmiWorkbenchContextHostEndpointArchiveSchema, UmiWorkbenchContextHostEndpointArchiveBound, UmiWorkbenchContextHostEndpointArchiveWrite, UmiWorkbenchContextHostEndpointArchiveRead, UmiWorkbenchContextHostEndpointArchiveValidate)
