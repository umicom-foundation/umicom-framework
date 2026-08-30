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

const char *umi_ai_developer_state_schema_root(void)
{
    return UMI_AI_DEVELOPER_STATE_ROOT;
}

uint32_t umi_ai_developer_state_schema_version(void)
{
    return UMI_AI_DEVELOPER_STATE_SCHEMA_VERSION;
}
