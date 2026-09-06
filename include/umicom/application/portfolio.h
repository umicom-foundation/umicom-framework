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
 * Return the native GUI executable basename for a supported product, without
 * a platform suffix or directory. The immutable string is Framework-owned.
 * NULL means this ID has no native GUI build mapping; console commands are
 * never returned as a fallback. A mapping does not prove installation or
 * product feature completeness: the host must discover the executable first.
 */
const char *umi_application_portfolio_gui_executable(const char *application_id);
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
