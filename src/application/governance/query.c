/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/governance/query.c
 *
 * PURPOSE:
 *   Implement bounded, allocation-free component discovery for Studio,
 *   diagnostics, native tools and future application composition workflows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/governance/query.h"

#include <ctype.h>
#include <string.h>

/* Provide the contains text operation used by this module and its client applications. */
static int contains_text(const char *value, const char *needle) {
  size_t value_index;
  const size_t needle_length = needle != NULL ? strlen(needle) : 0U;

  /* Apply this branch only when its contract condition is satisfied. */
  if (needle_length == 0U)
    return 1;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (value == NULL)
    return 0;
  /* Visit each bounded item once so every record receives the same rule. */
  for (value_index = 0U; value[value_index] != '\0'; ++value_index) {
    size_t needle_index = 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (needle_index < needle_length && value[value_index + needle_index] != '\0' &&
           tolower((unsigned char)value[value_index + needle_index]) ==
               tolower((unsigned char)needle[needle_index])) {
      needle_index += 1U;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (needle_index == needle_length)
      return 1;
  }
  return 0;
}

/* Provide the record matches operation used by this module and its client applications. */
static int record_matches(const UmiComponentGovernanceRecord *record,
                          const UmiComponentQuery *query) {
  const UmiApplicationComponentDefinition *definition = record->definition;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (query->domain_id != NULL && query->domain_id[0] != '\0' &&
      strcmp(query->domain_id, definition->domain_id) != 0)
    return 0;
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (query->status_mask != 0U &&
      (query->status_mask & umi_component_api_status_mask(record->api_status)) == 0U)
    return 0;
  /* Apply this branch only when its contract condition is satisfied. */
  if (query->role_mask != 0U &&
      (query->role_mask & umi_component_role_mask(definition->role)) == 0U)
    return 0;
  /* Apply this branch only when its contract condition is satisfied. */
  if (query->maturity_mask != 0U &&
      (query->maturity_mask & umi_component_maturity_mask(definition->maturity)) == 0U)
    return 0;
  /* Apply this branch only when its contract condition is satisfied. */
  if ((query->required_frontends & ~record->frontend_support) != 0U)
    return 0;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (query->evidence_complete >= 0 &&
      umi_component_evidence_is_complete(record) != (query->evidence_complete != 0))
    return 0;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (query->search_text != NULL && query->search_text[0] != '\0' &&
      !contains_text(definition->component_id, query->search_text) &&
      !contains_text(definition->title, query->search_text) &&
      !contains_text(definition->description, query->search_text) &&
      !contains_text(definition->capability_id, query->search_text))
    return 0;
  return 1;
}

/*
 * Initialise component query from caller-provided values so later operations receive a
 * known state.
 */
void umi_component_query_init(UmiComponentQuery *query) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (query == NULL)
    return;
  (void)memset(query, 0, sizeof(*query));
  query->evidence_complete = -1;
}

/*
 * Provide the component inventory query operation used by this module and its client
 * applications.
 */
UmiStatus umi_component_inventory_query(const UmiComponentInventory *inventory,
                                        const UmiComponentQuery *query,
                                        UmiComponentQueryResult *out_result) {
  size_t index;
  const uint32_t known_status_mask = umi_component_api_status_mask(UMI_COMPONENT_API_PLANNED) |
                                     umi_component_api_status_mask(UMI_COMPONENT_API_EXPERIMENTAL) |
                                     umi_component_api_status_mask(UMI_COMPONENT_API_CANDIDATE) |
                                     umi_component_api_status_mask(UMI_COMPONENT_API_STABLE) |
                                     umi_component_api_status_mask(UMI_COMPONENT_API_DEPRECATED);
  const uint32_t known_role_mask =
      umi_component_role_mask(UMI_APPLICATION_COMPONENT_VIEW) |
      umi_component_role_mask(UMI_APPLICATION_COMPONENT_EDITOR) |
      umi_component_role_mask(UMI_APPLICATION_COMPONENT_TOOL) |
      umi_component_role_mask(UMI_APPLICATION_COMPONENT_DASHBOARD) |
      umi_component_role_mask(UMI_APPLICATION_COMPONENT_INSPECTOR) |
      umi_component_role_mask(UMI_APPLICATION_COMPONENT_SERVICE_SURFACE);
  const uint32_t known_maturity_mask = umi_component_maturity_mask(UMI_CAPABILITY_IMPLEMENTED) |
                                       umi_component_maturity_mask(UMI_CAPABILITY_FOUNDATION) |
                                       umi_component_maturity_mask(UMI_CAPABILITY_PLANNED);

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (inventory == NULL || query == NULL || out_result == NULL ||
      umi_component_inventory_validate(inventory) != UMI_STATUS_OK ||
      (query->status_mask & ~known_status_mask) != 0U ||
      (query->role_mask & ~known_role_mask) != 0U ||
      (query->maturity_mask & ~known_maturity_mask) != 0U ||
      (query->required_frontends & ~umi_component_frontend_known_mask()) != 0U ||
      query->evidence_complete < -1 || query->evidence_complete > 1) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  (void)memset(out_result, 0, sizeof(*out_result));
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < inventory->component_count; ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (record_matches(&inventory->records[index], query)) {
      out_result->indexes[out_result->count++] = index;
    }
  }
  return UMI_STATUS_OK;
}

/*
 * Find component query result while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiComponentGovernanceRecord *
umi_component_query_result_at(const UmiComponentInventory *inventory,
                              const UmiComponentQueryResult *result, size_t index) {
  return inventory != NULL && result != NULL && index < result->count
             ? umi_component_inventory_at(inventory, result->indexes[index])
             : NULL;
}
