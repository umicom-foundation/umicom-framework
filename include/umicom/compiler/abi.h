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
typedef enum UmiCompilerAbiKind { UMI_COMPILER_ABI_UNKNOWN = 0, UMI_COMPILER_ABI_C, UMI_COMPILER_ABI_CPP, UMI_COMPILER_ABI_SYSTEM, UMI_COMPILER_ABI_UAI } UmiCompilerAbiKind;
typedef struct UmiCompilerAbiProfile { char abi_id[UMI_COMPILER_ID_CAPACITY]; UmiCompilerAbiKind kind; UmiCompilerTarget target; uint32_t pointer_width; bool stable; bool exceptions; bool rtti; } UmiCompilerAbiProfile;
UmiStatus umi_compiler_abi_init(UmiCompilerAbiProfile *profile,const char *abi_id,UmiCompilerAbiKind kind,const UmiCompilerTarget *target);
bool umi_compiler_abi_compatible(const UmiCompilerAbiProfile *producer,const UmiCompilerAbiProfile *consumer);
const char *umi_compiler_abi_name(UmiCompilerAbiKind kind);
#endif
