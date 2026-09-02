/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/directive.h
 *
 * PURPOSE:
 *   Classify C preprocessor directive lines without taking ownership of macro expansion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_DIRECTIVE_H
#define UMICOM_COMPILER_NATIVE_DIRECTIVE_H
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named native directive kind values accepted by this public contract.
 */
typedef enum UmiNativeDirectiveKind { UMI_NC_DIRECTIVE_UNKNOWN=0, UMI_NC_DIRECTIVE_DEFINE=1, UMI_NC_DIRECTIVE_UNDEF=2, UMI_NC_DIRECTIVE_INCLUDE=3, UMI_NC_DIRECTIVE_IF=4, UMI_NC_DIRECTIVE_IFDEF=5, UMI_NC_DIRECTIVE_IFNDEF=6, UMI_NC_DIRECTIVE_ELIF=7, UMI_NC_DIRECTIVE_ELSE=8, UMI_NC_DIRECTIVE_ENDIF=9, UMI_NC_DIRECTIVE_PRAGMA=10, UMI_NC_DIRECTIVE_ERROR=11, UMI_NC_DIRECTIVE_LINE=12 } UmiNativeDirectiveKind;
/**
 * Represent the native directive data shared with callers of this public contract.
 */
typedef struct UmiNativeDirective { UmiNativeDirectiveKind kind; char argument[UMI_NC_TEXT_CAPACITY]; } UmiNativeDirective;
/**
 * Read nc directive into validated module state and return a status when input cannot be
 * used.
 */
UmiStatus umi_nc_directive_parse(const char *line,UmiNativeDirective *out_directive);
/**
 * Provide the nc directive kind name operation used by this module and its client
 * applications.
 */
const char *umi_nc_directive_kind_name(UmiNativeDirectiveKind kind);
#ifdef __cplusplus
}
#endif
#endif
