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

void umi_application_experience_dispatcher_init(UmiApplicationExperienceDispatcher *dispatcher) {
  if (dispatcher != NULL) {
    (void)memset(dispatcher, 0, sizeof(*dispatcher));
    dispatcher->next_sequence = 1U;
  }
}

UmiStatus umi_application_experience_dispatcher_bind(UmiApplicationExperienceDispatcher *dispatcher,
                                                     const char *command_id,
                                                     UmiApplicationExperienceCommandHandler handler,
                                                     void *user_data) {
  size_t index;
  UmiStatus status;
  if (dispatcher == NULL || handler == NULL ||
      !umi_application_experience_identifier_valid(command_id))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (dispatcher->count > UMI_APPLICATION_EXPERIENCE_COMMAND_CAPACITY)
    return UMI_STATUS_INVALID_STATE;
  for (index = 0U; index < dispatcher->count; ++index) {
    if (strcmp(dispatcher->bindings[index].command_id, command_id) == 0)
      return UMI_STATUS_ALREADY_EXISTS;
  }
  if (dispatcher->count >= UMI_APPLICATION_EXPERIENCE_COMMAND_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  index = dispatcher->count;
  status = umi_text_copy(dispatcher->bindings[index].command_id,
                         sizeof(dispatcher->bindings[index].command_id), command_id);
  if (status == UMI_STATUS_OK) {
    dispatcher->bindings[index].handler = handler;
    dispatcher->bindings[index].user_data = user_data;
    dispatcher->count += 1U;
  }
  return status;
}

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
  if (dispatcher == NULL || registry == NULL || context == NULL || invocation == NULL ||
      result == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  (void)memset(result, 0, sizeof(*result));
  command = umi_application_experience_command_registry_find(registry, invocation->command_id);
  if (command == NULL)
    return UMI_STATUS_NOT_FOUND;
  status = umi_application_experience_invocation_validate(command, invocation);
  if (status != UMI_STATUS_OK)
    return status;
  enablement = umi_application_experience_command_enablement(command, permissions, context);
  if (enablement.availability != UMI_APPLICATION_EXPERIENCE_COMMAND_ENABLED) {
    (void)umi_text_copy(result->message, sizeof(result->message), enablement.reason);
    result->status = UMI_STATUS_UNAVAILABLE;
    return result->status;
  }
  for (index = 0U; index < dispatcher->count; ++index) {
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
