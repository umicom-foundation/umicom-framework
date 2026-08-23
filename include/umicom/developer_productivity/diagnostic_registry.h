/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/diagnostic_registry.h
 *
 * PURPOSE:
 *   Register normalized diagnostic parsers and parse one output line through
 *   priority-ordered providers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_DIAGNOSTIC_REGISTRY_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_DIAGNOSTIC_REGISTRY_H

#include "umicom/developer_productivity/diagnostic_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_DIAGNOSTIC_PARSER_CAPACITY 32U

typedef struct UmiDeveloperDiagnosticRegistry
    UmiDeveloperDiagnosticRegistry;

UmiStatus umi_developer_diagnostic_registry_create(
    UmiDeveloperDiagnosticRegistry **out_registry);

void umi_developer_diagnostic_registry_destroy(
    UmiDeveloperDiagnosticRegistry *registry);

UmiStatus umi_developer_diagnostic_registry_register(
    UmiDeveloperDiagnosticRegistry *registry,
    const UmiDeveloperDiagnosticParser *parser);

UmiStatus umi_developer_diagnostic_registry_parse(
    const UmiDeveloperDiagnosticRegistry *registry,
    const char *line,
    UmiDeveloperProblem *out_problem,
    int *out_matched);

size_t umi_developer_diagnostic_registry_count(
    const UmiDeveloperDiagnosticRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
