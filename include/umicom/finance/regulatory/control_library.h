/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/control_library.h
 *
 * PURPOSE:
 *   Maintain a bounded catalogue of unique financial-control identifiers.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_CONTROL_LIBRARY_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_CONTROL_LIBRARY_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the control library data shared with callers of this public contract.
 */
typedef struct UmiControlLibrary { char control_ids[UMI_REG_MAX_RULES][UMI_REG_ID_CAPACITY]; size_t count; } UmiControlLibrary;
/* Initialise an empty control library. */
void umi_reg_control_library_init(UmiControlLibrary *library);
/* Add one unique control identifier. */
UmiStatus umi_reg_control_library_add(UmiControlLibrary *library,const char *control_id);
/* Return one when the library contains a control identifier. */
int umi_reg_control_library_contains(const UmiControlLibrary *library,const char *control_id);

#ifdef __cplusplus
}
#endif

#endif
