/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/persistence_equivalence.c
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
#include "umicom/frontend/conformance/persistence_equivalence.h"

/*
 * Provide the fc persistence equivalence evaluate operation used by this module and its
 * client applications.
 */
double umi_fc_persistence_equivalence_evaluate(const UmiFcPersistenceEquivalence *item){unsigned req,hit;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0.0;req=(unsigned)__builtin_popcountll(item->required);hit=(unsigned)__builtin_popcountll(item->required&item->persisted);return umi_fc_ratio((size_t)hit,(size_t)req);}
/*
 * Provide the fc persistence equivalence schema compatible operation used by this module
 * and its client applications.
 */
bool umi_fc_persistence_equivalence_schema_compatible(const UmiFcPersistenceEquivalence *item){return item!=NULL&&item->expected_schema==item->actual_schema;}
