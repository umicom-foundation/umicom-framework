/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/portfolio.h
 *
 * PURPOSE:
 *   Publish the canonical Umicom application portfolio. This is the single
 *   catalogue used by launchers, Studio, Umicom OS, installers and audits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PORTFOLIO_H
#define UMICOM_APPLICATION_PORTFOLIO_H

#include "umicom/application/definition.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Return the number of records represented by application portfolio without changing their
 * state.
 */
size_t umi_application_portfolio_count(void);
/**
 * Find application portfolio while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiApplicationDefinition *umi_application_portfolio_at(size_t index);
/**
 * Find application portfolio while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiApplicationDefinition *umi_application_portfolio_find(
    const char *application_id);
/**
 * Return the number of records represented by application portfolio family without
 * changing their state.
 */
size_t umi_application_portfolio_family_count(UmiApplicationFamily family);
/**
 * Return the number of records represented by application portfolio maturity without
 * changing their state.
 */
size_t umi_application_portfolio_maturity_count(
    UmiApplicationMaturity maturity);

#ifdef __cplusplus
}
#endif

#endif
