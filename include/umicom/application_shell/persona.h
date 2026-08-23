/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_shell/persona.h
 *
 * PURPOSE:
 *   Compose reusable shell profiles into product personas. Studio and Desk can
 *   therefore remain thin products that select Framework-owned shell behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SHELL_PERSONA_H
#define UMICOM_APPLICATION_SHELL_PERSONA_H

#include "umicom/application_shell/builtin_profiles.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_SHELL_PERSONA_MAX_PROFILES 48U

typedef struct UmiApplicationShellPersonaDefinition {
    uint32_t structure_size;
    uint32_t api_version;
    const char *persona_id;
    const char *title;
    const char *description;
    const char *const *profile_ids;
    size_t profile_count;
} UmiApplicationShellPersonaDefinition;

size_t umi_application_shell_persona_count(void);

const UmiApplicationShellPersonaDefinition *
umi_application_shell_persona_at(size_t index);

const UmiApplicationShellPersonaDefinition *
umi_application_shell_persona_find(const char *persona_id);

UmiStatus umi_application_shell_persona_validate(
    const UmiApplicationShellPersonaDefinition *persona);

UmiStatus umi_application_shell_persona_install(
    UmiApplicationShellRegistry *registry,
    const UmiApplicationShellPersonaDefinition *persona);

#ifdef __cplusplus
}
#endif

#endif
