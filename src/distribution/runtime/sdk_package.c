/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/sdk_package.c
 *
 * PURPOSE:
 *   developer SDK package metadata and ABI compatibility range.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/sdk_package.h"

void umi_dr_sdk_package_init(UmiDrSdkPackage *value) { if (value != NULL) { *value = (UmiDrSdkPackage){0};  } }
bool umi_dr_sdk_package_valid(const UmiDrSdkPackage *value) { return value != NULL && (value->id[0] != '\0' && (value->headers || value->libraries) && umi_dr_version_compare(value->maximum_abi,value->minimum_abi)>=0); }
uint64_t umi_dr_sdk_package_fingerprint(const UmiDrSdkPackage *value) {
    uint64_t h = 0U;
    if (!umi_dr_sdk_package_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
