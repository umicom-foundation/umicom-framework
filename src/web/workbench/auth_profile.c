/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/workbench/auth_profile.c
 *
 * PURPOSE:
 *   Implement credential-free authentication profiles and transient request
 *   application. Secret values are never retained in catalogue state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/workbench/auth_profile.h"

#include <stdio.h>
#include <string.h>

static size_t profile_index(
    const UmiWebWorkbenchAuthCatalogue *catalogue,
    const char *profile_id)
{
    size_t index;
    for (index = 0U; index < catalogue->profile_count; ++index) {
        if (strcmp(catalogue->profiles[index].profile_id, profile_id) == 0) return index;
    }
    return catalogue->profile_count;
}

void umi_web_workbench_auth_profile_init(
    UmiWebWorkbenchAuthProfile *profile,
    const char *profile_id,
    const char *name,
    UmiWebWorkbenchAuthKind kind,
    const char *secret_reference)
{
    if (profile == NULL) return;
    memset(profile, 0, sizeof(*profile));
    (void)umi_web_workbench_copy_text(profile->profile_id,
        sizeof(profile->profile_id), profile_id != NULL ? profile_id : "auth");
    (void)umi_web_workbench_copy_text(profile->name,
        sizeof(profile->name), name != NULL ? name : "Authentication");
    (void)umi_web_workbench_copy_text(profile->secret_reference,
        sizeof(profile->secret_reference), secret_reference != NULL ? secret_reference : "");
    profile->kind = kind;
    profile->enabled = true;
    if (kind == UMI_WEB_WORKBENCH_AUTH_API_KEY_HEADER) {
        (void)umi_web_workbench_copy_text(profile->field_name,
            sizeof(profile->field_name), "X-API-Key");
    } else if (kind == UMI_WEB_WORKBENCH_AUTH_API_KEY_QUERY) {
        (void)umi_web_workbench_copy_text(profile->field_name,
            sizeof(profile->field_name), "api_key");
    } else {
        (void)umi_web_workbench_copy_text(profile->field_name,
            sizeof(profile->field_name), "Authorization");
    }
}

