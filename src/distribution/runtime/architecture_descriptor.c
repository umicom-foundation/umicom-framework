/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/architecture_descriptor.c
 *
 * PURPOSE:
 *   CPU architecture, ABI, pointer-width and endianness descriptors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/architecture_descriptor.h"

/*
 * Initialise dr architecture descriptor from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_architecture_descriptor_init(UmiDrArchitectureDescriptor *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrArchitectureDescriptor){0}; value->architecture=UMI_DR_ARCH_X86_64; value->endian=UMI_DR_ENDIAN_LITTLE; value->pointer_bits=64U; } }
/*
 * Check that dr architecture descriptor satisfies its contract before another service
 * relies on it.
 */
bool umi_dr_architecture_descriptor_valid(const UmiDrArchitectureDescriptor *value) { return value != NULL && (value->id[0] != '\0' && (value->pointer_bits==32U || value->pointer_bits==64U) && value->abi[0] != '\0'); }
/*
 * Provide the dr architecture descriptor fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_architecture_descriptor_fingerprint(const UmiDrArchitectureDescriptor *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_architecture_descriptor_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
