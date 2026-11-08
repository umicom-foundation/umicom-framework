/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/local_profile.c
 *
 * PURPOSE:
 *   Keep local profile validation and password handling in the shared security module.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/security/local_profile.h"
#include "umicom/security/secrets.h"
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

struct UmiLocalProfileStore { atomic_uint references; UmiLocalProfileBackend backend; };
static size_t ProfileLength(const char *text, size_t limit)
{
    size_t n = 0U;
    if (text == NULL) return limit;
    while (n < limit && text[n] != '\0') ++n;
    return n;
}
/* Naming now lives in profile_name.c so web and mobile consumers can link
 * the exact same policy without credential backends. Preserve the previous
 * placement for review; the public functions and account callers are unchanged. */
#if 0
/* Form and protocol adapters can carry bounded byte spans. Centralising the
 * policy here avoids a temporary string copy and keeps every application on the
 * same naming rules. Read and validate completely before changing caller state. */
UmiStatus UmiLocalProfileNameBytes(const char *name, size_t length,
                                 char out[UMI_LOCAL_PROFILE_NAME_CAPACITY])
{
    char value[UMI_LOCAL_PROFILE_NAME_CAPACITY] = {0};
    if (name == NULL || out == NULL || length < 3U || length >= sizeof(value))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < length; ++i) {
        unsigned char c = (unsigned char)name[i];
        if (c >= 'A' && c <= 'Z') c = (unsigned char)(c + ('a' - 'A'));
        int letter = c >= 'a' && c <= 'z';
        if (!letter && (i == 0U || !((c >= '0' && c <= '9') || c == '-' || c == '_')))
            return UMI_STATUS_INVALID_ARGUMENT;
        value[i] = (char)c;
    }
    memcpy(out, value, sizeof(value));
    return UMI_STATUS_OK;
}

/* Keep the public string entry point while sharing its policy with byte spans.
 * Existing callers still provide a valid C string. Bounded readers use Bytes. */
UmiStatus UmiLocalProfileName(const char *name, char out[UMI_LOCAL_PROFILE_NAME_CAPACITY])
{
    return UmiLocalProfileNameBytes(name,
        ProfileLength(name, UMI_LOCAL_PROFILE_NAME_CAPACITY), out);
}

#endif

/* The original string-only implementation is retained for engineering review.
 * The shared byte-span validator above now owns validation and publication. */
#if 0
UmiStatus UmiLocalProfileName(const char *name, char out[UMI_LOCAL_PROFILE_NAME_CAPACITY])
{
    char value[UMI_LOCAL_PROFILE_NAME_CAPACITY] = {0};
    size_t n = ProfileLength(name, sizeof(value));
    if (out == NULL || n < 3U || n >= sizeof(value)) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < n; ++i) {
        unsigned char c = (unsigned char)name[i];
        if (c >= 'A' && c <= 'Z') c = (unsigned char)(c + ('a' - 'A'));
        int letter = c >= 'a' && c <= 'z';
        if (!letter && (i == 0U || !((c >= '0' && c <= '9') || c == '-' || c == '_')))
            return UMI_STATUS_INVALID_ARGUMENT;
        value[i] = (char)c;
    }
    memcpy(out, value, sizeof(value));
    return UMI_STATUS_OK;
}
#endif

UmiStatus UmiLocalProfileStoreCreate(const UmiLocalProfileBackend *backend, UmiLocalProfileStore **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (backend == NULL || backend->read == NULL || backend->create == NULL ||
        backend->remove == NULL || backend->random == NULL || backend->derive == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLocalProfileStore *store = calloc(1U, sizeof(*store));
    if (store == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    atomic_init(&store->references, 1U); store->backend = *backend;
    *out = store; return UMI_STATUS_OK;
}
void UmiLocalProfileStoreRetain(UmiLocalProfileStore *store)
{ if (store != NULL) (void)atomic_fetch_add_explicit(&store->references, 1U, memory_order_relaxed); }
void UmiLocalProfileStoreRelease(UmiLocalProfileStore *store)
{
    if (store != NULL && atomic_fetch_sub_explicit(&store->references, 1U, memory_order_acq_rel) == 1U) {
        if (store->backend.destroy != NULL) store->backend.destroy(store->backend.context);
        umi_secret_clear(store, sizeof(*store)); free(store);
    }
}
static UmiStatus ProfileInput(UmiLocalProfileStore *store, const char *name, const char *password,
                              char canonical[UMI_LOCAL_PROFILE_NAME_CAPACITY], size_t *length)
{
    if (store == NULL || UmiLocalProfileName(name, canonical) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    *length = ProfileLength(password, UMI_LOCAL_PROFILE_PASSWORD_CAPACITY);
    return *length >= 12U && *length < UMI_LOCAL_PROFILE_PASSWORD_CAPACITY
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UmiStatus UmiLocalProfileRegister(UmiLocalProfileStore *store, const char *name, const char *password)
{
    char canonical[UMI_LOCAL_PROFILE_NAME_CAPACITY]; size_t length;
    UmiStatus status = ProfileInput(store, name, password, canonical, &length);
    if (status != UMI_STATUS_OK) return status;
    UmiLocalProfileRecord record = {0};
    status = store->backend.read(store->backend.context, canonical, &record);
    umi_secret_clear(&record, sizeof(record));
    if (status != UMI_STATUS_NOT_FOUND)
        return status == UMI_STATUS_OK ? UMI_STATUS_ALREADY_EXISTS : status;
    record.version = 1U; record.iterations = UMI_LOCAL_PROFILE_ITERATIONS;
    status = store->backend.random(store->backend.context, record.salt, sizeof(record.salt));
    if (status == UMI_STATUS_OK)
        status = store->backend.derive(store->backend.context, password, length, &record, record.verifier);
    if (status == UMI_STATUS_OK)
        status = store->backend.create(store->backend.context, canonical, &record);
    umi_secret_clear(&record, sizeof(record)); return status;
}
static UmiStatus ProfileCheck(UmiLocalProfileStore *store, const char *name, const char *password, int remove)
{
    char canonical[UMI_LOCAL_PROFILE_NAME_CAPACITY]; size_t length;
    UmiStatus status = ProfileInput(store, name, password, canonical, &length);
    if (status != UMI_STATUS_OK) return status;
    UmiLocalProfileRecord record = {0}; unsigned char actual[32] = {0};
    status = store->backend.read(store->backend.context, canonical, &record);
    if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_PERMISSION_DENIED;
    if (status == UMI_STATUS_OK && (record.version != 1U || record.iterations != UMI_LOCAL_PROFILE_ITERATIONS))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = store->backend.derive(store->backend.context, password, length, &record, actual);
    if (status == UMI_STATUS_OK) {
        /* Examine every byte; do not stop at the first unequal verifier byte. */
        volatile unsigned int difference = 0U;
        for (size_t i = 0U; i < sizeof(actual); ++i) difference |= (unsigned int)(actual[i] ^ record.verifier[i]);
        if (difference != 0U) status = UMI_STATUS_PERMISSION_DENIED;
    }
    if (status == UMI_STATUS_OK && remove)
        status = store->backend.remove(store->backend.context, canonical, &record);
    umi_secret_clear(actual, sizeof(actual)); umi_secret_clear(&record, sizeof(record)); return status;
}
UmiStatus UmiLocalProfileVerify(UmiLocalProfileStore *store, const char *name, const char *password)
{ return ProfileCheck(store, name, password, 0); }
UmiStatus UmiLocalProfileRemove(UmiLocalProfileStore *store, const char *name, const char *password)
{ return ProfileCheck(store, name, password, 1); }
