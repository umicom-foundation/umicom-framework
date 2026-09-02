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

/**
 * Return the number of records represented by developer builtin diagnostic parser without
 * changing their state.
 */
size_t umi_developer_builtin_diagnostic_parser_count(void);

/**
 * Find developer builtin diagnostic parser while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperDiagnosticParser *
umi_developer_builtin_diagnostic_parser_at(size_t index);

/**
 * Add developer builtin diagnostics only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_developer_builtin_diagnostics_register(
    UmiDeveloperDiagnosticRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
