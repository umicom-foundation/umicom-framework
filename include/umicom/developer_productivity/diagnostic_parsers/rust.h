/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/diagnostic_parsers/rust.h
 *
 * PURPOSE:
 *   Publish the normalized Rust diagnostic parser.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_DIAGNOSTIC_PARSER_RUST_H
#define UMICOM_DEVELOPER_DIAGNOSTIC_PARSER_RUST_H

#include "umicom/developer_productivity/diagnostic_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiDeveloperDiagnosticParser *umi_developer_diagnostic_parser_rust(void);

#ifdef __cplusplus
}
#endif

#endif
