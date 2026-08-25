/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/persistence_contract.c
 *
 * PURPOSE:
 *   layout, focus, panel, geometry and context state persistence requirements.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/persistence_contract.h"

bool umi_fc_persistence_contract_validate(const UmiFcPersistenceContract *item){if(item==NULL)return false;return item->required_fields!=0U && item->schema_version>0U;}
