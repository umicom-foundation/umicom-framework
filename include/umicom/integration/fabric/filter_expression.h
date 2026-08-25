/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/filter_expression.h
 *
 * PURPOSE:
 *   Describe a bounded textual equality/prefix filter used by route and workflow policies.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_FILTER_EXPRESSION_H
#define UMICOM_INTEGRATION_FABRIC_FILTER_EXPRESSION_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFabricFilterExpression {
    char field[UMI_FABRIC_TEXT_CAPACITY];
    char operation[UMI_FABRIC_ID_CAPACITY];
    char value[UMI_FABRIC_TEXT_CAPACITY];
} UmiFabricFilterExpression;

UmiStatus umi_fabric_filter_expression_init(UmiFabricFilterExpression *item, const char *field, const char *operation, const char *value);
UmiStatus umi_fabric_filter_expression_validate(const UmiFabricFilterExpression *item);

#ifdef __cplusplus
}
#endif
#endif
