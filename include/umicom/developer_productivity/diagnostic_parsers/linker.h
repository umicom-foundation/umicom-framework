/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/diagnostic_parsers/linker.h
 *
 * PURPOSE:
 *   Publish the normalized Native Linker diagnostic parser.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_DIAGNOSTIC_PARSER_LINKER_H
#define UMICOM_DEVELOPER_DIAGNOSTIC_PARSER_LINKER_H

#include "umicom/developer_productivity/diagnostic_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer diagnostic parser linker operation used by this module and its
 * client applications.
 */
const UmiDeveloperDiagnosticParser *umi_developer_diagnostic_parser_linker(void);

#ifdef __cplusplus
}
#endif

#endif
