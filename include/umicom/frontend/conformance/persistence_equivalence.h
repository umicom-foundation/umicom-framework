/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/persistence_equivalence.h
 *
 * PURPOSE:
 *   persistence-field parity and schema-compatibility scoring.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_PERSISTENCE_EQUIVALENCE_H
#define UMICOM_FRONTEND_CONFORMANCE_PERSISTENCE_EQUIVALENCE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc persistence equivalence data shared with callers of this public
 * contract.
 */
typedef struct UmiFcPersistenceEquivalence { uint64_t required; uint64_t persisted; uint32_t expected_schema; uint32_t actual_schema; } UmiFcPersistenceEquivalence;
/**
 * Provide the fc persistence equivalence evaluate operation used by this module and its
 * client applications.
 */
double umi_fc_persistence_equivalence_evaluate(const UmiFcPersistenceEquivalence *item);
/**
 * Provide the fc persistence equivalence schema compatible operation used by this module
 * and its client applications.
 */
bool umi_fc_persistence_equivalence_schema_compatible(const UmiFcPersistenceEquivalence *item);

#ifdef __cplusplus
}
#endif
#endif
