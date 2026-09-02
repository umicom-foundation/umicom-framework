/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/runtime_parser.h
 *
 * PURPOSE:
 *   Publish the public runtime parser contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_RUNTIME_PARSER_H
#define UMICOM_DIAGNOSTICS_RUNTIME_PARSER_H

#include "umicom/diagnostics/parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the runtime diagnostic parser operation used by this module and its client
 * applications.
 */
UmiDiagnosticParser umi_runtime_diagnostic_parser(void);
/**
 * Read runtime diagnostic into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_runtime_diagnostic_parse(const UmiOutputRecord *output,
                                       UmiDiagnosticSnapshot *out_diagnostic,
                                       int *out_matched,
                                       void *user_data);

#ifdef __cplusplus
}
#endif
#endif
