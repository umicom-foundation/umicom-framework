/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/command.h
 *
 * PURPOSE:
 *   Define parameterised application commands with risk, permission,
 *   asynchronous execution and undo metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_COMMAND_H
#define UMICOM_APPLICATION_EXPERIENCE_COMMAND_H

#include "umicom/application/experience/parameter.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiApplicationExperienceCommandFlags {
  UMI_APPLICATION_EXPERIENCE_COMMAND_UNDOABLE = 1U << 0,
  UMI_APPLICATION_EXPERIENCE_COMMAND_ASYNCHRONOUS = 1U << 1,
  UMI_APPLICATION_EXPERIENCE_COMMAND_REQUIRES_SELECTION = 1U << 2,
  UMI_APPLICATION_EXPERIENCE_COMMAND_REQUIRES_ONLINE = 1U << 3,
  UMI_APPLICATION_EXPERIENCE_COMMAND_CHANGES_DATA = 1U << 4,
  UMI_APPLICATION_EXPERIENCE_COMMAND_ALLOWED_READ_ONLY = 1U << 5
} UmiApplicationExperienceCommandFlags;

typedef struct UmiApplicationExperienceCommand {
  uint32_t struct_size;
  uint32_t api_version;
  const char *command_id;
  const char *title;
  const char *description;
  const char *category;
  const char *permission_id;
  UmiApplicationExperienceCommandRisk risk;
  uint32_t flags;
  const UmiApplicationExperienceParameter *parameters;
  size_t parameter_count;
} UmiApplicationExperienceCommand;

UmiStatus
umi_application_experience_command_validate(const UmiApplicationExperienceCommand *command);
const UmiApplicationExperienceParameter *
umi_application_experience_command_parameter(const UmiApplicationExperienceCommand *command,
                                             const char *parameter_id);
int umi_application_experience_command_has_flag(const UmiApplicationExperienceCommand *command,
                                                UmiApplicationExperienceCommandFlags flag);

#ifdef __cplusplus
}
#endif

#endif
