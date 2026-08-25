/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/architecture_descriptor.h
 *
 * PURPOSE:
 *   CPU architecture, ABI, pointer-width and endianness descriptors.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_ARCHITECTURE_DESCRIPTOR_H
#define UMICOM_DISTRIBUTION_RUNTIME_ARCHITECTURE_DESCRIPTOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrArchitectureDescriptor { char id[UMI_DR_ID_CAPACITY]; UmiDrArchitecture architecture; UmiDrEndian endian; uint32_t pointer_bits; char abi[UMI_DR_ID_CAPACITY]; } UmiDrArchitectureDescriptor;
void umi_dr_architecture_descriptor_init(UmiDrArchitectureDescriptor *value);
bool umi_dr_architecture_descriptor_valid(const UmiDrArchitectureDescriptor *value);
uint64_t umi_dr_architecture_descriptor_fingerprint(const UmiDrArchitectureDescriptor *value);

#ifdef __cplusplus
}
#endif
#endif
