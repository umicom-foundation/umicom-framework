/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue.c
 *
 * PURPOSE:
 *   Define reusable professional surfaces for the complete Umicom portfolio.
 *   The catalogue expresses product vocabulary without embedding product code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component_catalogue.h"

#include <string.h>

#define COMP(id, domain, label, capability, detail, component_role, state, multi, context)         \
  {(id), (domain), (label), (capability), (detail), (component_role), (state), (multi), (context)}

static const UmiApplicationComponentDefinition COMPONENTS[] = {
#include "component_catalogue/records.inc"
};

typedef struct UmiApplicationComponentDomainAlias {
  const char *domain_id;
  const char *capability_id;
} UmiApplicationComponentDomainAlias;

/* Product profiles use business-domain language while the physical catalogue
 * groups several shared finance surfaces under one broad domain. These aliases
 * resolve product vocabulary through canonical capability ownership rather
 * than duplicating reusable component definitions. */
static const UmiApplicationComponentDomainAlias DOMAIN_ALIASES[] = {
    {"accounting", "umicom.accounting"}, {"banking", "umicom.banking"},
    {"commodity", "umicom.commodity"},   {"digital-assets", "umicom.digital-asset"},
    {"exchange", "umicom.exchange"},     {"logistics", "umicom.commodity"},
    {"payments", "umicom.payments"},     {"payroll", "umicom.payroll"}};

/* Provide the find domain alias operation used by this module and its client applications. */
static const UmiApplicationComponentDomainAlias *find_domain_alias(const char *domain_id) {
  size_t index;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (domain_id == NULL)
    return NULL;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < sizeof(DOMAIN_ALIASES) / sizeof(DOMAIN_ALIASES[0]); ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(DOMAIN_ALIASES[index].domain_id, domain_id) == 0)
      return &DOMAIN_ALIASES[index];
  }
  return NULL;
}

/*
 * Return the number of records represented by application component catalogue without
 * changing their state.
 */
size_t umi_application_component_catalogue_count(void) {
  return sizeof(COMPONENTS) / sizeof(COMPONENTS[0]);
}

/*
 * Find application component catalogue while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationComponentDefinition *umi_application_component_catalogue_at(size_t index) {
  return index < umi_application_component_catalogue_count() ? &COMPONENTS[index] : NULL;
}

/*
 * Find application component catalogue while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationComponentDefinition *
umi_application_component_catalogue_find(const char *component_id) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (component_id == NULL)
    return NULL;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_application_component_catalogue_count(); ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(COMPONENTS[index].component_id, component_id) == 0)
      return &COMPONENTS[index];
  }
  return NULL;
}

/*
 * Return the number of records represented by application component capability without
 * changing their state.
 */
size_t umi_application_component_capability_count(const char *capability_id) {
  size_t index;
  size_t count = 0U;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (capability_id == NULL)
    return 0U;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_application_component_catalogue_count(); ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(COMPONENTS[index].capability_id, capability_id) == 0)
      count += 1U;
  }
  return count;
}

/*
 * Find application component capability while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationComponentDefinition *
umi_application_component_capability_at(const char *capability_id, size_t capability_index) {
  size_t index;
  size_t match_index = 0U;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (capability_id == NULL)
    return NULL;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_application_component_catalogue_count(); ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(COMPONENTS[index].capability_id, capability_id) != 0)
      continue;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (match_index == capability_index)
      return &COMPONENTS[index];
    match_index += 1U;
  }
  return NULL;
}

/*
 * Return the number of records represented by application component domain without
 * changing their state.
 */
size_t umi_application_component_domain_count(const char *domain_id) {
  size_t index;
  size_t count = 0U;
  const UmiApplicationComponentDomainAlias *alias;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (domain_id == NULL)
    return 0U;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_application_component_catalogue_count(); ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(COMPONENTS[index].domain_id, domain_id) == 0)
      count += 1U;
  }
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (count > 0U)
    return count;
  alias = find_domain_alias(domain_id);
  return alias != NULL ? umi_application_component_capability_count(alias->capability_id) : 0U;
}

/*
 * Find application component domain while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiApplicationComponentDefinition *umi_application_component_domain_at(const char *domain_id,
                                                                             size_t domain_index) {
  size_t index;
  size_t match_index = 0U;
  const UmiApplicationComponentDomainAlias *alias;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (domain_id == NULL)
    return NULL;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_application_component_catalogue_count(); ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(COMPONENTS[index].domain_id, domain_id) != 0)
      continue;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (match_index == domain_index)
      return &COMPONENTS[index];
    match_index += 1U;
  }
  alias = find_domain_alias(domain_id);
  return alias != NULL ? umi_application_component_capability_at(alias->capability_id, domain_index)
                       : NULL;
}

/*
 * Provide the application component domain exists operation used by this module and its
 * client applications.
 */
int umi_application_component_domain_exists(const char *domain_id) {
  return umi_application_component_domain_count(domain_id) > 0U;
}

/*
 * Provide the application component domain is alias operation used by this module and its
 * client applications.
 */
