/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/tool_registry.c
 *
 * PURPOSE:
 *   Register and resolve AI tools by identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The fixed registry keeps tool discovery simple and deterministic for the first stable C ABI.
 */

#include "umicom/ai/tool_registry.h"
#include <stddef.h>

#include <string.h>

/*
 * Initialise ai tool registry from caller-provided values so later operations receive a
 * known state.
 */
void umi_ai_tool_registry_init(UmiAiToolRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry != NULL) {
        (void)memset(registry, 0, sizeof(*registry));
    }
}

/* Add ai tool registry only after its inputs and available capacity have been checked. */
/* Registration admits the candidate and existing catalogue before duplicate lookup or publication. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_tool_registry_add(UmiAiToolRegistry *registry,
                                   const UmiAiTool *tool)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || tool == NULL || tool->tool_id[0] == '\0' ||
        tool->invoke == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_ai_tool_registry_find(registry, tool->tool_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_AI_MAX_TOOLS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->tools[registry->count++] = *tool;
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_tool_registry_add(UmiAiToolRegistry *registry, const UmiAiTool *tool)
{
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiToolValidate(tool);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiToolRegistryValidate(registry);
    if (status != UMI_STATUS_OK) return status;
    for (size_t index = 0U; index < registry->count; ++index)
        if (strcmp(registry->tools[index].tool_id, tool->tool_id) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    if (registry->count == UMI_AI_MAX_TOOLS) return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiAiTool candidate = *tool;
    registry->tools[registry->count] = candidate;
    ++registry->count;
    return UMI_STATUS_OK;
}
/*
 * Find ai tool registry while leaving the underlying catalogue or model owned by this
 * module.
 */
/* Checked resolution validates catalogue bounds and text before comparison while keeping the pointer-returning compatibility API. The former implementation is retained for owner review. */
#if 0
UmiAiTool *umi_ai_tool_registry_find(UmiAiToolRegistry *registry,
                                     const char *tool_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || tool_id == NULL) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->tools[index].tool_id, tool_id) == 0) {
            return &registry->tools[index];
        }
    }
    return NULL;
}

#endif
/* Admit the complete declared catalogue before lookup. This prevents an early
 * matching identifier from hiding a malformed later record. Unused capacity
 * is not part of the catalogue and is deliberately left uninterpreted. */
UmiStatus UmiAiToolRegistryValidate(const UmiAiToolRegistry *registry)
{
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (registry->count > UMI_AI_MAX_TOOLS) return UMI_STATUS_INVALID_STATE;
    for (size_t index = 0U; index < registry->count; ++index) {
        if (UmiAiToolValidate(&registry->tools[index]) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_STATE;
        for (size_t earlier = 0U; earlier < index; ++earlier)
            if (strcmp(registry->tools[earlier].tool_id, registry->tools[index].tool_id) == 0)
                return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiAiToolRegistryResolve(UmiAiToolRegistry *registry,
    const char *tool_id, UmiAiTool **out_tool)
{
    if (registry == NULL || tool_id == NULL || out_tool == NULL || tool_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_AI_ID_CAPACITY && tool_id[length] != '\0') ++length;
    if (length == UMI_AI_ID_CAPACITY) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiToolRegistryValidate(registry);
    if (status != UMI_STATUS_OK) return status;
    for (size_t index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->tools[index].tool_id, tool_id) == 0) {
            *out_tool = &registry->tools[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiAiTool *umi_ai_tool_registry_find(UmiAiToolRegistry *registry, const char *tool_id)
{
    UmiAiTool *tool = NULL;
    return UmiAiToolRegistryResolve(registry, tool_id, &tool) == UMI_STATUS_OK ? tool : NULL;
}