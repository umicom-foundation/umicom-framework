/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/state_schema.c
 *
 * PURPOSE:
 *   Implement stable durable-state schema identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/state_schema.h"

/*
 * Provide the ai developer state schema root operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_state_schema_root(void)
{
    return UMI_AI_DEVELOPER_STATE_ROOT;
}

/*
 * Provide the ai developer state schema version operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_developer_state_schema_version(void)
{
    return UMI_AI_DEVELOPER_STATE_SCHEMA_VERSION;
}
