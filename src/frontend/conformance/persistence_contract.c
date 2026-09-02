/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/persistence_contract.c
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
#include "umicom/frontend/conformance/persistence_contract.h"

/*
 * Check that fc persistence contract satisfies its contract before another service relies
 * on it.
 */
bool umi_fc_persistence_contract_validate(const UmiFcPersistenceContract *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return false;return item->required_fields!=0U && item->schema_version>0U;}
