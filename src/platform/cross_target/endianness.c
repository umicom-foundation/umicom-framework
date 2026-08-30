/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/endianness.c
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

#include "umicom/platform/cross_target/endianness.h"

UmiCtEndian umi_ct_host_endian(void){const uint16_t v=1U;return *((const uint8_t*)&v)==1U?UMI_CT_ENDIAN_LITTLE:UMI_CT_ENDIAN_BIG;}
uint16_t umi_ct_bswap16(uint16_t v){return (uint16_t)((v>>8U)|(v<<8U));}
uint32_t umi_ct_bswap32(uint32_t v){return ((v&UINT32_C(0x000000ff))<<24U)|((v&UINT32_C(0x0000ff00))<<8U)|((v&UINT32_C(0x00ff0000))>>8U)|((v&UINT32_C(0xff000000))>>24U);}
uint64_t umi_ct_bswap64(uint64_t v){return ((uint64_t)umi_ct_bswap32((uint32_t)v)<<32U)|(uint64_t)umi_ct_bswap32((uint32_t)(v>>32U));}
uint32_t umi_ct_to_endian32(uint32_t v,UmiCtEndian a,UmiCtEndian b){return a==b?v:umi_ct_bswap32(v);}
