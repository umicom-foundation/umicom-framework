/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/workspace_recipe_main.c
 *
 * PURPOSE:
 *   Let developers list, inspect, validate and export Framework-owned
 *   workspace recipes without launching a graphical application.
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

static UmiApplicationComponentRecipeAudience parse_audience(const char *text) {
  if (text != NULL && strcmp(text, "learning") == 0)
    return UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING;
  if (text != NULL && strcmp(text, "focus") == 0)
    return UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS;
  return UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD;
}

static int list_recipes(const char *application_id) {
  size_t index;
  for (index = 0U; index < umi_application_component_recipe_catalogue_count(); ++index) {
    const UmiApplicationComponentRecipe *recipe =
        umi_application_component_recipe_catalogue_at(index);
    if (application_id != NULL && strcmp(recipe->application_id, application_id) != 0)
      continue;
    (void)printf("%s | %s | %s | %zu components\n", recipe->recipe_id, recipe->application_id,
                 umi_application_component_recipe_audience_text(recipe->audience),
                 recipe->slot_count);
  }
  return 0;
}

static int show_recipe(const char *recipe_id) {
  const UmiApplicationComponentRecipe *recipe =
      umi_application_component_recipe_catalogue_find(recipe_id);
  size_t index;
  if (recipe == NULL) {
    (void)fprintf(stderr, "Workspace recipe not found: %s\n", recipe_id);
    return 2;
  }
  (void)printf("%s\n%s\n%s\nExperience: %s\n", recipe->title, recipe->application_id,
               recipe->description, recipe->experience_profile_id);
  for (index = 0U; index < recipe->slot_count; ++index) {
    const UmiApplicationComponentRecipeSlot *slot = &recipe->slots[index];
    (void)printf("- %s -> %s, weight %u%s\n", slot->component_id,
                 umi_application_component_region_text(slot->region), (unsigned int)slot->weight,
                 slot->locked ? ", locked" : "");
  }
  return 0;
}

static int validate_catalogue(void) {
  UmiApplicationComponentRegistry registry;
  UmiStatus status;
  umi_application_component_registry_init(&registry);
  status = umi_application_component_registry_seed_catalogue(&registry);
  if (status == UMI_STATUS_OK)
    status = umi_application_component_recipe_catalogue_validate(
        &registry, UMI_APPLICATION_COMPONENT_FRONTEND_HEADLESS);
  if (status != UMI_STATUS_OK) {
    (void)fprintf(stderr, "Workspace recipe validation failed: %s\n", umi_status_text(status));
    return 3;
  }
  (void)printf("Validated %zu workspace recipes.\n",
               umi_application_component_recipe_catalogue_count());
  return 0;
}

static int export_recipe(const char *application_id, const char *audience_text) {
  UmiApplicationComponentRegistry registry;
  UmiApplicationComponentWorkspaceDraft draft;
  UmiApplicationComponentWorkspacePolicy policy;
  UmiApplicationComponentDiagnosticReport diagnostics;
  const UmiApplicationComponentRecipe *recipe;
  UmiApplicationComponentRecipeAudience audience = parse_audience(audience_text);
  char encoded[65536];
  size_t encoded_length = 0U;
  UmiStatus status;

  recipe = umi_application_component_recipe_catalogue_recommend(application_id, audience);
  if (recipe == NULL) {
    (void)fprintf(stderr, "No workspace recipe is available for %s.\n", application_id);
    return 2;
  }
  umi_application_component_registry_init(&registry);
  status = umi_application_component_registry_seed_catalogue(&registry);
  if (status == UMI_STATUS_OK)
    status = umi_application_component_workspace_draft_init(&draft, recipe);
  if (audience == UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING)
    umi_application_component_workspace_policy_learning(&policy);
  else
    umi_application_component_workspace_policy_default(&policy);
  if (status == UMI_STATUS_OK)
    status = umi_application_component_workspace_policy_evaluate(
        &registry, &draft, UMI_APPLICATION_COMPONENT_FRONTEND_HEADLESS, &policy, &diagnostics);
  if (status == UMI_STATUS_OK && !umi_application_component_diagnostic_report_passed(&diagnostics))
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK)
    status = umi_application_component_workspace_encode(&draft, encoded, sizeof(encoded),
                                                        &encoded_length);
  if (status != UMI_STATUS_OK) {
    (void)fprintf(stderr, "Workspace export failed: %s\n", umi_status_text(status));
    return 4;
  }
  (void)fwrite(encoded, 1U, encoded_length, stdout);
  return 0;
}

static void print_help(void) {
  (void)printf("Usage:\n");
  (void)printf("  umicom-workspace-recipes list [application-id]\n");
  (void)printf("  umicom-workspace-recipes show <recipe-id>\n");
  (void)printf("  umicom-workspace-recipes validate\n");
  (void)printf("  umicom-workspace-recipes export <application-id> [learning|standard|focus]\n");
}

int main(int argc, char **argv) {
  if (argc == 1 || (argc >= 2 && strcmp(argv[1], "list") == 0))
    return list_recipes(argc >= 3 ? argv[2] : NULL);
  if (argc == 3 && strcmp(argv[1], "show") == 0)
    return show_recipe(argv[2]);
  if (argc == 2 && strcmp(argv[1], "validate") == 0)
    return validate_catalogue();
  if (argc >= 3 && argc <= 4 && strcmp(argv[1], "export") == 0)
    return export_recipe(argv[2], argc == 4 ? argv[3] : "standard");
  print_help();
  return 1;
}
