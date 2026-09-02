/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/governance/inventory.h
 *
 * PURPOSE:
 *   Build a deterministic governance inventory over every reusable application
 *   component and expose portfolio-wide and per-domain maturity metrics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_GOVERNANCE_INVENTORY_H
#define UMICOM_APPLICATION_GOVERNANCE_INVENTORY_H

#include "umicom/application/governance/policy.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the component domain inventory data shared with callers of this public
 * contract.
 */
typedef struct UmiComponentDomainInventory {
  const char *domain_id;
  size_t component_count;
  size_t status_count[UMI_COMPONENT_API_DEPRECATED + 1U];
  size_t role_count[UMI_APPLICATION_COMPONENT_SERVICE_SURFACE + 1U];
  size_t evidence_complete_count;
  size_t evidence_incomplete_count;
} UmiComponentDomainInventory;

/**
 * Represent the component inventory data shared with callers of this public contract.
 */
typedef struct UmiComponentInventory {
  UmiComponentGovernanceRecord records[UMI_COMPONENT_GOVERNANCE_MAX_COMPONENTS];
  UmiComponentDomainInventory domains[UMI_COMPONENT_GOVERNANCE_MAX_DOMAINS];
  size_t component_count;
  size_t domain_count;
  size_t status_count[UMI_COMPONENT_API_DEPRECATED + 1U];
  size_t role_count[UMI_APPLICATION_COMPONENT_SERVICE_SURFACE + 1U];
  size_t maturity_count[UMI_CAPABILITY_PLANNED + 1U];
  size_t evidence_complete_count;
  size_t evidence_incomplete_count;
} UmiComponentInventory;

/**
 * Provide the component inventory build operation used by this module and its client
 * applications.
 */
UmiStatus umi_component_inventory_build(const UmiComponentGovernanceOverride *overrides,
                                        size_t override_count,
                                        UmiComponentInventory *out_inventory);
/**
 * Check that component inventory satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_component_inventory_validate(const UmiComponentInventory *inventory);
/**
 * Find component inventory while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiComponentGovernanceRecord *
umi_component_inventory_at(const UmiComponentInventory *inventory, size_t index);
/**
 * Find component inventory while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiComponentGovernanceRecord *
umi_component_inventory_find(const UmiComponentInventory *inventory, const char *component_id);
/**
 * Find component inventory domain while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiComponentDomainInventory *
umi_component_inventory_domain_at(const UmiComponentInventory *inventory, size_t index);
/**
 * Find component inventory domain while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiComponentDomainInventory *
umi_component_inventory_domain_find(const UmiComponentInventory *inventory, const char *domain_id);

#ifdef __cplusplus
}
#endif

#endif
