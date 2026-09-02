/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/governance/inventory.c
 *
 * PURPOSE:
 *   Project governance policy over all existing catalogue components without
 *   duplicating or modifying their definitions and aggregate actionable gaps.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/governance/inventory.h"

#include <string.h>

/*
 * Provide the validate overrides operation used by this module and its client
 * applications.
 */
static UmiStatus validate_overrides(const UmiComponentGovernanceOverride *overrides,
                                    size_t override_count) {
  size_t index;

  /* Apply this branch only when its contract condition is satisfied. */
  if (override_count > UMI_COMPONENT_GOVERNANCE_MAX_COMPONENTS ||
      (override_count > 0U && overrides == NULL)) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < override_count; ++index) {
    size_t previous;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (overrides[index].component_id == NULL ||
        umi_application_component_catalogue_find(overrides[index].component_id) == NULL) {
      return UMI_STATUS_NOT_FOUND;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (previous = 0U; previous < index; ++previous) {
      /* Keep the operation inside its valid bounds before reading, writing or adding data. */
      if (strcmp(overrides[previous].component_id, overrides[index].component_id) == 0) {
        return UMI_STATUS_ALREADY_EXISTS;
      }
    }
  }
  return UMI_STATUS_OK;
}

/* Provide the find override operation used by this module and its client applications. */
static const UmiComponentGovernanceOverride *
find_override(const UmiComponentGovernanceOverride *overrides, size_t override_count,
              const char *component_id) {
  size_t index;

  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < override_count; ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(overrides[index].component_id, component_id) == 0) {
      return &overrides[index];
    }
  }
  return NULL;
}

/*
 * Provide the find or add domain operation used by this module and its client
 * applications.
 */
static UmiComponentDomainInventory *find_or_add_domain(UmiComponentInventory *inventory,
                                                       const char *domain_id) {
  size_t index;

  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < inventory->domain_count; ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(inventory->domains[index].domain_id, domain_id) == 0) {
      return &inventory->domains[index];
    }
  }
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (inventory->domain_count >= UMI_COMPONENT_GOVERNANCE_MAX_DOMAINS) {
    return NULL;
  }
  inventory->domains[inventory->domain_count].domain_id = domain_id;
  return &inventory->domains[inventory->domain_count++];
}

/* Provide the add metrics operation used by this module and its client applications. */
static UmiStatus add_metrics(UmiComponentInventory *inventory,
                             const UmiComponentGovernanceRecord *record) {
  UmiComponentDomainInventory *domain;
  const UmiApplicationComponentDefinition *definition = record->definition;
  const int complete = umi_component_evidence_is_complete(record);

  domain = find_or_add_domain(inventory, definition->domain_id);
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (domain == NULL)
    return UMI_STATUS_CAPACITY_EXCEEDED;

  inventory->status_count[record->api_status] += 1U;
  inventory->role_count[definition->role] += 1U;
  inventory->maturity_count[definition->maturity] += 1U;
  domain->component_count += 1U;
  domain->status_count[record->api_status] += 1U;
  domain->role_count[definition->role] += 1U;
  /* Apply this branch only when its contract condition is satisfied. */
  if (complete) {
    inventory->evidence_complete_count += 1U;
    domain->evidence_complete_count += 1U;
  } /* Use this fallback path when the earlier condition does not apply. */ else {
    inventory->evidence_incomplete_count += 1U;
    domain->evidence_incomplete_count += 1U;
  }
  return UMI_STATUS_OK;
}

/*
 * Provide the component inventory build operation used by this module and its client
 * applications.
 */
UmiStatus umi_component_inventory_build(const UmiComponentGovernanceOverride *overrides,
                                        size_t override_count,
                                        UmiComponentInventory *out_inventory) {
  size_t index;
  const size_t component_count = umi_application_component_catalogue_count();
  UmiStatus status;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (out_inventory == NULL || component_count > UMI_COMPONENT_GOVERNANCE_MAX_COMPONENTS) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  status = umi_application_component_catalogue_validate();
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    return status;
  status = validate_overrides(overrides, override_count);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    return status;

  (void)memset(out_inventory, 0, sizeof(*out_inventory));
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < component_count; ++index) {
    const UmiApplicationComponentDefinition *definition =
        umi_application_component_catalogue_at(index);
    const UmiComponentGovernanceOverride *override_record;
    UmiComponentGovernanceRecord *record = &out_inventory->records[out_inventory->component_count];

    status = umi_component_governance_default(definition, record);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK)
      return status;
    override_record = find_override(overrides, override_count, definition->component_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (override_record != NULL) {
      status = umi_component_governance_apply_override(record, override_record);
      /* Preserve the original failure result so the caller can respond to the correct cause. */
      if (status != UMI_STATUS_OK)
        return status;
    }
    status = add_metrics(out_inventory, record);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK)
      return status;
    out_inventory->component_count += 1U;
  }
  return umi_component_inventory_validate(out_inventory);
}

