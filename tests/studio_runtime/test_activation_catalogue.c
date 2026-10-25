/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_activation_catalogue.c
 *
 * PURPOSE:
 *   Focused regression coverage for Studio runtime activation catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include "umicom/studio_runtime/activation_catalogue.h"
#include <assert.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  UmiStudioRuntimeActivationRule rule;
/* The Kanban entry extends the supported catalogue. Retain the earlier
 * cardinality for compatibility review; exact identity checks remain active. */
#if 0
    assert(umi_studio_activation_profile_count() == 27U);
#endif
  assert(umi_studio_activation_profile_count() == 28U);
  assert(umi_studio_activation_resolve("ide.open.problem", &rule) == UMI_STATUS_OK);
  assert(rule.surface == UMI_STUDIO_SURFACE_EDITOR);
  assert(umi_studio_activation_resolve("ai.developer.open-approvals", &rule) == UMI_STATUS_OK);
  assert(rule.surface == UMI_STUDIO_SURFACE_AI_APPROVALS);
  assert(umi_studio_activation_resolve("ai.developer.open-model-comparison", &rule) ==
         UMI_STATUS_OK);
  assert(rule.surface == UMI_STUDIO_SURFACE_AI_MODEL_COMPARISON);
  assert(umi_studio_activation_resolve("help.ai-teacher", &rule) == UMI_STATUS_OK);
  assert(rule.surface == UMI_STUDIO_SURFACE_LEARNING);
  assert(umi_studio_activation_resolve("help.documentation", &rule) == UMI_STATUS_OK);
  assert(rule.surface == UMI_STUDIO_SURFACE_DOCUMENTATION);
  assert(umi_studio_activation_resolve("designer.open", &rule) == UMI_STATUS_OK);
  assert(rule.surface == UMI_STUDIO_SURFACE_VISUAL_DESIGNER);
  assert(umi_studio_activation_resolve("designer.palette.search", &rule) == UMI_STATUS_OK);
  assert(rule.surface == UMI_STUDIO_SURFACE_COMPONENT_PALETTE);
  assert(umi_studio_activation_resolve("designer.structure.select", &rule) == UMI_STATUS_OK);
  assert(rule.surface == UMI_STUDIO_SURFACE_COMPONENT_STRUCTURE);
  assert(umi_studio_activation_resolve("designer.inspector.property", &rule) == UMI_STATUS_OK);
  assert(rule.surface == UMI_STUDIO_SURFACE_OBJECT_INSPECTOR);
  assert(umi_studio_activation_resolve("designer.code", &rule) == UMI_STATUS_OK);
  assert(rule.surface == UMI_STUDIO_SURFACE_DESIGN_CODE);
  assert(umi_studio_activation_resolve("designer.mixed", &rule) == UMI_STATUS_OK);
  assert(rule.surface == UMI_STUDIO_SURFACE_DESIGN_MIXED);
  assert(umi_studio_activation_resolve("designer.preview", &rule) == UMI_STATUS_OK);
  assert(rule.surface == UMI_STUDIO_SURFACE_DESIGN_PREVIEW);
  /* Opening Live Preview must focus the active-document surface rather than
   * the visual designer's separate preview mode. */
  assert(umi_studio_activation_resolve("view.live-preview", &rule) == UMI_STATUS_OK);
  assert(rule.surface == UMI_STUDIO_SURFACE_LIVE_PREVIEW);
  return 0;
}
