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

/* Keep the public C symbol names when this header is included directly by a
 * C++ client. Includes remain outside the linkage block; layouts are unchanged. */
#ifdef __cplusplus
extern "C" {
#endif
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
/**
 * Initialization preserves the destination on refusal and accepts its own identifier
 * and target as inputs. An overlong identifier returns CAPACITY_EXCEEDED.
 * Compatibility rejects unterminated ABI/target text, then retains its existing
 * target, pointer-width and kind comparison. Storage admission does not certify an ABI.
 */
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
