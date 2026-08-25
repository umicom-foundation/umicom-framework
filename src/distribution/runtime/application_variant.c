/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/application_variant.c
 *
 * PURPOSE:
 *   platform-specific application variants without moving reusable logic into products.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/application_variant.h"

void umi_dr_application_variant_init(UmiDrApplicationVariant *value) { if (value != NULL) { *value = (UmiDrApplicationVariant){0}; value->platform=UMI_DR_PLATFORM_WINDOWS; value->architecture=UMI_DR_ARCH_X86_64; value->preferred_format=UMI_DR_PACKAGE_ZIP; } }
bool umi_dr_application_variant_valid(const UmiDrApplicationVariant *value) { return value != NULL && (value->id[0] != '\0' && value->platform != 0 && value->architecture != 0 && value->entrypoint[0] != '\0'); }
uint64_t umi_dr_application_variant_fingerprint(const UmiDrApplicationVariant *value) {
    uint64_t h = 0U;
    if (!umi_dr_application_variant_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
