/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/diagnostic_parsers/ctest.h
 *
 * PURPOSE:
 *   Publish the normalized CTest diagnostic parser.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_DIAGNOSTIC_PARSER_CTEST_H
#define UMICOM_DEVELOPER_DIAGNOSTIC_PARSER_CTEST_H

#include "umicom/developer_productivity/diagnostic_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiDeveloperDiagnosticParser *umi_developer_diagnostic_parser_ctest(void);

#ifdef __cplusplus
}
#endif

#endif
