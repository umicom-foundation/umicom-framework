/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/manifest_contract.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_MANIFEST_CONTRACT_H
#define UMICOM_APPLICATION_PRODUCTION_MANIFEST_CONTRACT_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/application_binding.h"

/**
 * Represent the application production manifest contract data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationProductionManifestContract {
    char application_id[UMI_APPLICATION_PRODUCTION_ID_CAPACITY];
    char display_name[UMI_APPLICATION_PRODUCTION_TEXT_CAPACITY];
    char executable_id[UMI_APPLICATION_PRODUCTION_ID_CAPACITY];
    char default_layout_id[UMI_APPLICATION_PRODUCTION_ID_CAPACITY];
    uint32_t frontend_flags;
    size_t panel_count;
    size_t layout_count;
    size_t feature_count;
} UmiApplicationProductionManifestContract;

/**
 * Provide the application production manifest contract build operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_production_manifest_contract_build(
    const UmiApplicationProductionBinding *binding,
    UmiApplicationProductionManifestContract *out_contract);
/**
 * Check that application production manifest contract satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_application_production_manifest_contract_validate(
    const UmiApplicationProductionManifestContract *contract);

#ifdef __cplusplus
}
#endif
#endif
