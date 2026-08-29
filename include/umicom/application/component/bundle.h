/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/component/bundle.h
 *
 * PURPOSE:
 *   Publish Framework-owned component bundles for common, Studio and Trader
 *   workspaces so applications assemble proven Lego-like layout recipes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_COMPONENT_BUNDLE_H
#define UMICOM_APPLICATION_COMPONENT_BUNDLE_H

#include "umicom/application/component/layout.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationComponentBundle {
  const char *bundle_id;
  const char *title;
  const char *description;
  const char *const *component_ids;
  const UmiApplicationComponentRegion *regions;
  size_t component_count;
} UmiApplicationComponentBundle;

size_t umi_application_component_bundle_count(void);
const UmiApplicationComponentBundle *umi_application_component_bundle_at(
    size_t index);
const UmiApplicationComponentBundle *umi_application_component_bundle_find(
    const char *bundle_id);
UmiStatus umi_application_component_bundle_layout(
    const UmiApplicationComponentBundle *bundle,
    UmiApplicationComponentLayout *out_layout);

#ifdef __cplusplus
}
#endif

#endif
