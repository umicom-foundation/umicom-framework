/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/diagnostic_parsers/rust.h
 *
 * PURPOSE:
 *   Publish the normalized Rust diagnostic parser.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_DIAGNOSTIC_PARSER_RUST_H
#define UMICOM_DEVELOPER_DIAGNOSTIC_PARSER_RUST_H

#include "umicom/developer_productivity/diagnostic_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer diagnostic parser rust operation used by this module and its
 * client applications.
 */
const UmiDeveloperDiagnosticParser *umi_developer_diagnostic_parser_rust(void);

#ifdef __cplusplus
}
#endif

#endif
