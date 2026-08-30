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

typedef struct UmiDiagnosticParser {
    const char *parser_id;
    int priority;
    UmiDiagnosticParseFn parse;
    void *user_data;
} UmiDiagnosticParser;

#define UMI_DIAGNOSTIC_PARSER_MAX 32U

typedef struct UmiDiagnosticParserRegistry UmiDiagnosticParserRegistry;

UmiStatus umi_diagnostic_parser_registry_create(
    UmiDiagnosticParserRegistry **out_registry);
void umi_diagnostic_parser_registry_destroy(UmiDiagnosticParserRegistry *registry);
UmiStatus umi_diagnostic_parser_registry_add(UmiDiagnosticParserRegistry *registry,
                                             const UmiDiagnosticParser *parser);
UmiStatus umi_diagnostic_parser_registry_remove(
    UmiDiagnosticParserRegistry *registry,
    const char *parser_id);
UmiStatus umi_diagnostic_parser_registry_parse(
    const UmiDiagnosticParserRegistry *registry,
    const UmiOutputRecord *output,
    UmiDiagnosticSnapshot *out_diagnostic,
    int *out_matched);
size_t umi_diagnostic_parser_registry_count(
    const UmiDiagnosticParserRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif
