/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/frame_layout.h
 *
 * PURPOSE:
 *   Allocate aligned stack-frame slots and compute deterministic final frame sizes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_FRAME_LAYOUT_H
#define UMICOM_COMPILER_NATIVE_FRAME_LAYOUT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NC_MAX_FRAME_SLOTS 64U
/**
 * Represent the native frame slot data shared with callers of this public contract.
 */
typedef struct UmiNativeFrameSlot { uint32_t id; size_t size; size_t alignment; size_t offset; } UmiNativeFrameSlot;
/**
 * Represent the native frame layout data shared with callers of this public contract.
 */
typedef struct UmiNativeFrameLayout { UmiNativeFrameSlot slots[UMI_NC_MAX_FRAME_SLOTS]; size_t count; size_t size; size_t alignment; } UmiNativeFrameLayout;
/**
 * Initialise nc frame layout from caller-provided values so later operations receive a
 * known state.
 */
void umi_nc_frame_layout_init(UmiNativeFrameLayout *layout,size_t alignment);
/**
 * Add nc frame layout only after its inputs and available capacity have been checked.
 */
UmiStatus umi_nc_frame_layout_add(UmiNativeFrameLayout *layout,uint32_t id,size_t size,size_t alignment,size_t *out_offset);
/**
 * Provide the nc frame layout finalize operation used by this module and its client
 * applications.
 */
size_t umi_nc_frame_layout_finalize(UmiNativeFrameLayout *layout);
#ifdef __cplusplus
}
#endif
#endif
