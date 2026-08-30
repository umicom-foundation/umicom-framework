/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/assembly_emitter.h
 *
 * PURPOSE:
 *   Emit deterministic textual assembly from selected machine functions for bootstrap inspection and external assemblers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_ASSEMBLY_EMITTER_H
#define UMICOM_COMPILER_NATIVE_ASSEMBLY_EMITTER_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/machine_function.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NC_ASSEMBLY_BUFFER_CAPACITY 32768U
typedef struct UmiNativeAssemblyBuffer { char text[UMI_NC_ASSEMBLY_BUFFER_CAPACITY]; size_t length; } UmiNativeAssemblyBuffer;
void umi_nc_assembly_buffer_init(UmiNativeAssemblyBuffer *buffer);
UmiStatus umi_nc_assembly_emit_function(const UmiNativeMachineFunction *function,UmiNativeAssemblyBuffer *buffer);
#ifdef __cplusplus
}
#endif
#endif
