/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/diagnostic_parsers/java.h
 *
 * PURPOSE:
 *   Publish the normalized Java/javac diagnostic parser.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_DIAGNOSTIC_PARSER_JAVA_H
#define UMICOM_DEVELOPER_DIAGNOSTIC_PARSER_JAVA_H

#include "umicom/developer_productivity/diagnostic_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiDeveloperDiagnosticParser *umi_developer_diagnostic_parser_java(void);

#ifdef __cplusplus
}
#endif

#endif
