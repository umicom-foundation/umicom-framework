/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/copy_propagation.h
 *
 * PURPOSE:
 *   Replace uses of trivial IR copy results with their source values and erase redundant copy instructions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_COPY_PROPAGATION_H
#define UMICOM_COMPILER_NATIVE_COPY_PROPAGATION_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/ir_function.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the nc copy propagate operation used by this module and its client applications.
 */
UmiStatus umi_nc_copy_propagate(UmiNativeIrFunction *function,size_t *out_replacements,size_t *out_removed_copies);
#ifdef __cplusplus
}
#endif
#endif
