/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/types.c
 *
 * PURPOSE:
 *   Convert stable component frontend, lifecycle and layout-region values into
 *   portable diagnostic and persistence text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/types.h"

/*
 * Check that application component frontend satisfies its contract before another service
 * relies on it.
 */
int umi_application_component_frontend_valid(
    UmiApplicationComponentFrontend frontend) {
  return frontend == UMI_APPLICATION_COMPONENT_FRONTEND_HEADLESS ||
         frontend == UMI_APPLICATION_COMPONENT_FRONTEND_GTK4 ||
         frontend == UMI_APPLICATION_COMPONENT_FRONTEND_QT6 ||
         frontend == UMI_APPLICATION_COMPONENT_FRONTEND_WEB;
}

/*
 * Provide the application component frontend text operation used by this module and its
 * client applications.
 */
const char *umi_application_component_frontend_text(
    UmiApplicationComponentFrontend frontend) {
  /* Select the behaviour associated with the requested command or state value. */
  switch (frontend) {
  case UMI_APPLICATION_COMPONENT_FRONTEND_HEADLESS:
    return "headless";
  case UMI_APPLICATION_COMPONENT_FRONTEND_GTK4:
    return "gtk4";
  case UMI_APPLICATION_COMPONENT_FRONTEND_QT6:
    return "qt6";
  case UMI_APPLICATION_COMPONENT_FRONTEND_WEB:
    return "web";
  default:
    return "none";
  }
}

/*
 * Provide the application component lifecycle text operation used by this module and its
 * client applications.
 */
const char *umi_application_component_lifecycle_text(
    UmiApplicationComponentLifecycle lifecycle) {
  /* Select the behaviour associated with the requested command or state value. */
  switch (lifecycle) {
  case UMI_APPLICATION_COMPONENT_REGISTERED:
    return "registered";
  case UMI_APPLICATION_COMPONENT_CREATED:
    return "created";
  case UMI_APPLICATION_COMPONENT_ACTIVE:
    return "active";
  case UMI_APPLICATION_COMPONENT_SUSPENDED:
    return "suspended";
  case UMI_APPLICATION_COMPONENT_CLOSED:
    return "closed";
  case UMI_APPLICATION_COMPONENT_FAILED:
    return "failed";
  default:
    return "unknown";
  }
}

/*
 * Provide the application component region text operation used by this module and its
 * client applications.
 */
const char *umi_application_component_region_text(
    UmiApplicationComponentRegion region) {
  /* Select the behaviour associated with the requested command or state value. */
  switch (region) {
  case UMI_APPLICATION_COMPONENT_REGION_PRIMARY:
    return "primary";
  case UMI_APPLICATION_COMPONENT_REGION_LEFT:
    return "left";
  case UMI_APPLICATION_COMPONENT_REGION_RIGHT:
    return "right";
  case UMI_APPLICATION_COMPONENT_REGION_BOTTOM:
    return "bottom";
  case UMI_APPLICATION_COMPONENT_REGION_FLOATING:
    return "floating";
  default:
    return "unknown";
  }
}