int umi_application_component_domain_is_alias(const char *domain_id) {
  return find_domain_alias(domain_id) != NULL;
}

/*
 * Provide the application component domain capability id operation used by this module and
 * its client applications.
 */
const char *umi_application_component_domain_capability_id(const char *domain_id) {
  const UmiApplicationComponentDomainAlias *alias = find_domain_alias(domain_id);
  return alias != NULL ? alias->capability_id : NULL;
}

/*
 * Return the number of records represented by application component role without changing
 * their state.
 */
size_t umi_application_component_role_count(UmiApplicationComponentRole role) {
  size_t index;
  size_t count = 0U;

  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_application_component_catalogue_count(); ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (COMPONENTS[index].role == role)
      count += 1U;
  }
  return count;
}

/*
 * Return the number of records represented by application component maturity without
 * changing their state.
 */
size_t umi_application_component_maturity_count(UmiCapabilityMaturity maturity) {
  size_t index;
  size_t count = 0U;

  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_application_component_catalogue_count(); ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (COMPONENTS[index].maturity == maturity)
      count += 1U;
  }
  return count;
}

/*
 * Check that application component definition satisfies its contract before another
 * service relies on it.
 */
UmiStatus
umi_application_component_definition_validate(const UmiApplicationComponentDefinition *definition) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (definition == NULL || definition->component_id == NULL || definition->domain_id == NULL ||
      definition->title == NULL || definition->capability_id == NULL ||
      definition->description == NULL || definition->component_id[0] == '\0' ||
      definition->domain_id[0] == '\0' || definition->title[0] == '\0' ||
      definition->capability_id[0] == '\0' || definition->description[0] == '\0') {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  /* Use the stable identifier comparison to choose the matching record or policy. */
  if (strncmp(definition->component_id, "umicom.", 7U) != 0 ||
      strchr(definition->domain_id, '.') != NULL ||
      umi_framework_capability_catalogue_find(definition->capability_id) == NULL) {
    return UMI_STATUS_INVALID_STATE;
  }
  /* Apply this branch only when its contract condition is satisfied. */
  if (definition->role < UMI_APPLICATION_COMPONENT_VIEW ||
      definition->role > UMI_APPLICATION_COMPONENT_SERVICE_SURFACE ||
      definition->maturity < UMI_CAPABILITY_IMPLEMENTED ||
      definition->maturity > UMI_CAPABILITY_PLANNED ||
      (definition->multi_instance != 0 && definition->multi_instance != 1) ||
      (definition->context_aware != 0 && definition->context_aware != 1)) {
    return UMI_STATUS_INVALID_STATE;
  }
  return UMI_STATUS_OK;
}

/*
 * Check that application component catalogue satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_application_component_catalogue_validate(void) {
  size_t index;

  /* Validate the complete catalogue once so downstream tools can trust that
   * every projected component has a unique Framework-owned identity. */
  for (index = 0U; index < umi_application_component_catalogue_count(); ++index) {
    size_t previous;
    UmiStatus status = umi_application_component_definition_validate(&COMPONENTS[index]);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK)
      return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (previous = 0U; previous < index; ++previous) {
      /* Keep the operation inside its valid bounds before reading, writing or adding data. */
      if (strcmp(COMPONENTS[previous].component_id, COMPONENTS[index].component_id) == 0) {
        return UMI_STATUS_ALREADY_EXISTS;
      }
    }
  }
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < sizeof(DOMAIN_ALIASES) / sizeof(DOMAIN_ALIASES[0]); ++index) {
    size_t previous;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_framework_capability_catalogue_find(DOMAIN_ALIASES[index].capability_id) == NULL ||
        umi_application_component_capability_count(DOMAIN_ALIASES[index].capability_id) == 0U) {
      return UMI_STATUS_INVALID_STATE;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (previous = 0U; previous < index; ++previous) {
      /* Keep the operation inside its valid bounds before reading, writing or adding data. */
      if (strcmp(DOMAIN_ALIASES[previous].domain_id, DOMAIN_ALIASES[index].domain_id) == 0) {
        return UMI_STATUS_ALREADY_EXISTS;
      }
    }
  }
  return UMI_STATUS_OK;
}

/*
 * Provide the application component role text operation used by this module and its client
 * applications.
 */
const char *umi_application_component_role_text(UmiApplicationComponentRole role) {
  /* Select the behaviour associated with the requested command or state value. */
  switch (role) {
  case UMI_APPLICATION_COMPONENT_VIEW:
    return "view";
  case UMI_APPLICATION_COMPONENT_EDITOR:
    return "editor";
  case UMI_APPLICATION_COMPONENT_TOOL:
    return "tool";
  case UMI_APPLICATION_COMPONENT_DASHBOARD:
    return "dashboard";
  case UMI_APPLICATION_COMPONENT_INSPECTOR:
    return "inspector";
  case UMI_APPLICATION_COMPONENT_SERVICE_SURFACE:
    return "service-surface";
  default:
    return "unknown";
  }
}
