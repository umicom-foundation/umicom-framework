/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/inline_history.h
 *
 * PURPOSE:
 *   Retain recent inline AI suggestion outcomes for diagnostics and UX metrics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_INLINE_HISTORY_H
#define UMICOM_IDE_INTEGRATION_INLINE_HISTORY_H
#include "umicom/ide_integration/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_IDE_INLINE_HISTORY_CAPACITY 64U

typedef struct UmiIdeInlineHistory UmiIdeInlineHistory;

UmiStatus umi_ide_inline_history_create(UmiIdeInlineHistory **out_history);
void umi_ide_inline_history_destroy(UmiIdeInlineHistory *history);

UmiStatus umi_ide_inline_history_record(
    UmiIdeInlineHistory *history,
    const UmiIdeInlineSuggestion *suggestion);

UmiStatus umi_ide_inline_history_at(
    const UmiIdeInlineHistory *history,
    size_t index,
    UmiIdeInlineSuggestion *out_suggestion);

size_t umi_ide_inline_history_count(const UmiIdeInlineHistory *history);
uint64_t umi_ide_inline_history_revision(const UmiIdeInlineHistory *history);

#ifdef __cplusplus
}
#endif
#endif
