/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/diagnostic_registry.c
 *
 * PURPOSE:
 *   Implement normalized diagnostic parser registration and dispatch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/diagnostic_registry.h"

#include <stdlib.h>
#include <string.h>

struct UmiDeveloperDiagnosticRegistry {
    const UmiDeveloperDiagnosticParser *
        parsers[UMI_DEVELOPER_DIAGNOSTIC_PARSER_CAPACITY];
    size_t count;
};

/*
 * Initialise developer diagnostic registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_developer_diagnostic_registry_create(
    UmiDeveloperDiagnosticRegistry **out_registry)
{
    UmiDeveloperDiagnosticRegistry *registry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;

    registry = (UmiDeveloperDiagnosticRegistry *)calloc(
        1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by developer diagnostic registry so the same storage can be
 * reused safely.
 */
void umi_developer_diagnostic_registry_destroy(
    UmiDeveloperDiagnosticRegistry *registry)
{
    free(registry);
}

/*
 * Add developer diagnostic registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_developer_diagnostic_registry_register(
    UmiDeveloperDiagnosticRegistry *registry,
    const UmiDeveloperDiagnosticParser *parser)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || parser == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_diagnostic_parser_validate(parser);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->parsers[index]->parser_id,
                   parser->parser_id) == 0) {
            registry->parsers[index] = parser;
            return UMI_STATUS_OK;
        }
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_DEVELOPER_DIAGNOSTIC_PARSER_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /*
     * Small bounded registry: insertion sort by priority keeps dispatch stable
     * and avoids allocating another ordering structure.
     */
    index = registry->count;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index > 0U &&
           registry->parsers[index - 1U]->priority < parser->priority) {
        registry->parsers[index] = registry->parsers[index - 1U];
        --index;
    }
    registry->parsers[index] = parser;
    registry->count += 1U;
    return UMI_STATUS_OK;
}

/*
 * Read developer diagnostic registry into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_developer_diagnostic_registry_parse(
    const UmiDeveloperDiagnosticRegistry *registry,
    const char *line,
    UmiDeveloperProblem *out_problem,
    int *out_matched)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || line == NULL ||
        out_problem == NULL || out_matched == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_matched = 0;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        int matched = 0;
        UmiStatus status = registry->parsers[index]->parse(
            line,
            out_problem,
            &matched);

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (matched) {
            *out_matched = 1;
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by developer diagnostic registry without
 * changing their state.
 */
size_t umi_developer_diagnostic_registry_count(
    const UmiDeveloperDiagnosticRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
