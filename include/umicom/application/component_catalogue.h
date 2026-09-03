/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/component_catalogue.h
 *
 * PURPOSE:
 *   Catalogue reusable cross-application surfaces by domain. These definitions
 *   belong to Framework; applications select and arrange them without copying
 *   their models, commands, policies or rendering contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_COMPONENT_CATALOGUE_H
#define UMICOM_APPLICATION_COMPONENT_CATALOGUE_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/runtime/capability_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Roles describe how a reusable component participates in a workspace. */
typedef enum UmiApplicationComponentRole {
  UMI_APPLICATION_COMPONENT_VIEW = 1,
  UMI_APPLICATION_COMPONENT_EDITOR = 2,
  UMI_APPLICATION_COMPONENT_TOOL = 3,
  UMI_APPLICATION_COMPONENT_DASHBOARD = 4,
  UMI_APPLICATION_COMPONENT_INSPECTOR = 5,
  UMI_APPLICATION_COMPONENT_SERVICE_SURFACE = 6
} UmiApplicationComponentRole;

/* Component definitions are immutable borrowed catalogue records. Applications
 * select them by stable ID and provide only specialist adapters or policies. */
typedef struct UmiApplicationComponentDefinition {
  const char *component_id;       /* Stable ID used by recipes and layouts. */
  const char *domain_id;          /* Reusable subject area such as editor or data. */
  const char *title;              /* Human-readable component name. */
  const char *capability_id;      /* Capability required before activation. */
  const char *description;        /* User outcome provided by the component. */
  UmiApplicationComponentRole role; /* View, editor, tool or service role. */
  UmiCapabilityMaturity maturity; /* Truthful implementation maturity. */
  int multi_instance;             /* Non-zero permits multiple independent copies. */
  int context_aware;              /* Non-zero permits linked-selection updates. */
  const char *search_terms;       /* Plain-language aliases used by menus and catalogues. */
} UmiApplicationComponentDefinition;

/* Return the total number of reusable Framework component definitions. */
size_t umi_application_component_catalogue_count(void);
/* Borrow a component by position, or return NULL outside the catalogue. */
const UmiApplicationComponentDefinition *umi_application_component_catalogue_at(size_t index);
/* Borrow a component by stable ID, or return NULL when unknown. */
const UmiApplicationComponentDefinition *
umi_application_component_catalogue_find(const char *component_id);
/* Count components that require one capability. */
size_t umi_application_component_capability_count(const char *capability_id);
/* Borrow the requested component within a capability-filtered view. */
const UmiApplicationComponentDefinition *
umi_application_component_capability_at(const char *capability_id, size_t capability_index);
/* Count components published for one reusable domain. */
size_t umi_application_component_domain_count(const char *domain_id);
/* Borrow the requested component within a domain-filtered view. */
const UmiApplicationComponentDefinition *umi_application_component_domain_at(const char *domain_id,
                                                                             size_t domain_index);
/* Count components whose identity, title, description or aliases contain every query word. */
size_t umi_application_component_search_count(const char *query);
/* Borrow one result from the deterministic catalogue-order search projection. */
const UmiApplicationComponentDefinition *
umi_application_component_search_at(const char *query, size_t result_index);
/* Return non-zero when the domain or a supported domain alias exists. */
int umi_application_component_domain_exists(const char *domain_id);
/* Return non-zero when callers supplied a historical domain alias. */
int umi_application_component_domain_is_alias(const char *domain_id);
/* Return the borrowed capability ID associated with a domain or alias. */
const char *umi_application_component_domain_capability_id(const char *domain_id);
/* Count components by their workspace role. */
size_t umi_application_component_role_count(UmiApplicationComponentRole role);
/* Count components by truthful capability maturity. */
size_t umi_application_component_maturity_count(UmiCapabilityMaturity maturity);
/* Validate IDs, text, role, maturity and capability of one borrowed definition. */
UmiStatus
umi_application_component_definition_validate(const UmiApplicationComponentDefinition *definition);
/* Validate the complete catalogue and reject duplicates or malformed records. */
UmiStatus umi_application_component_catalogue_validate(void);
/* Return immutable role text for diagnostics and generated documentation. */
const char *umi_application_component_role_text(UmiApplicationComponentRole role);

#ifdef __cplusplus
}
#endif

#endif
