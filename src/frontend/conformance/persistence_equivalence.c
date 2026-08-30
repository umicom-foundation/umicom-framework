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

double umi_fc_persistence_equivalence_evaluate(const UmiFcPersistenceEquivalence *item){unsigned req,hit;if(item==NULL)return 0.0;req=(unsigned)__builtin_popcountll(item->required);hit=(unsigned)__builtin_popcountll(item->required&item->persisted);return umi_fc_ratio((size_t)hit,(size_t)req);}
bool umi_fc_persistence_equivalence_schema_compatible(const UmiFcPersistenceEquivalence *item){return item!=NULL&&item->expected_schema==item->actual_schema;}
