/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/target.h
 *
 * PURPOSE:
 *   Publish the public target contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_TARGET_H
#define UMICOM_COMPILER_TARGET_H
#include "umicom/compiler/common.h"
/**
 * Represent the compiler target data shared with callers of this public contract.
 */
typedef struct UmiCompilerTarget {
    char triple[UMI_COMPILER_NAME_CAPACITY];
    char architecture[64U];
    char vendor[64U];
    char operating_system[64U];
    char environment[64U];
    UmiCompilerObjectFormat object_format;
    uint32_t pointer_width;
    bool little_endian;
} UmiCompilerTarget;
/**
 * Read compiler target into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_compiler_target_parse(const char *triple,UmiCompilerTarget *out_target);
/**
 * Provide the compiler target host operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_target_host(UmiCompilerTarget *out_target);
/**
 * Provide the compiler target compatible operation used by this module and its client
 * applications.
 */
bool umi_compiler_target_compatible(const UmiCompilerTarget *left,const UmiCompilerTarget *right);
/**
 * Provide the compiler object format name operation used by this module and its client
 * applications.
 */
const char *umi_compiler_object_format_name(UmiCompilerObjectFormat format);
#endif
