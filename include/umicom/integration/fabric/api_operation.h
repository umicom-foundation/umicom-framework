/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/api_operation.h
 *
 * PURPOSE:
 *   Describe one API operation including method, route, schemas and idempotency semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_API_OPERATION_H
#define UMICOM_INTEGRATION_FABRIC_API_OPERATION_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric api operation data shared with callers of this public contract.
 */
typedef struct UmiFabricApiOperation {
    char operation_id[UMI_FABRIC_ID_CAPACITY];
    char method[UMI_FABRIC_ID_CAPACITY];
    char path[UMI_FABRIC_URI_CAPACITY];
    char request_schema[UMI_FABRIC_ID_CAPACITY];
    char response_schema[UMI_FABRIC_ID_CAPACITY];
    bool idempotent;
} UmiFabricApiOperation;

/**
 * Initialise fabric api operation from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_fabric_api_operation_init(UmiFabricApiOperation *item, const char *operation_id, const char *method, const char *path, const char *request_schema, const char *response_schema, bool idempotent);
/**
 * Check that fabric api operation satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_fabric_api_operation_validate(const UmiFabricApiOperation *item);

#ifdef __cplusplus
}
#endif
#endif
