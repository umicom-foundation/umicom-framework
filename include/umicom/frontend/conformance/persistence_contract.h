/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/persistence_contract.h
 *
 * PURPOSE:
 *   layout, focus, panel, geometry and context state persistence requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_PERSISTENCE_CONTRACT_H
#define UMICOM_FRONTEND_CONFORMANCE_PERSISTENCE_CONTRACT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcPersistenceContract { uint64_t required_fields; uint32_t schema_version; bool forward_readable; } UmiFcPersistenceContract;
bool umi_fc_persistence_contract_validate(const UmiFcPersistenceContract *item);

#ifdef __cplusplus
}
#endif
#endif
