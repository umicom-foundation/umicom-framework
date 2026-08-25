/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/kit_json.h
 * PURPOSE: Provide bounded machine-readable developer-kit snapshots for CLI/UI.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_KIT_JSON_H
#define UMICOM_TOOLCHAIN_KIT_JSON_H

#include <stddef.h>

#include "umicom/toolchain/kit_health.h"
#include "umicom/toolchain/kit_provenance.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_toolchain_kit_json_write(
    const UmiToolchainKitSnapshot *kit,
    const UmiToolchainKitHealthSnapshot *health,
    const UmiToolchainKitProvenanceSnapshot *provenance,
    char *out_json,
    size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
