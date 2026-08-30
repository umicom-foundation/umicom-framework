/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/builtin_diagnostics.h
 *
 * PURPOSE:
 *   Enumerate and bulk-register built-in compiler/build/test parsers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_BUILTIN_DIAGNOSTICS_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_BUILTIN_DIAGNOSTICS_H

#include "umicom/developer_productivity/diagnostic_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_developer_builtin_diagnostic_parser_count(void);

const UmiDeveloperDiagnosticParser *
umi_developer_builtin_diagnostic_parser_at(size_t index);

UmiStatus umi_developer_builtin_diagnostics_register(
    UmiDeveloperDiagnosticRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
