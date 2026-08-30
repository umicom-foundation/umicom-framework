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

UmiStatus umi_developer_diagnostic_registry_create(
    UmiDeveloperDiagnosticRegistry **out_registry)
{
    UmiDeveloperDiagnosticRegistry *registry;

    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;

    registry = (UmiDeveloperDiagnosticRegistry *)calloc(
        1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_developer_diagnostic_registry_destroy(
    UmiDeveloperDiagnosticRegistry *registry)
{
    free(registry);
}

UmiStatus umi_developer_diagnostic_registry_register(
    UmiDeveloperDiagnosticRegistry *registry,
    const UmiDeveloperDiagnosticParser *parser)
{
    size_t index;
    UmiStatus status;

    if (registry == NULL || parser == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_diagnostic_parser_validate(parser);
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->parsers[index]->parser_id,
                   parser->parser_id) == 0) {
            registry->parsers[index] = parser;
            return UMI_STATUS_OK;
        }
    }

    if (registry->count >= UMI_DEVELOPER_DIAGNOSTIC_PARSER_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /*
     * Small bounded registry: insertion sort by priority keeps dispatch stable
     * and avoids allocating another ordering structure.
     */
    index = registry->count;
    while (index > 0U &&
           registry->parsers[index - 1U]->priority < parser->priority) {
        registry->parsers[index] = registry->parsers[index - 1U];
        --index;
    }
    registry->parsers[index] = parser;
    registry->count += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_diagnostic_registry_parse(
    const UmiDeveloperDiagnosticRegistry *registry,
    const char *line,
    UmiDeveloperProblem *out_problem,
    int *out_matched)
{
    size_t index;

    if (registry == NULL || line == NULL ||
        out_problem == NULL || out_matched == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_matched = 0;

    for (index = 0U; index < registry->count; ++index) {
        int matched = 0;
        UmiStatus status = registry->parsers[index]->parse(
            line,
            out_problem,
            &matched);

        if (status != UMI_STATUS_OK) return status;

        if (matched) {
            *out_matched = 1;
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_OK;
}

size_t umi_developer_diagnostic_registry_count(
    const UmiDeveloperDiagnosticRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
