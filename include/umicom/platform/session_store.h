/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/session_store.h
 *
 * PURPOSE:
 *   Define a small, deterministic and thread-safe session-state repository for
 *   workspace identity, open documents, active views, cursor state and product
 *   restoration metadata without requiring a graphical toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_SESSION_STORE_H
#define UMICOM_PLATFORM_SESSION_STORE_H

#include <stddef.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SESSION_KEY_CAPACITY 160U
#define UMI_SESSION_VALUE_CAPACITY 2048U
#define UMI_SESSION_STORE_MAX 512U

/**
 * Represent the session entry snapshot data shared with callers of this public contract.
 */
typedef struct UmiSessionEntrySnapshot {
    char key[UMI_SESSION_KEY_CAPACITY];
    char value[UMI_SESSION_VALUE_CAPACITY];
} UmiSessionEntrySnapshot;

/**
 * Represent the session store data shared with callers of this public contract.
 */
typedef struct UmiSessionStore UmiSessionStore;

/**
 * Initialise session store from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_session_store_create(UmiSessionStore **out_store);
/**
 * Release or reset state held by session store so the same storage can be reused safely.
 */
void umi_session_store_destroy(UmiSessionStore *store);
/**
 * Copy session store into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_session_store_set(UmiSessionStore *store,
                                const char *key,
                                const char *value);
/**
 * Provide the session store get operation used by this module and its client applications.
 */
UmiStatus umi_session_store_get(const UmiSessionStore *store,
                                const char *key,
                                char *out_value,
                                size_t value_capacity);
/**
 * Remove session store while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_session_store_remove(UmiSessionStore *store,
                                   const char *key);
/**
 * Release or reset state held by session store so the same storage can be reused safely.
 */
void umi_session_store_clear(UmiSessionStore *store);
/**
 * Return the number of records represented by session store without changing their state.
 */
size_t umi_session_store_count(const UmiSessionStore *store);
/**
 * Find session store while leaving the underlying catalogue or model owned by this module.
 */
UmiStatus umi_session_store_at(const UmiSessionStore *store,
                               size_t index,
                               UmiSessionEntrySnapshot *out_snapshot);
/**
 * Read session store into validated module state and return a status when input cannot be
 * used.
 */
UmiStatus umi_session_store_load(UmiSessionStore *store,
                                 const char *path,
                                 int *out_loaded);
/**
 * Write session store in its stable representation and report capacity or input failures
 * to the caller.
 */
UmiStatus umi_session_store_save(const UmiSessionStore *store,
                                 const char *path);

#ifdef __cplusplus
}
#endif

#endif
