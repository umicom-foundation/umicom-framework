/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/portfolio.h
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
#ifndef UMICOM_APPLICATION_PRODUCTION_PORTFOLIO_H
#define UMICOM_APPLICATION_PRODUCTION_PORTFOLIO_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/runtime.h"

/**
 * Represent the application production portfolio entry data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationProductionPortfolioEntry {
    char module_id[UMI_APPLICATION_PRODUCTION_ID_CAPACITY];
    char executable_id[UMI_APPLICATION_PRODUCTION_ID_CAPACITY];
    UmiProductApplicationAdoption adoption;
    UmiApplicationProductionRuntime runtime;
} UmiApplicationProductionPortfolioEntry;

/**
 * Represent the application production portfolio data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationProductionPortfolio {
    UmiApplicationProductionPortfolioEntry
        entries[UMI_APPLICATION_PRODUCTION_MAX_APPLICATIONS];
    size_t count;
} UmiApplicationProductionPortfolio;

/**
 * Provide the application production portfolio build operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_production_portfolio_build(
    UmiApplicationCapabilityProbe probe, void *probe_context,
    UmiApplicationProductionPortfolio *out_portfolio);
/**
 * Find application production portfolio while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationProductionPortfolioEntry *
umi_application_production_portfolio_find(
    const UmiApplicationProductionPortfolio *portfolio,
    const char *application_id);

#ifdef __cplusplus
}
#endif
#endif
