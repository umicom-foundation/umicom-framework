/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_application_component_catalogue.c
 *
 * PURPOSE:
 *   Verify all strategic product domains are represented by reusable Framework
 *   components rather than application-private surface definitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application/application.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  const char *domains[] = {"shell",    "development", "trading",    "treasury", "media",
                           "music",    "creator",     "ai",         "rag",      "llm",
                           "games",    "cad",         "kitchen",    "author",   "web",
                           "database", "integration", "operations", "os",       "education"};
  size_t index;
  size_t trading_count;
  size_t role_total = 0U;
  size_t maturity_total = 0U;
  assert(umi_application_component_catalogue_count() >= 90U);
  assert(umi_application_component_catalogue_validate() == UMI_STATUS_OK);
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_application_component_catalogue_count(); ++index) {
    const UmiApplicationComponentDefinition *definition =
        umi_application_component_catalogue_at(index);
    assert(definition != NULL);
    assert(definition->search_terms != NULL);
    assert(definition->search_terms[0] != '\0');
    assert(umi_application_component_definition_validate(definition) == UMI_STATUS_OK);
    assert(umi_application_component_catalogue_find(definition->component_id) == definition);
    assert(umi_application_component_domain_at(definition->domain_id, 0U) != NULL);
  }
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < sizeof(domains) / sizeof(domains[0]); ++index)
    assert(umi_application_component_domain_exists(domains[index]));
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = UMI_APPLICATION_COMPONENT_VIEW; index <= UMI_APPLICATION_COMPONENT_SERVICE_SURFACE;
       ++index)
    role_total += umi_application_component_role_count((UmiApplicationComponentRole)index);
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = UMI_CAPABILITY_IMPLEMENTED; index <= UMI_CAPABILITY_PLANNED; ++index)
    maturity_total += umi_application_component_maturity_count((UmiCapabilityMaturity)index);
  assert(role_total == umi_application_component_catalogue_count());
  assert(maturity_total == umi_application_component_catalogue_count());
  assert(umi_application_component_catalogue_find("umicom.shell.context-links") != NULL);
  assert(umi_application_component_catalogue_find("umicom.trading.strategy") != NULL);
  assert(umi_application_component_catalogue_find("umicom.treasury.settlement") != NULL);
  assert(umi_application_component_catalogue_find("umicom.media.timeline") != NULL);
  assert(umi_application_component_catalogue_find("umicom.cad.viewport") != NULL);
  /* Search accepts plain-language words and returns canonical borrowed records,
   * allowing one catalogue to power menus, palettes and assistant tools. */
  assert(umi_application_component_search_count("trading chart") >= 1U);
  assert(umi_application_component_search_at("trading chart", 0U) != NULL);
  assert(umi_application_component_search_count("words that cannot match") == 0U);
  assert(umi_application_component_search_at("words that cannot match", 0U) == NULL);
  assert(umi_application_component_search_count(NULL) == 0U);
  /* The final valid record and the first out-of-range record define the
   * catalogue boundary without freezing the test at an old component count. */
  trading_count = umi_application_component_domain_count("trading");
  assert(trading_count > 0U);
  assert(umi_application_component_domain_at(
             "trading", trading_count - 1U) != NULL);
  assert(umi_application_component_domain_at("trading", trading_count) == NULL);
  assert(umi_application_component_domain_is_alias("banking"));
  assert(umi_application_component_domain_is_alias("logistics"));
  assert(umi_application_component_domain_capability_id("banking") != NULL);
  assert(umi_application_component_domain_count("banking") == 1U);
  assert(umi_application_component_domain_at("banking", 0U) ==
         umi_application_component_catalogue_find("umicom.shared.banking"));
  assert(umi_application_component_domain_at(NULL, 0U) == NULL);
  return 0;
}
