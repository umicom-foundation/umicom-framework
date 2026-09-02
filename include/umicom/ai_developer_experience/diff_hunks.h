/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/diff_hunks.h
 *
 * PURPOSE:
 *   Group changed diff lines into bounded hunks with configurable context.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_DIFF_HUNKS_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_DIFF_HUNKS_H
#include "umicom/ai_developer_experience/diff.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ai developer diff build hunks operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_diff_build_hunks(
    UmiAiDeveloperFileDiff *diff,
    size_t context_lines);

#ifdef __cplusplus
}
#endif
#endif
