/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/governance/query.h
 *
 * PURPOSE:
 *   Query the component inventory by domain, role, maturity, lifecycle,
 *   frontend support, evidence completeness and human-readable search text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_GOVERNANCE_QUERY_H
#define UMICOM_APPLICATION_GOVERNANCE_QUERY_H

#include "umicom/application/governance/inventory.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiComponentQuery {
  const char *domain_id;
  const char *search_text;
  uint32_t status_mask;
  uint32_t role_mask;
  uint32_t maturity_mask;
  uint32_t required_frontends;
  int evidence_complete;
} UmiComponentQuery;

typedef struct UmiComponentQueryResult {
  size_t indexes[UMI_COMPONENT_GOVERNANCE_MAX_COMPONENTS];
  size_t count;
} UmiComponentQueryResult;

void umi_component_query_init(UmiComponentQuery *query);
UmiStatus umi_component_inventory_query(const UmiComponentInventory *inventory,
                                        const UmiComponentQuery *query,
                                        UmiComponentQueryResult *out_result);
const UmiComponentGovernanceRecord *
umi_component_query_result_at(const UmiComponentInventory *inventory,
                              const UmiComponentQueryResult *result, size_t index);

#ifdef __cplusplus
}
#endif

#endif
