/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/abi.h
 *
 * PURPOSE:
 *   Publish the public abi contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_ABI_H
#define UMICOM_COMPILER_ABI_H
#include "umicom/compiler/target.h"
/**
 * List the named compiler abi kind values accepted by this public contract.
 */
typedef enum UmiCompilerAbiKind { UMI_COMPILER_ABI_UNKNOWN = 0, UMI_COMPILER_ABI_C, UMI_COMPILER_ABI_CPP, UMI_COMPILER_ABI_SYSTEM, UMI_COMPILER_ABI_UAI } UmiCompilerAbiKind;
/**
 * Represent the compiler abi profile data shared with callers of this public contract.
 */
typedef struct UmiCompilerAbiProfile { char abi_id[UMI_COMPILER_ID_CAPACITY]; UmiCompilerAbiKind kind; UmiCompilerTarget target; uint32_t pointer_width; bool stable; bool exceptions; bool rtti; } UmiCompilerAbiProfile;
/**
 * Initialise compiler abi from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_compiler_abi_init(UmiCompilerAbiProfile *profile,const char *abi_id,UmiCompilerAbiKind kind,const UmiCompilerTarget *target);
/**
 * Provide the compiler abi compatible operation used by this module and its client
 * applications.
 */
bool umi_compiler_abi_compatible(const UmiCompilerAbiProfile *producer,const UmiCompilerAbiProfile *consumer);
/**
 * Provide the compiler abi name operation used by this module and its client applications.
 */
const char *umi_compiler_abi_name(UmiCompilerAbiKind kind);
#endif
