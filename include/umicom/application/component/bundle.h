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

/**
 * Represent the application component bundle data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationComponentBundle {
  const char *bundle_id;
  const char *title;
  const char *description;
  const char *const *component_ids;
  const UmiApplicationComponentRegion *regions;
  size_t component_count;
} UmiApplicationComponentBundle;

/**
 * Return the number of records represented by application component bundle without
 * changing their state.
 */
size_t umi_application_component_bundle_count(void);
/**
 * Find application component bundle while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiApplicationComponentBundle *umi_application_component_bundle_at(
    size_t index);
/**
 * Find application component bundle while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiApplicationComponentBundle *umi_application_component_bundle_find(
    const char *bundle_id);
/**
 * Provide the application component bundle layout operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_component_bundle_layout(
    const UmiApplicationComponentBundle *bundle,
    UmiApplicationComponentLayout *out_layout);

#ifdef __cplusplus
}
#endif

#endif
