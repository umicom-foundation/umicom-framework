/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_shell/persona.h
 *
 * PURPOSE:
 *   Compose reusable shell profiles into product personas. Studio and Desk can
 *   therefore remain thin products that select Framework-owned shell behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SHELL_PERSONA_H
#define UMICOM_APPLICATION_SHELL_PERSONA_H

#include "umicom/application_shell/builtin_profiles.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_SHELL_PERSONA_MAX_PROFILES 48U

/**
 * Represent the application shell persona definition data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationShellPersonaDefinition {
    uint32_t structure_size;
    uint32_t api_version;
    const char *persona_id;
    const char *title;
    const char *description;
    const char *const *profile_ids;
    size_t profile_count;
} UmiApplicationShellPersonaDefinition;

/**
 * Return the number of records represented by application shell persona without changing
 * their state.
 */
size_t umi_application_shell_persona_count(void);

/**
 * Find application shell persona while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiApplicationShellPersonaDefinition *
umi_application_shell_persona_at(size_t index);

/**
 * Find application shell persona while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiApplicationShellPersonaDefinition *
umi_application_shell_persona_find(const char *persona_id);

/**
 * Check that application shell persona satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_application_shell_persona_validate(
    const UmiApplicationShellPersonaDefinition *persona);

/**
 * Provide the application shell persona install operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_shell_persona_install(
    UmiApplicationShellRegistry *registry,
    const UmiApplicationShellPersonaDefinition *persona);

#ifdef __cplusplus
}
#endif

#endif
