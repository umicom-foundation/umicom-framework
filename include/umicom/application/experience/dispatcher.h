/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/dispatcher.h
 *
 * PURPOSE:
 *   Dispatch validated command invocations through registered handlers while
 *   preserving status, messages and undo tokens for frontend-independent use.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_DISPATCHER_H
#define UMICOM_APPLICATION_EXPERIENCE_DISPATCHER_H

#include "umicom/application/experience/enablement.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application experience command result data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationExperienceCommandResult {
  UmiStatus status;
  char message[UMI_APPLICATION_EXPERIENCE_TEXT_CAPACITY];
  char undo_token[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  uint64_t completed_sequence;
} UmiApplicationExperienceCommandResult;

typedef UmiStatus (*UmiApplicationExperienceCommandHandler)(
    const UmiApplicationExperienceInvocation *invocation,
    UmiApplicationExperienceCommandResult *result, void *user_data);

/**
 * Represent the application experience handler binding data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationExperienceHandlerBinding {
  char command_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  UmiApplicationExperienceCommandHandler handler;
  void *user_data;
} UmiApplicationExperienceHandlerBinding;

/**
 * Represent the application experience dispatcher data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationExperienceDispatcher {
  UmiApplicationExperienceHandlerBinding bindings[UMI_APPLICATION_EXPERIENCE_COMMAND_CAPACITY];
  size_t count;
  uint64_t next_sequence;
} UmiApplicationExperienceDispatcher;

/**
 * Initialise application experience dispatcher from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_experience_dispatcher_init(UmiApplicationExperienceDispatcher *dispatcher);
/**
 * Provide the application experience dispatcher bind operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_experience_dispatcher_bind(UmiApplicationExperienceDispatcher *dispatcher,
                                                     const char *command_id,
                                                     UmiApplicationExperienceCommandHandler handler,
                                                     void *user_data);
/**
 * Perform application experience dispatcher through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_application_experience_dispatcher_dispatch(
    UmiApplicationExperienceDispatcher *dispatcher,
    const UmiApplicationExperienceCommandRegistry *registry,
    const UmiApplicationExperiencePermissionSet *permissions,
    const UmiApplicationExperienceContext *context, UmiApplicationExperienceInvocation *invocation,
    UmiApplicationExperienceCommandResult *result);

#ifdef __cplusplus
}
#endif

#endif
