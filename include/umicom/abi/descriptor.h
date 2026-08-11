/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/abi/descriptor.h
 *
 * PURPOSE:
 *   Describe one named public ABI with features, symbols and structure-layout evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * ABI files describe agreements between separately compiled code. They help us detect unsafe binary changes before an application is released.
 */
#ifndef UMICOM_ABI_DESCRIPTOR_H
#define UMICOM_ABI_DESCRIPTOR_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/abi/feature.h"
#include "umicom/abi/platform.h"
#include "umicom/abi/struct_layout.h"
#include "umicom/abi/symbol.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAbiDescriptor { uint32_t structure_size; const char *abi_id; uint32_t abi_version; UmiAbiPlatform platform; const UmiAbiFeature *features; size_t feature_count; const UmiAbiSymbol *symbols; size_t symbol_count; const UmiAbiStructLayout *layouts; size_t layout_count; } UmiAbiDescriptor;
UmiStatus umi_abi_descriptor_validate(const UmiAbiDescriptor *descriptor);
#ifdef __cplusplus
}
#endif
#endif
