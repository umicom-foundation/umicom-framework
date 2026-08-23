/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/diagnostic_parsers/python.h
 *
 * PURPOSE:
 *   Publish the normalized Python diagnostic parser.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_DIAGNOSTIC_PARSER_PYTHON_H
#define UMICOM_DEVELOPER_DIAGNOSTIC_PARSER_PYTHON_H

#include "umicom/developer_productivity/diagnostic_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiDeveloperDiagnosticParser *umi_developer_diagnostic_parser_python(void);

#ifdef __cplusplus
}
#endif

#endif
