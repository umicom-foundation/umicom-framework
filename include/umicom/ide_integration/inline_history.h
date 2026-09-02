/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/inline_history.h
 *
 * PURPOSE:
 *   Retain recent inline AI suggestion outcomes for diagnostics and UX metrics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_INLINE_HISTORY_H
#define UMICOM_IDE_INTEGRATION_INLINE_HISTORY_H
#include "umicom/ide_integration/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_IDE_INLINE_HISTORY_CAPACITY 64U

/**
 * Represent the ide inline history data shared with callers of this public contract.
 */
typedef struct UmiIdeInlineHistory UmiIdeInlineHistory;

/**
 * Initialise ide inline history from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ide_inline_history_create(UmiIdeInlineHistory **out_history);
/**
 * Release or reset state held by ide inline history so the same storage can be reused
 * safely.
 */
void umi_ide_inline_history_destroy(UmiIdeInlineHistory *history);

/**
 * Provide the ide inline history record operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_inline_history_record(
    UmiIdeInlineHistory *history,
    const UmiIdeInlineSuggestion *suggestion);

/**
 * Find ide inline history while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ide_inline_history_at(
    const UmiIdeInlineHistory *history,
    size_t index,
    UmiIdeInlineSuggestion *out_suggestion);

/**
 * Return the number of records represented by ide inline history without changing their
 * state.
 */
size_t umi_ide_inline_history_count(const UmiIdeInlineHistory *history);
/**
 * Provide the ide inline history revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ide_inline_history_revision(const UmiIdeInlineHistory *history);

#ifdef __cplusplus
}
#endif
#endif
