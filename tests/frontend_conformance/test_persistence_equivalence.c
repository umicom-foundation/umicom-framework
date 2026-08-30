/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_persistence_equivalence.c
 *
 * PURPOSE:
 *   Focused regression coverage for persistence-field parity and schema-compatibility scoring.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/persistence_equivalence.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcPersistenceEquivalence e={7U,7U,2U,2U}; CHECK(umi_fc_persistence_equivalence_evaluate(&e)==1.0); CHECK(umi_fc_persistence_equivalence_schema_compatible(&e));
    return 0;
}
