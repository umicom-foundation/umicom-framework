/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/diagnostic_parsers/clang.h
 *
 * PURPOSE:
 *   Publish the normalized Clang diagnostic parser.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_DIAGNOSTIC_PARSER_CLANG_H
#define UMICOM_DEVELOPER_DIAGNOSTIC_PARSER_CLANG_H

#include "umicom/developer_productivity/diagnostic_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer diagnostic parser clang operation used by this module and its
 * client applications.
 */
const UmiDeveloperDiagnosticParser *umi_developer_diagnostic_parser_clang(void);

#ifdef __cplusplus
}
#endif

#endif