UmiStatus umi_web_workbench_auth_profile_validate(
    const UmiWebWorkbenchAuthProfile *profile)
{
    if (profile == NULL || profile->profile_id[0] == '\0' || profile->name[0] == '\0' ||
        profile->kind > UMI_WEB_WORKBENCH_AUTH_OAUTH_ACCESS_TOKEN) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (profile->kind != UMI_WEB_WORKBENCH_AUTH_NONE &&
        (profile->secret_reference[0] == '\0' || profile->field_name[0] == '\0')) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (profile->kind == UMI_WEB_WORKBENCH_AUTH_BASIC &&
        profile->public_identity[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

void umi_web_workbench_auth_catalogue_init(
    UmiWebWorkbenchAuthCatalogue *catalogue)
{
    if (catalogue == NULL) return;
    memset(catalogue, 0, sizeof(*catalogue));
    catalogue->revision = 1U;
}

UmiStatus umi_web_workbench_auth_catalogue_upsert(
    UmiWebWorkbenchAuthCatalogue *catalogue,
    const UmiWebWorkbenchAuthProfile *profile)
{
    size_t index;
    UmiStatus status = umi_web_workbench_auth_profile_validate(profile);
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (status != UMI_STATUS_OK) return status;
    index = profile_index(catalogue, profile->profile_id);
    if (index == catalogue->profile_count) {
        if (index >= UMI_WEB_WORKBENCH_MAX_AUTH_PROFILES) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        catalogue->profile_count++;
    }
    catalogue->profiles[index] = *profile;
    catalogue->revision++;
    return UMI_STATUS_OK;
}

const UmiWebWorkbenchAuthProfile *umi_web_workbench_auth_catalogue_find(
    const UmiWebWorkbenchAuthCatalogue *catalogue,
    const char *profile_id)
{
    size_t index;
    if (catalogue == NULL || profile_id == NULL) return NULL;
    index = profile_index(catalogue, profile_id);
    return index < catalogue->profile_count ? &catalogue->profiles[index] : NULL;
}

static UmiStatus encode_base64(
    const unsigned char *source,
    size_t length,
    char *out_text,
    size_t capacity)
{
    static const char alphabet[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t input = 0U;
    size_t output = 0U;
    if (capacity < ((length + 2U) / 3U) * 4U + 1U) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    while (input < length) {
        uint32_t value = (uint32_t)source[input++] << 16U;
        bool have_second = input < length;
        bool have_third;
        if (have_second) value |= (uint32_t)source[input++] << 8U;
        have_third = input < length;
        if (have_third) value |= (uint32_t)source[input++];
        out_text[output++] = alphabet[(value >> 18U) & 63U];
        out_text[output++] = alphabet[(value >> 12U) & 63U];
        out_text[output++] = have_second ? alphabet[(value >> 6U) & 63U] : '=';
        out_text[output++] = have_third ? alphabet[value & 63U] : '=';
    }
    out_text[output] = '\0';
    return UMI_STATUS_OK;
}

static int query_safe(unsigned char value)
{
    return (value >= (unsigned char)'a' && value <= (unsigned char)'z') ||
           (value >= (unsigned char)'A' && value <= (unsigned char)'Z') ||
           (value >= (unsigned char)'0' && value <= (unsigned char)'9') ||
           value == (unsigned char)'-' || value == (unsigned char)'_' ||
           value == (unsigned char)'.' || value == (unsigned char)'~';
}

static UmiStatus append_query_secret(
    UmiWebWorkbenchRequest *request,
    const char *name,
    const char *secret)
{
    static const char hex[] = "0123456789ABCDEF";
    char encoded[UMI_WEB_HEADER_VALUE_CAPACITY * 3U];
    size_t output = 0U;
    const unsigned char *cursor = (const unsigned char *)secret;
    size_t url_length = strlen(request->url);
    while (*cursor != 0U) {
        if (query_safe(*cursor)) {
            if (output + 1U >= sizeof(encoded)) return UMI_STATUS_CAPACITY_EXCEEDED;
            encoded[output++] = (char)*cursor;
        } else {
            if (output + 3U >= sizeof(encoded)) return UMI_STATUS_CAPACITY_EXCEEDED;
            encoded[output++] = '%';
            encoded[output++] = hex[(*cursor >> 4U) & 15U];
            encoded[output++] = hex[*cursor & 15U];
        }
        ++cursor;
    }
    encoded[output] = '\0';
    if (url_length + 1U + strlen(name) + 1U + output >= sizeof(request->url)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    request->url[url_length++] = strchr(request->url, '?') != NULL ? '&' : '?';
    request->url[url_length] = '\0';
    (void)strcat(request->url, name);
    (void)strcat(request->url, "=");
    (void)strcat(request->url, encoded);
    request->revision++;
    return UMI_STATUS_OK;
}

UmiStatus umi_web_workbench_auth_apply(
    const UmiWebWorkbenchAuthProfile *profile,
    const char *transient_secret,
    UmiWebWorkbenchRequest *request)
{
    char value[UMI_WEB_HEADER_VALUE_CAPACITY];
    UmiStatus status;
    if (request == NULL || profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_web_workbench_auth_profile_validate(profile);
    if (status != UMI_STATUS_OK) return status;
    if (!profile->enabled || profile->kind == UMI_WEB_WORKBENCH_AUTH_NONE) {
        return UMI_STATUS_OK;
    }
    if (transient_secret == NULL || transient_secret[0] == '\0') {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    if (profile->kind == UMI_WEB_WORKBENCH_AUTH_API_KEY_QUERY) {
        return append_query_secret(request, profile->field_name, transient_secret);
    }
    if (profile->kind == UMI_WEB_WORKBENCH_AUTH_BASIC) {
        char credentials[UMI_WEB_HEADER_VALUE_CAPACITY];
        char encoded[UMI_WEB_HEADER_VALUE_CAPACITY];
        int written = snprintf(credentials, sizeof(credentials), "%s:%s",
            profile->public_identity, transient_secret);
        if (written < 0 || (size_t)written >= sizeof(credentials)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        status = encode_base64((const unsigned char *)credentials,
            (size_t)written, encoded, sizeof(encoded));
        if (status != UMI_STATUS_OK) return status;
        written = snprintf(value, sizeof(value), "Basic %s", encoded);
        memset(credentials, 0, sizeof(credentials));
        memset(encoded, 0, sizeof(encoded));
        if (written < 0 || (size_t)written >= sizeof(value)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    } else if (profile->kind == UMI_WEB_WORKBENCH_AUTH_BEARER ||
               profile->kind == UMI_WEB_WORKBENCH_AUTH_OAUTH_ACCESS_TOKEN) {
        int written = snprintf(value, sizeof(value), "Bearer %s", transient_secret);
        if (written < 0 || (size_t)written >= sizeof(value)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    } else {
        status = umi_web_workbench_copy_text(value, sizeof(value), transient_secret);
        if (status != UMI_STATUS_OK) return status;
    }
    status = umi_web_workbench_request_set_header(request, profile->field_name, value);
    memset(value, 0, sizeof(value));
    return status;
}
