/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/surface_reference.h
 *
 * PURPOSE:
 *   Define provider-neutral surface references.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_SURFACE_REFERENCE_H
#define UMICOM_FINANCE_CORE_SURFACE_REFERENCE_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSurfaceReference { UmiFinancialId reference_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; char code[UMI_FINANCIAL_CORE_CODE_CAPACITY]; bool active; } UmiSurfaceReference;
/* Initialize the typed financial record. */ UmiStatus umi_surface_reference_init(UmiSurfaceReference *item,const char *id,const char *name,const char *code);
/* Validate the typed financial record. */ bool umi_surface_reference_is_valid(const UmiSurfaceReference *item);
#ifdef __cplusplus
}
#endif

#endif
