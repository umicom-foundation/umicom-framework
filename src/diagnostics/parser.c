/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/parser.c
 *
 * PURPOSE:
 *   Implement the parser behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/parser.h"

#include <stdlib.h>
#include <string.h>

#include "diagnostic_lock.h"

struct UmiDiagnosticParserRegistry {
    UmiDiagnosticParser parsers[UMI_DIAGNOSTIC_PARSER_MAX];
    size_t count;
    UmiDiagnosticLock lock;
};

/*
 * Initialise diagnostic parser registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_diagnostic_parser_registry_create(UmiDiagnosticParserRegistry **out_registry)
{
    UmiDiagnosticParserRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiDiagnosticParserRegistry *)calloc(1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_diagnostic_lock_init(&registry->lock);
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by diagnostic parser registry so the same storage can be
 * reused safely.
 */
void umi_diagnostic_parser_registry_destroy(UmiDiagnosticParserRegistry *registry)
{
    free(registry);
}

/*
 * Add diagnostic parser registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_diagnostic_parser_registry_add(UmiDiagnosticParserRegistry *registry,
                                             const UmiDiagnosticParser *parser)
{
    size_t index;
    size_t destination;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || parser == NULL || parser->parser_id == NULL ||
        parser->parser_id[0] == '\0' || parser->parse == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&registry->lock);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->parsers[index].parser_id, parser->parser_id) == 0) {
            umi_diagnostic_lock_release(&registry->lock);
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count == UMI_DIAGNOSTIC_PARSER_MAX) {
        umi_diagnostic_lock_release(&registry->lock);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    destination = registry->count;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (destination > 0U && registry->parsers[destination - 1U].priority < parser->priority) {
        registry->parsers[destination] = registry->parsers[destination - 1U];
        --destination;
    }
    registry->parsers[destination] = *parser;
    ++registry->count;
    umi_diagnostic_lock_release(&registry->lock);
    return UMI_STATUS_OK;
}

/*
 * Remove diagnostic parser registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_diagnostic_parser_registry_remove(UmiDiagnosticParserRegistry *registry,
                                                const char *parser_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || parser_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&registry->lock);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->parsers[index].parser_id, parser_id) == 0) break;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == registry->count) {
        umi_diagnostic_lock_release(&registry->lock);
        return UMI_STATUS_NOT_FOUND;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (; index + 1U < registry->count; ++index) registry->parsers[index] = registry->parsers[index + 1U];
    --registry->count;
    umi_diagnostic_lock_release(&registry->lock);
    return UMI_STATUS_OK;
}

/*
 * Read diagnostic parser registry into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_diagnostic_parser_registry_parse(const UmiDiagnosticParserRegistry *registry,
                                               const UmiOutputRecord *output,
                                               UmiDiagnosticSnapshot *out_diagnostic,
                                               int *out_matched)
{
    UmiStatus status = UMI_STATUS_OK;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || output == NULL || out_diagnostic == NULL || out_matched == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_matched = 0;
    (void)memset(out_diagnostic, 0, sizeof(*out_diagnostic));
    umi_diagnostic_lock_acquire(&registry->lock);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        status = registry->parsers[index].parse(output, out_diagnostic, out_matched,
                                                registry->parsers[index].user_data);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK || *out_matched != 0) break;
    }
    umi_diagnostic_lock_release(&registry->lock);
    return status;
}

/*
 * Return the number of records represented by diagnostic parser registry without changing
 * their state.
 */
size_t umi_diagnostic_parser_registry_count(const UmiDiagnosticParserRegistry *registry)
{
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry != NULL) {
        umi_diagnostic_lock_acquire(&registry->lock);
        count = registry->count;
        umi_diagnostic_lock_release(&registry->lock);
    }
    return count;
}
