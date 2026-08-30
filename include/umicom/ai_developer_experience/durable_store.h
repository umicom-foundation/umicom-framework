/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/durable_store.h
 *
 * PURPOSE:
 *   Own an existing UmiSessionStore plus an optional disk path so AI developer
 *   state can be loaded/saved without introducing another persistence engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_DURABLE_STORE_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_DURABLE_STORE_H
#include <stdint.h>
#include "umicom/platform/session_store.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDeveloperDurableStore {
    UmiSessionStore *store;
    char path[2048];
    uint64_t revision;
    int loaded;
} UmiAiDeveloperDurableStore;

UmiStatus umi_ai_developer_durable_store_create(
    const char *path,
    UmiAiDeveloperDurableStore **out_store);

void umi_ai_developer_durable_store_destroy(
    UmiAiDeveloperDurableStore *store);

UmiStatus umi_ai_developer_durable_store_load(
    UmiAiDeveloperDurableStore *store);

UmiStatus umi_ai_developer_durable_store_save(
    UmiAiDeveloperDurableStore *store);

UmiSessionStore *umi_ai_developer_durable_store_session(
    UmiAiDeveloperDurableStore *store);

#ifdef __cplusplus
}
#endif
#endif
