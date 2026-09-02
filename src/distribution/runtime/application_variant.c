/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/application_variant.c
 *
 * PURPOSE:
 *   platform-specific application variants without moving reusable logic into products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/application_variant.h"

/*
 * Initialise dr application variant from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_application_variant_init(UmiDrApplicationVariant *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrApplicationVariant){0}; value->platform=UMI_DR_PLATFORM_WINDOWS; value->architecture=UMI_DR_ARCH_X86_64; value->preferred_format=UMI_DR_PACKAGE_ZIP; } }
/*
 * Check that dr application variant satisfies its contract before another service relies
 * on it.
 */
bool umi_dr_application_variant_valid(const UmiDrApplicationVariant *value) { return value != NULL && (value->id[0] != '\0' && value->platform != 0 && value->architecture != 0 && value->entrypoint[0] != '\0'); }
/*
 * Provide the dr application variant fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_application_variant_fingerprint(const UmiDrApplicationVariant *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_application_variant_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
