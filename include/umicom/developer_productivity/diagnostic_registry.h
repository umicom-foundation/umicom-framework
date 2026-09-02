/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/diagnostic_registry.h
 *
 * PURPOSE:
 *   Register normalized diagnostic parsers and parse one output line through
 *   priority-ordered providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_DIAGNOSTIC_REGISTRY_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_DIAGNOSTIC_REGISTRY_H

#include "umicom/developer_productivity/diagnostic_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_DIAGNOSTIC_PARSER_CAPACITY 32U

/**
 * Represent the developer diagnostic registry data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperDiagnosticRegistry
    UmiDeveloperDiagnosticRegistry;

/**
 * Initialise developer diagnostic registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_developer_diagnostic_registry_create(
    UmiDeveloperDiagnosticRegistry **out_registry);

/**
 * Release or reset state held by developer diagnostic registry so the same storage can be
 * reused safely.
 */
void umi_developer_diagnostic_registry_destroy(
    UmiDeveloperDiagnosticRegistry *registry);

/**
 * Add developer diagnostic registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_developer_diagnostic_registry_register(
    UmiDeveloperDiagnosticRegistry *registry,
    const UmiDeveloperDiagnosticParser *parser);

/**
 * Read developer diagnostic registry into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_developer_diagnostic_registry_parse(
    const UmiDeveloperDiagnosticRegistry *registry,
    const char *line,
    UmiDeveloperProblem *out_problem,
    int *out_matched);

/**
 * Return the number of records represented by developer diagnostic registry without
 * changing their state.
 */
size_t umi_developer_diagnostic_registry_count(
    const UmiDeveloperDiagnosticRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
