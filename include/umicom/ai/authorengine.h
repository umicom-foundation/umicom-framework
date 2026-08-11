/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/authorengine.h
 *
 * PURPOSE:
 *   Configure the Umicom AuthorEngine executable and build deterministic CLI arguments.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * AuthorEngine is integrated through a process boundary, so Studio can reuse its capabilities without copying publishing logic into the IDE.
 */

#ifndef INCLUDE_UMICOM_AI_AUTHORENGINE_H
#define INCLUDE_UMICOM_AI_AUTHORENGINE_H

#include <stdint.h>
#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiAuthorEngineConfig {
    char executable[UMI_AI_TEXT_CAPACITY];
    char workspace[UMI_AI_TEXT_CAPACITY];
    char provider[UMI_AI_ID_CAPACITY];
} UmiAiAuthorEngineConfig;

UmiStatus umi_ai_authorengine_validate(const UmiAiAuthorEngineConfig *config);

#ifdef __cplusplus
}
#endif

#endif
