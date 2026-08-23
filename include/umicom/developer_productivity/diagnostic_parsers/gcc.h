/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/diagnostic_parsers/gcc.h
 *
 * PURPOSE:
 *   Publish the normalized GCC diagnostic parser.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_DIAGNOSTIC_PARSER_GCC_H
#define UMICOM_DEVELOPER_DIAGNOSTIC_PARSER_GCC_H

#include "umicom/developer_productivity/diagnostic_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiDeveloperDiagnosticParser *umi_developer_diagnostic_parser_gcc(void);

#ifdef __cplusplus
}
#endif

#endif
