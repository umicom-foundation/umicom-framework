/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/persistence_equivalence.h
 *
 * PURPOSE:
 *   persistence-field parity and schema-compatibility scoring.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiFcPersistenceEquivalence { uint64_t required; uint64_t persisted; uint32_t expected_schema; uint32_t actual_schema; } UmiFcPersistenceEquivalence;
double umi_fc_persistence_equivalence_evaluate(const UmiFcPersistenceEquivalence *item);
bool umi_fc_persistence_equivalence_schema_compatible(const UmiFcPersistenceEquivalence *item);

#ifdef __cplusplus
}
#endif
#endif
