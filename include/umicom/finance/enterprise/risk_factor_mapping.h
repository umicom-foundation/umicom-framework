/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/risk_factor_mapping.h
 *
 * PURPOSE:
 *   Map product identifiers to factor loadings for portfolio risk aggregation.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_RISK_FACTOR_MAPPING_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_RISK_FACTOR_MAPPING_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseRiskFactorMappingEntry { UmiFinancialId product_id; char factor_name[UMI_ENTERPRISE_NAME_CAPACITY]; double loading; } UmiEnterpriseRiskFactorMappingEntry;
typedef struct UmiEnterpriseRiskFactorMapping { UmiEnterpriseRiskFactorMappingEntry entries[UMI_ENTERPRISE_MAX_ITEMS]; size_t count; } UmiEnterpriseRiskFactorMapping;
/* Initialise an empty factor mapping. */
void umi_enterprise_risk_factor_mapping_init(UmiEnterpriseRiskFactorMapping *mapping);
/* Add a finite product-to-factor loading. */
UmiStatus umi_enterprise_risk_factor_mapping_add(UmiEnterpriseRiskFactorMapping *mapping,const char *product_id,const char *factor_name,double loading);
/* Sum loadings for a product/factor pair. */
double umi_enterprise_risk_factor_mapping_loading(const UmiEnterpriseRiskFactorMapping *mapping,const char *product_id,const char *factor_name);

#ifdef __cplusplus
}
#endif

#endif
