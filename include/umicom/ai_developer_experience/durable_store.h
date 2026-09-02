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

/**
 * Represent the ai developer durable store data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDeveloperDurableStore {
    UmiSessionStore *store;
    char path[2048];
    uint64_t revision;
    int loaded;
} UmiAiDeveloperDurableStore;

/**
 * Initialise ai developer durable store from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_developer_durable_store_create(
    const char *path,
    UmiAiDeveloperDurableStore **out_store);

/**
 * Release or reset state held by ai developer durable store so the same storage can be
 * reused safely.
 */
void umi_ai_developer_durable_store_destroy(
    UmiAiDeveloperDurableStore *store);

/**
 * Read ai developer durable store into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_ai_developer_durable_store_load(
    UmiAiDeveloperDurableStore *store);

/**
 * Write ai developer durable store in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_ai_developer_durable_store_save(
    UmiAiDeveloperDurableStore *store);

/**
 * Provide the ai developer durable store session operation used by this module and its
 * client applications.
 */
UmiSessionStore *umi_ai_developer_durable_store_session(
    UmiAiDeveloperDurableStore *store);

#ifdef __cplusplus
}
#endif
#endif
