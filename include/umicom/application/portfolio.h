/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/portfolio.h
 *
 * PURPOSE:
 *   Publish the canonical Umicom application portfolio. This is the single
 *   catalogue used by launchers, Studio, Umicom OS, installers and audits.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PORTFOLIO_H
#define UMICOM_APPLICATION_PORTFOLIO_H

#include "umicom/application/definition.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_application_portfolio_count(void);
const UmiApplicationDefinition *umi_application_portfolio_at(size_t index);
const UmiApplicationDefinition *umi_application_portfolio_find(
    const char *application_id);
size_t umi_application_portfolio_family_count(UmiApplicationFamily family);
size_t umi_application_portfolio_maturity_count(
    UmiApplicationMaturity maturity);

#ifdef __cplusplus
}
#endif

#endif
