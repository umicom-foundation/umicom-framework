/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/endianness.h
 *
 * PURPOSE:
 *   Provide explicit endian conversion helpers for portable file, IPC and device structures.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_ENDIANNESS_H
#define UMICOM_PLATFORM_CROSS_TARGET_ENDIANNESS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiCtEndian umi_ct_host_endian(void);
uint16_t umi_ct_bswap16(uint16_t value);
uint32_t umi_ct_bswap32(uint32_t value);
uint64_t umi_ct_bswap64(uint64_t value);
uint32_t umi_ct_to_endian32(uint32_t value,UmiCtEndian from,UmiCtEndian to);

#ifdef __cplusplus
}
#endif

#endif
