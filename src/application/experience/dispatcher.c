/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/dispatcher.c
 *
 * PURPOSE:
 *   Apply definition, argument, permission and enablement checks before a
 *   reusable command handler is allowed to execute application behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/dispatcher.h"

#include <string.h>

#include "umicom/base/text.h"

/*
 * Initialise application experience dispatcher from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_experience_dispatcher_init(UmiApplicationExperienceDispatcher *dispatcher) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (dispatcher != NULL) {
    (void)memset(dispatcher, 0, sizeof(*dispatcher));
    dispatcher->next_sequence = 1U;
  }
}

/*
 * Provide the application experience dispatcher bind operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_experience_dispatcher_bind(UmiApplicationExperienceDispatcher *dispatcher,
                                                     const char *command_id,
                                                     UmiApplicationExperienceCommandHandler handler,
                                                     void *user_data) {
  size_t index;
  UmiStatus status;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (dispatcher == NULL || handler == NULL ||
      !umi_application_experience_identifier_valid(command_id))
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (dispatcher->count > UMI_APPLICATION_EXPERIENCE_COMMAND_CAPACITY)
    return UMI_STATUS_INVALID_STATE;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < dispatcher->count; ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(dispatcher->bindings[index].command_id, command_id) == 0)
      return UMI_STATUS_ALREADY_EXISTS;
  }
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (dispatcher->count >= UMI_APPLICATION_EXPERIENCE_COMMAND_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  index = dispatcher->count;
  status = umi_text_copy(dispatcher->bindings[index].command_id,
                         sizeof(dispatcher->bindings[index].command_id), command_id);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK) {
    dispatcher->bindings[index].handler = handler;
    dispatcher->bindings[index].user_data = user_data;
    dispatcher->count += 1U;
  }
  return status;
}

/*
 * Perform application experience dispatcher through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_application_experience_dispatcher_dispatch(
    UmiApplicationExperienceDispatcher *dispatcher,
    const UmiApplicationExperienceCommandRegistry *registry,
    const UmiApplicationExperiencePermissionSet *permissions,
    const UmiApplicationExperienceContext *context, UmiApplicationExperienceInvocation *invocation,
    UmiApplicationExperienceCommandResult *result) {
  const UmiApplicationExperienceCommand *command;
  UmiApplicationExperienceEnablement enablement;
  size_t index;
  UmiStatus status;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (dispatcher == NULL || registry == NULL || context == NULL || invocation == NULL ||
      result == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  (void)memset(result, 0, sizeof(*result));
  command = umi_application_experience_command_registry_find(registry, invocation->command_id);
  /* Use the shared build helper when it is available from the parent composition. */
  if (command == NULL)
    return UMI_STATUS_NOT_FOUND;
  status = umi_application_experience_invocation_validate(command, invocation);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    return status;
  enablement = umi_application_experience_command_enablement(command, permissions, context);
  /* Apply this operation only while the related capability or state is available. */
  if (enablement.availability != UMI_APPLICATION_EXPERIENCE_COMMAND_ENABLED) {
    (void)umi_text_copy(result->message, sizeof(result->message), enablement.reason);
    result->status = UMI_STATUS_UNAVAILABLE;
    return result->status;
  }
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < dispatcher->count; ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(dispatcher->bindings[index].command_id, command->command_id) == 0) {
      invocation->sequence = dispatcher->next_sequence++;
      status = dispatcher->bindings[index].handler(invocation, result,
                                                   dispatcher->bindings[index].user_data);
      result->status = status;
      result->completed_sequence = invocation->sequence;
      return status;
    }
  }
  return UMI_STATUS_NOT_IMPLEMENTED;
}
