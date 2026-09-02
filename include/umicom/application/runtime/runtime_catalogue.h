/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/runtime_catalogue.h
 *
 * PURPOSE:
 *   Validate catalogue alignment and expose cross-product readiness summaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_RUNTIME_CATALOGUE_H
#define UMICOM_APPLICATION_RUNTIME_RUNTIME_CATALOGUE_H

#include "umicom/application/runtime/readiness.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application runtime catalogue status data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationRuntimeCatalogueStatus {
    uint32_t structure_size;
    size_t portfolio_count;
    size_t experience_count;
    size_t aligned_count;
    size_t missing_experience_count;
    unsigned average_readiness_percent;
} UmiApplicationRuntimeCatalogueStatus;

/**
 * Provide the application runtime catalogue status operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_runtime_catalogue_status(
    UmiApplicationRuntimeCatalogueStatus *out_status);
/**
 * Check that application runtime catalogue satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_application_runtime_catalogue_validate(void);

#ifdef __cplusplus
}
#endif

#endif
