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
/**
 * Represent the native assembly buffer data shared with callers of this public contract.
 */
typedef struct UmiNativeAssemblyBuffer { char text[UMI_NC_ASSEMBLY_BUFFER_CAPACITY]; size_t length; } UmiNativeAssemblyBuffer;
/**
 * Initialise nc assembly buffer from caller-provided values so later operations receive a
 * known state.
 */
void umi_nc_assembly_buffer_init(UmiNativeAssemblyBuffer *buffer);
/**
 * Provide the nc assembly emit function operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_assembly_emit_function(const UmiNativeMachineFunction *function,UmiNativeAssemblyBuffer *buffer);
#ifdef __cplusplus
}
#endif
#endif
