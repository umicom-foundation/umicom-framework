/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/component_composer_main.c
 *
 * PURPOSE:
 *   Demonstrate declarative common, Studio and Trader bundle composition
 *   through the same registry, layout and headless factory contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/component.h"

#include <stdio.h>
#include <string.h>

static const char *bundle_id_from_argument(const char *argument) {
  if (argument == NULL || strcmp(argument, "common") == 0)
    return "umicom.bundle.common";
  if (strcmp(argument, "studio") == 0)
    return "umicom.bundle.studio";
  if (strcmp(argument, "trader") == 0)
    return "umicom.bundle.trader";
  return argument;
}

int main(int argc, char **argv) {
  UmiApplicationComponentRegistry components;
  UmiApplicationComponentFactoryRegistry factories;
  UmiApplicationComponentLayout layout;
  UmiApplicationComponentWorkspace workspace;
  const UmiApplicationComponentBundle *bundle;
  size_t index;
  UmiStatus status;

  umi_application_component_registry_init(&components);
  umi_application_component_factory_registry_init(&factories);
  status = umi_application_component_registry_seed_catalogue(&components);
  if (status == UMI_STATUS_OK)
    status = umi_application_component_factory_registry_seed_headless(&factories);
  bundle = umi_application_component_bundle_find(
      bundle_id_from_argument(argc > 1 ? argv[1] : NULL));
  if (status != UMI_STATUS_OK || bundle == NULL) {
    (void)fprintf(stderr, "Unable to resolve component bundle.\n");
    return 2;
  }
  status = umi_application_component_bundle_layout(bundle, &layout);
  if (status == UMI_STATUS_OK)
    status = umi_application_component_layout_materialise(
        &components, &factories, &layout,
        UMI_APPLICATION_COMPONENT_FRONTEND_HEADLESS, &workspace);
  if (status != UMI_STATUS_OK) {
    (void)fprintf(stderr, "Composition failed: %s\n", umi_status_text(status));
    return 3;
  }

  (void)printf("%s: %zu active components\n", bundle->title,
               workspace.instance_count);
  for (index = 0U; index < workspace.instance_count; ++index) {
    (void)printf("- %s [%s] -> %s\n",
                 workspace.instances[index].component_id,
                 umi_application_component_region_text(workspace.regions[index]),
                 umi_application_component_lifecycle_text(
                     workspace.instances[index].lifecycle));
  }
  umi_application_component_workspace_destroy(&workspace);
  return 0;
}
