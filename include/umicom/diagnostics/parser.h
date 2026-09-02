/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/parser.h
 *
 * PURPOSE:
 *   Publish the public parser contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_PARSER_H
#define UMICOM_DIAGNOSTICS_PARSER_H

#include "umicom/diagnostics/diagnostic.h"
#include "umicom/diagnostics/output.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiDiagnosticParseFn)(const UmiOutputRecord *output,
                                          UmiDiagnosticSnapshot *out_diagnostic,
                                          int *out_matched,
                                          void *user_data);

/**
 * Represent the diagnostic parser data shared with callers of this public contract.
 */
typedef struct UmiDiagnosticParser {
    const char *parser_id;
    int priority;
    UmiDiagnosticParseFn parse;
    void *user_data;
} UmiDiagnosticParser;

#define UMI_DIAGNOSTIC_PARSER_MAX 32U

/**
 * Represent the diagnostic parser registry data shared with callers of this public
 * contract.
 */
typedef struct UmiDiagnosticParserRegistry UmiDiagnosticParserRegistry;

/**
 * Initialise diagnostic parser registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_diagnostic_parser_registry_create(
    UmiDiagnosticParserRegistry **out_registry);
/**
 * Release or reset state held by diagnostic parser registry so the same storage can be
 * reused safely.
 */
void umi_diagnostic_parser_registry_destroy(UmiDiagnosticParserRegistry *registry);
/**
 * Add diagnostic parser registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_diagnostic_parser_registry_add(UmiDiagnosticParserRegistry *registry,
                                             const UmiDiagnosticParser *parser);
/**
 * Remove diagnostic parser registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_diagnostic_parser_registry_remove(
    UmiDiagnosticParserRegistry *registry,
    const char *parser_id);
/**
 * Read diagnostic parser registry into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_diagnostic_parser_registry_parse(
    const UmiDiagnosticParserRegistry *registry,
    const UmiOutputRecord *output,
    UmiDiagnosticSnapshot *out_diagnostic,
    int *out_matched);
/**
 * Return the number of records represented by diagnostic parser registry without changing
 * their state.
 */
size_t umi_diagnostic_parser_registry_count(
    const UmiDiagnosticParserRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif
