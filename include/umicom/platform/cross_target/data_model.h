/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/data_model.h
 *
 * PURPOSE:
 *   Resolve portable C fundamental-type widths for ILP32, LP64 and LLP64 target data models.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_CROSS_TARGET_DATA_MODEL_H
#define UMICOM_PLATFORM_CROSS_TARGET_DATA_MODEL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/abi_descriptor.h"
typedef struct UmiCtTypeWidths { uint16_t short_bits; uint16_t int_bits; uint16_t long_bits; uint16_t long_long_bits; uint16_t pointer_bits; uint16_t size_t_bits; } UmiCtTypeWidths;
UmiStatus umi_ct_data_model_widths(UmiCtDataModel model,UmiCtTypeWidths *out_widths);

#ifdef __cplusplus
}
#endif

#endif
