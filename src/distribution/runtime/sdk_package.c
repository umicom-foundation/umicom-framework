/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/sdk_package.c
 *
 * PURPOSE:
 *   developer SDK package metadata and ABI compatibility range.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/sdk_package.h"

/*
 * Initialise dr sdk package from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_sdk_package_init(UmiDrSdkPackage *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrSdkPackage){0};  } }
/* Check that dr sdk package satisfies its contract before another service relies on it. */
bool umi_dr_sdk_package_valid(const UmiDrSdkPackage *value) { return value != NULL && (value->id[0] != '\0' && (value->headers || value->libraries) && umi_dr_version_compare(value->maximum_abi,value->minimum_abi)>=0); }
/*
 * Provide the dr sdk package fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_sdk_package_fingerprint(const UmiDrSdkPackage *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_sdk_package_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
