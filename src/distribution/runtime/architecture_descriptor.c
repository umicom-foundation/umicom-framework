/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/architecture_descriptor.c
 *
 * PURPOSE:
 *   CPU architecture, ABI, pointer-width and endianness descriptors.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/architecture_descriptor.h"

void umi_dr_architecture_descriptor_init(UmiDrArchitectureDescriptor *value) { if (value != NULL) { *value = (UmiDrArchitectureDescriptor){0}; value->architecture=UMI_DR_ARCH_X86_64; value->endian=UMI_DR_ENDIAN_LITTLE; value->pointer_bits=64U; } }
bool umi_dr_architecture_descriptor_valid(const UmiDrArchitectureDescriptor *value) { return value != NULL && (value->id[0] != '\0' && (value->pointer_bits==32U || value->pointer_bits==64U) && value->abi[0] != '\0'); }
uint64_t umi_dr_architecture_descriptor_fingerprint(const UmiDrArchitectureDescriptor *value) {
    uint64_t h = 0U;
    if (!umi_dr_architecture_descriptor_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
