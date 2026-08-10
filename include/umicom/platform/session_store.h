/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/session_store.h
 *
 * PURPOSE:
 *   Define a small, deterministic and thread-safe session-state repository for
 *   workspace identity, open documents, active views, cursor state and product
 *   restoration metadata without requiring a graphical toolkit.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiSessionEntrySnapshot {
    char key[UMI_SESSION_KEY_CAPACITY];
    char value[UMI_SESSION_VALUE_CAPACITY];
} UmiSessionEntrySnapshot;

typedef struct UmiSessionStore UmiSessionStore;

UmiStatus umi_session_store_create(UmiSessionStore **out_store);
void umi_session_store_destroy(UmiSessionStore *store);
UmiStatus umi_session_store_set(UmiSessionStore *store,
                                const char *key,
                                const char *value);
UmiStatus umi_session_store_get(const UmiSessionStore *store,
                                const char *key,
                                char *out_value,
                                size_t value_capacity);
UmiStatus umi_session_store_remove(UmiSessionStore *store,
                                   const char *key);
void umi_session_store_clear(UmiSessionStore *store);
size_t umi_session_store_count(const UmiSessionStore *store);
UmiStatus umi_session_store_at(const UmiSessionStore *store,
                               size_t index,
                               UmiSessionEntrySnapshot *out_snapshot);
UmiStatus umi_session_store_load(UmiSessionStore *store,
                                 const char *path,
                                 int *out_loaded);
UmiStatus umi_session_store_save(const UmiSessionStore *store,
                                 const char *path);

#ifdef __cplusplus
}
#endif

#endif
