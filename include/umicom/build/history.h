/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/history.h
 *
 * PURPOSE:
 *   Provide a bounded thread-safe history of completed build operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_HISTORY_H
#define UMICOM_BUILD_HISTORY_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/build/result.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiBuildHistory UmiBuildHistory;

UmiStatus umi_build_history_create(size_t capacity,
                                   UmiBuildHistory **out_history);
void umi_build_history_destroy(UmiBuildHistory *history);
UmiStatus umi_build_history_append(UmiBuildHistory *history,
                                   const UmiBuildResult *result);
size_t umi_build_history_count(const UmiBuildHistory *history);
UmiStatus umi_build_history_at(const UmiBuildHistory *history,
                               size_t index,
                               UmiBuildResult *out_result);
UmiStatus umi_build_history_latest(const UmiBuildHistory *history,
                                   UmiBuildResult *out_result);
void umi_build_history_clear(UmiBuildHistory *history);

#ifdef __cplusplus
}
#endif

#endif