/*
 * Check that component inventory satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_component_inventory_validate(const UmiComponentInventory *inventory) {
  size_t index;
  size_t status_total = 0U;
  size_t domain_total = 0U;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (inventory == NULL || inventory->component_count == 0U ||
      inventory->component_count > UMI_COMPONENT_GOVERNANCE_MAX_COMPONENTS ||
      inventory->domain_count == 0U ||
      inventory->domain_count > UMI_COMPONENT_GOVERNANCE_MAX_DOMAINS) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < inventory->component_count; ++index) {
    const UmiComponentGovernanceRecord *record = &inventory->records[index];
    size_t previous;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record->definition == NULL || record->introduced_version == NULL ||
        record->introduced_version[0] == '\0' ||
        umi_application_component_definition_validate(record->definition) != UMI_STATUS_OK ||
        umi_component_api_status_mask(record->api_status) == 0U ||
        record->owner < UMI_COMPONENT_OWNER_FRAMEWORK ||
        record->owner > UMI_COMPONENT_OWNER_EXTERNAL_ADAPTER ||
        (record->required_evidence & ~umi_component_evidence_known_mask()) != 0U ||
        (record->available_evidence & ~umi_component_evidence_known_mask()) != 0U ||
        (record->frontend_support & ~umi_component_frontend_known_mask()) != 0U ||
        record->required_evidence !=
            umi_component_evidence_required_for_status(record->api_status)) {
      return UMI_STATUS_INVALID_STATE;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record->replacement_component_id != NULL &&
        (strcmp(record->replacement_component_id, record->definition->component_id) == 0 ||
         umi_application_component_catalogue_find(record->replacement_component_id) == NULL)) {
      return UMI_STATUS_INVALID_STATE;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (previous = 0U; previous < index; ++previous) {
      /* Use the stable identifier comparison to choose the matching record or policy. */
      if (strcmp(inventory->records[previous].definition->component_id,
                 record->definition->component_id) == 0) {
        return UMI_STATUS_ALREADY_EXISTS;
      }
    }
  }
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = UMI_COMPONENT_API_PLANNED; index <= UMI_COMPONENT_API_DEPRECATED; ++index) {
    status_total += inventory->status_count[index];
  }
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < inventory->domain_count; ++index) {
    domain_total += inventory->domains[index].component_count;
  }
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status_total != inventory->component_count || domain_total != inventory->component_count ||
      inventory->evidence_complete_count + inventory->evidence_incomplete_count !=
          inventory->component_count) {
    return UMI_STATUS_INVALID_STATE;
  }
  return UMI_STATUS_OK;
}

/*
 * Find component inventory while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiComponentGovernanceRecord *
umi_component_inventory_at(const UmiComponentInventory *inventory, size_t index) {
  return inventory != NULL && index < inventory->component_count ? &inventory->records[index]
                                                                 : NULL;
}

/*
 * Find component inventory while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiComponentGovernanceRecord *
umi_component_inventory_find(const UmiComponentInventory *inventory, const char *component_id) {
  size_t index;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (inventory == NULL || component_id == NULL)
    return NULL;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < inventory->component_count; ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(inventory->records[index].definition->component_id, component_id) == 0) {
      return &inventory->records[index];
    }
  }
  return NULL;
}

/*
 * Find component inventory domain while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiComponentDomainInventory *
umi_component_inventory_domain_at(const UmiComponentInventory *inventory, size_t index) {
  return inventory != NULL && index < inventory->domain_count ? &inventory->domains[index] : NULL;
}

/*
 * Find component inventory domain while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiComponentDomainInventory *
umi_component_inventory_domain_find(const UmiComponentInventory *inventory, const char *domain_id) {
  size_t index;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (inventory == NULL || domain_id == NULL)
    return NULL;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < inventory->domain_count; ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(inventory->domains[index].domain_id, domain_id) == 0) {
      return &inventory->domains[index];
    }
  }
  return NULL;
}
