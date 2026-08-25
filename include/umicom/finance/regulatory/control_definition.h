/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/control_definition.h
 *
 * PURPOSE:
 *   Define a reusable financial or reporting control and severity.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_CONTROL_DEFINITION_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_CONTROL_DEFINITION_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiControlDefinition {
    char control_id[UMI_REG_ID_CAPACITY];
    char name[UMI_REG_NAME_CAPACITY];
    char owner_id[UMI_REG_ID_CAPACITY];
    UmiRegSeverity severity;
} UmiControlDefinition;

/* Initialise one validated define a reusable financial or reporting control and severity. record. */
UmiStatus umi_reg_control_definition_init(UmiControlDefinition *record, const char *control_id, const char *name, const char *owner_id, UmiRegSeverity severity);

#ifdef __cplusplus
}
#endif

#endif
