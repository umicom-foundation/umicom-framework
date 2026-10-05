/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/security/profile_keys_internal.h
 * PURPOSE: Separate private credential job inputs from native presentation state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_GTK4_SECURITY_PROFILE_KEYS_INTERNAL_H
#define UMICOM_GTK4_SECURITY_PROFILE_KEYS_INTERNAL_H
#include "umicom/security/gtk4/profile_keys.h"
typedef enum UmiProfileKeyOperation {
    UMI_PROFILE_KEY_REGISTER,
    UMI_PROFILE_KEY_SAVE,
    UMI_PROFILE_KEY_CHECK,
    UMI_PROFILE_KEY_REMOVE
} UmiProfileKeyOperation;
typedef UmiStatus (*UmiProfileKeyFactory)(const char *, const char *, UmiProfileSecrets **);
typedef struct UmiProfileKeyJob {
    UmiProfileKeyOperation operation;
    UmiProfileKeyFactory open_service;
    char application[UMI_PLATFORM_SECRET_SCOPE_CAPACITY];
    char profile[UMI_LOCAL_PROFILE_NAME_CAPACITY];
    char alias[UMI_PLATFORM_SECRET_NAME_CAPACITY];
    char password[UMI_LOCAL_PROFILE_PASSWORD_CAPACITY];
    char value[UMI_PLATFORM_SECRET_VALUE_CAPACITY];
    UmiStatus status;
} UmiProfileKeyJob;
struct UmiProfileKeysGtk {
    unsigned references, failures;
    bool closed, busy;
    gint64 retry_after;
    char application[UMI_PLATFORM_SECRET_SCOPE_CAPACITY];
    char profile[UMI_LOCAL_PROFILE_NAME_CAPACITY];
    /* Internal factory substitution keeps native tests out of personal vaults.
     * Production always selects UmiProfileSecretsPlatform; no widget is passed. */
    UmiProfileKeyFactory open_service;
    GtkWidget *root, *form, *password, *confirmation, *alias, *value, *message;
    GtkWidget *confirm_create, *confirm_save, *confirm_remove;
};
void UmiProfileKeysStart(UmiProfileKeysGtk *panel, UmiProfileKeyJob *job);
void UmiProfileKeysCompleted(UmiProfileKeysGtk *panel, const UmiProfileKeyJob *job);
void UmiProfileKeysRelease(UmiProfileKeysGtk *panel);
#endif
