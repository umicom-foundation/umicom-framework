/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/contract.c
 *
 * PURPOSE:
 *   Project catalogue definitions into stable, versioned and frontend-neutral
 *   contracts that factories and application layouts can share.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/contract.h"

#include <string.h>

/* Provide the factory for role operation used by this module and its client applications. */
static const char *factory_for_role(UmiApplicationComponentRole role) {
  /* Select the behaviour associated with the requested command or state value. */
  switch (role) {
  case UMI_APPLICATION_COMPONENT_VIEW:
    return "umicom.application.factory.view";
  case UMI_APPLICATION_COMPONENT_EDITOR:
    return "umicom.application.factory.editor";
  case UMI_APPLICATION_COMPONENT_TOOL:
    return "umicom.application.factory.tool";
  case UMI_APPLICATION_COMPONENT_DASHBOARD:
    return "umicom.application.factory.dashboard";
  case UMI_APPLICATION_COMPONENT_INSPECTOR:
    return "umicom.application.factory.inspector";
  case UMI_APPLICATION_COMPONENT_SERVICE_SURFACE:
    return "umicom.application.factory.service-surface";
  default:
    return NULL;
  }
}

/* Provide the region for role operation used by this module and its client applications. */
static UmiApplicationComponentRegion
region_for_role(UmiApplicationComponentRole role) {
  /* Select the behaviour associated with the requested command or state value. */
  switch (role) {
  case UMI_APPLICATION_COMPONENT_VIEW:
    return UMI_APPLICATION_COMPONENT_REGION_LEFT;
  case UMI_APPLICATION_COMPONENT_TOOL:
    return UMI_APPLICATION_COMPONENT_REGION_BOTTOM;
  case UMI_APPLICATION_COMPONENT_INSPECTOR:
    return UMI_APPLICATION_COMPONENT_REGION_RIGHT;
  default:
    return UMI_APPLICATION_COMPONENT_REGION_PRIMARY;
  }
}

/*
 * Provide the application component contract from definition operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_component_contract_from_definition(
    const UmiApplicationComponentDefinition *definition,
    UmiApplicationComponentContract *out_contract) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (definition == NULL || out_contract == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (umi_application_component_definition_validate(definition) != UMI_STATUS_OK)
    return UMI_STATUS_INVALID_STATE;

  (void)memset(out_contract, 0, sizeof(*out_contract));
  out_contract->struct_size = (uint32_t)sizeof(*out_contract);
  out_contract->api_version = UMI_APPLICATION_COMPONENT_MODEL_API_VERSION;
  out_contract->definition = definition;
  out_contract->version.major = 1U;
  out_contract->factory_id = factory_for_role(definition->role);
  out_contract->frontend_mask = UMI_APPLICATION_COMPONENT_FRONTEND_HEADLESS |
                                UMI_APPLICATION_COMPONENT_FRONTEND_GTK4 |
                                UMI_APPLICATION_COMPONENT_FRONTEND_QT6 |
                                UMI_APPLICATION_COMPONENT_FRONTEND_WEB;
  out_contract->default_region = region_for_role(definition->role);
  out_contract->minimum_width =
      definition->role == UMI_APPLICATION_COMPONENT_TOOL ? 320U : 480U;
  out_contract->minimum_height =
      definition->role == UMI_APPLICATION_COMPONENT_TOOL ? 160U : 320U;
  return umi_application_component_contract_validate(out_contract);
}

/*
 * Check that application component contract satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_application_component_contract_validate(
    const UmiApplicationComponentContract *contract) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (contract == NULL || contract->definition == NULL ||
      contract->factory_id == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Apply this branch only when its contract condition is satisfied. */
  if (contract->struct_size < sizeof(*contract) ||
      contract->api_version != UMI_APPLICATION_COMPONENT_MODEL_API_VERSION ||
      contract->version.major == 0U || contract->factory_id[0] == '\0' ||
      contract->frontend_mask == 0U || contract->minimum_width == 0U ||
      contract->minimum_height == 0U ||
      contract->default_region < UMI_APPLICATION_COMPONENT_REGION_PRIMARY ||
      contract->default_region > UMI_APPLICATION_COMPONENT_REGION_FLOATING)
    return UMI_STATUS_INVALID_STATE;
  return umi_application_component_definition_validate(contract->definition);
}

/*
 * Provide the application component contract supports operation used by this module and
 * its client applications.
 */
int umi_application_component_contract_supports(
    const UmiApplicationComponentContract *contract,
    UmiApplicationComponentFrontend frontend) {
  return contract != NULL &&
         umi_application_component_frontend_valid(frontend) &&
         (contract->frontend_mask & (uint32_t)frontend) != 0U;
}

/*
 * Provide the application component version compatible operation used by this module and
 * its client applications.
 */
int umi_application_component_version_compatible(
    UmiApplicationComponentVersion available,
    UmiApplicationComponentVersion required) {
  /* Apply this branch only when its contract condition is satisfied. */
  if (available.major == 0U || available.major != required.major)
    return 0;
  /* Apply this branch only when its contract condition is satisfied. */
  if (available.minor != required.minor)
    return available.minor > required.minor;
  return available.patch >= required.patch;
}
