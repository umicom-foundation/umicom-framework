/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/invocation.h
 *
 * PURPOSE:
 *   Capture a parameterised command request with correlation, confirmation
 *   and origin metadata so it can be validated, dispatched and replayed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_INVOCATION_H
#define UMICOM_APPLICATION_EXPERIENCE_INVOCATION_H

#include "umicom/application/experience/command.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationExperienceArgument {
  char parameter_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  UmiApplicationExperienceValue value;
} UmiApplicationExperienceArgument;

typedef struct UmiApplicationExperienceInvocation {
  char command_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  char correlation_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  char origin_component_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  UmiApplicationExperienceArgument arguments[UMI_APPLICATION_EXPERIENCE_PARAMETER_CAPACITY];
  size_t argument_count;
  uint64_t sequence;
  int confirmed;
} UmiApplicationExperienceInvocation;

UmiStatus umi_application_experience_invocation_init(UmiApplicationExperienceInvocation *invocation,
                                                     const char *command_id,
                                                     const char *correlation_id,
                                                     const char *origin_component_id);
UmiStatus umi_application_experience_invocation_set(UmiApplicationExperienceInvocation *invocation,
                                                    const char *parameter_id,
                                                    const UmiApplicationExperienceValue *value);
const UmiApplicationExperienceValue *
umi_application_experience_invocation_get(const UmiApplicationExperienceInvocation *invocation,
                                          const char *parameter_id);
UmiStatus umi_application_experience_invocation_validate(
    const UmiApplicationExperienceCommand *command,
    const UmiApplicationExperienceInvocation *invocation);

#ifdef __cplusplus
}
#endif

#endif
