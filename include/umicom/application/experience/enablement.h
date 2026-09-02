/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/enablement.h
 *
 * PURPOSE:
 *   Evaluate command visibility and availability from a small shared context
 *   instead of embedding enablement logic independently in every frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_ENABLEMENT_H
#define UMICOM_APPLICATION_EXPERIENCE_ENABLEMENT_H

#include "umicom/application/experience/permission.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application experience context data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationExperienceContext {
  size_t selected_item_count;
  int online;
  int busy;
  int read_only;
  int document_dirty;
  int component_visible;
} UmiApplicationExperienceContext;

/**
 * Represent the application experience enablement data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationExperienceEnablement {
  UmiApplicationExperienceCommandAvailability availability;
  const char *reason;
} UmiApplicationExperienceEnablement;

/**
 * Provide the application experience context default operation used by this module and its
 * client applications.
 */
void umi_application_experience_context_default(UmiApplicationExperienceContext *context);
/**
 * Provide the application experience command enablement operation used by this module and
 * its client applications.
 */
UmiApplicationExperienceEnablement umi_application_experience_command_enablement(
    const UmiApplicationExperienceCommand *command,
    const UmiApplicationExperiencePermissionSet *permissions,
    const UmiApplicationExperienceContext *context);

#ifdef __cplusplus
}
#endif

#endif
