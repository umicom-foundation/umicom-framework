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

typedef struct UmiApplicationExperienceCommandResult {
  UmiStatus status;
  char message[UMI_APPLICATION_EXPERIENCE_TEXT_CAPACITY];
  char undo_token[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  uint64_t completed_sequence;
} UmiApplicationExperienceCommandResult;

typedef UmiStatus (*UmiApplicationExperienceCommandHandler)(
    const UmiApplicationExperienceInvocation *invocation,
    UmiApplicationExperienceCommandResult *result, void *user_data);

typedef struct UmiApplicationExperienceHandlerBinding {
  char command_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  UmiApplicationExperienceCommandHandler handler;
  void *user_data;
} UmiApplicationExperienceHandlerBinding;

typedef struct UmiApplicationExperienceDispatcher {
  UmiApplicationExperienceHandlerBinding bindings[UMI_APPLICATION_EXPERIENCE_COMMAND_CAPACITY];
  size_t count;
  uint64_t next_sequence;
} UmiApplicationExperienceDispatcher;

void umi_application_experience_dispatcher_init(UmiApplicationExperienceDispatcher *dispatcher);
UmiStatus umi_application_experience_dispatcher_bind(UmiApplicationExperienceDispatcher *dispatcher,
                                                     const char *command_id,
                                                     UmiApplicationExperienceCommandHandler handler,
                                                     void *user_data);
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
