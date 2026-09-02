/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/common.h
 *
 * PURPOSE:
 *   Publish the public common contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_COMMON_H
#define UMICOM_COMPILER_COMMON_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#define UMI_COMPILER_ID_CAPACITY 128U
#define UMI_COMPILER_NAME_CAPACITY 192U
#define UMI_COMPILER_TEXT_CAPACITY 512U
#define UMI_COMPILER_PATH_CAPACITY 2048U
#define UMI_COMPILER_MAX_ARGUMENTS 96U
#define UMI_COMPILER_MAX_OPTIONS 32U
#define UMI_COMPILER_MAX_UNITS 128U
#define UMI_COMPILER_MAX_PROVIDERS 16U
#define UMI_COMPILER_MAX_PROFILES 32U
#define UMI_COMPILER_API_VERSION 1U
/**
 * List the named compiler language values accepted by this public contract.
 */
typedef enum UmiCompilerLanguage {
    UMI_COMPILER_LANGUAGE_UNKNOWN = 0,
    UMI_COMPILER_LANGUAGE_C = 1,
    UMI_COMPILER_LANGUAGE_CPP = 2,
    UMI_COMPILER_LANGUAGE_ASSEMBLY = 3,
    UMI_COMPILER_LANGUAGE_RUST = 4,
    UMI_COMPILER_LANGUAGE_ZIG = 5,
    UMI_COMPILER_LANGUAGE_UAI = 6
} UmiCompilerLanguage;
/**
 * List the named compiler family values accepted by this public contract.
 */
typedef enum UmiCompilerFamily {
    UMI_COMPILER_FAMILY_UNKNOWN = 0,
    UMI_COMPILER_FAMILY_GCC,
    UMI_COMPILER_FAMILY_CLANG,
    UMI_COMPILER_FAMILY_MSVC,
    UMI_COMPILER_FAMILY_RUSTC,
    UMI_COMPILER_FAMILY_ZIG,
    UMI_COMPILER_FAMILY_ASSEMBLER,
    UMI_COMPILER_FAMILY_UAI,
    UMI_COMPILER_FAMILY_UMICC
} UmiCompilerFamily;
/**
 * List the named compiler action values accepted by this public contract.
 */
typedef enum UmiCompilerAction {
    UMI_COMPILER_ACTION_COMPILE = 1,
    UMI_COMPILER_ACTION_ASSEMBLE,
    UMI_COMPILER_ACTION_LINK_EXECUTABLE,
    UMI_COMPILER_ACTION_LINK_SHARED,
    UMI_COMPILER_ACTION_ARCHIVE,
    UMI_COMPILER_ACTION_CHECK
} UmiCompilerAction;
/**
 * List the named compiler object format values accepted by this public contract.
 */
typedef enum UmiCompilerObjectFormat {
    UMI_COMPILER_OBJECT_UNKNOWN = 0,
    UMI_COMPILER_OBJECT_COFF,
    UMI_COMPILER_OBJECT_ELF,
    UMI_COMPILER_OBJECT_MACH_O,
    UMI_COMPILER_OBJECT_WASM
} UmiCompilerObjectFormat;
/**
 * List the named compiler diagnostic severity values accepted by this public contract.
 */
typedef enum UmiCompilerDiagnosticSeverity {
    UMI_COMPILER_DIAGNOSTIC_NOTE = 1,
    UMI_COMPILER_DIAGNOSTIC_WARNING,
    UMI_COMPILER_DIAGNOSTIC_ERROR,
    UMI_COMPILER_DIAGNOSTIC_FATAL
} UmiCompilerDiagnosticSeverity;
#define UMI_COMPILER_LANGUAGE_BIT(language) (UINT32_C(1) << (uint32_t)(language))
#endif
