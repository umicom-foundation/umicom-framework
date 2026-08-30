/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/state_schema.h
 *
 * PURPOSE:
 *   Publish durable AI developer state schema identifiers and migration version.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_STATE_SCHEMA_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_STATE_SCHEMA_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_DEVELOPER_STATE_SCHEMA_VERSION 1U
#define UMI_AI_DEVELOPER_STATE_ROOT "ai.developer"

const char *umi_ai_developer_state_schema_root(void);
uint32_t umi_ai_developer_state_schema_version(void);

#ifdef __cplusplus
}
#endif
#endif
