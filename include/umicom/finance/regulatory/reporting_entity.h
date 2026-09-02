/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/reporting_entity.h
 *
 * PURPOSE:
 *   Represent one legal reporting entity and consolidation perimeter.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_ENTITY_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_ENTITY_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the reporting entity data shared with callers of this public contract.
 */
typedef struct UmiReportingEntity {
    char entity_id[UMI_REG_ID_CAPACITY];
    char legal_name[UMI_REG_NAME_CAPACITY];
    char jurisdiction[UMI_REG_NAME_CAPACITY];
    int consolidated;
} UmiReportingEntity;

/* Initialise one validated represent one legal reporting entity and consolidation perimeter. record. */
UmiStatus umi_reg_reporting_entity_init(UmiReportingEntity *record, const char *entity_id, const char *legal_name, const char *jurisdiction, int consolidated);

#ifdef __cplusplus
}
#endif

#endif
