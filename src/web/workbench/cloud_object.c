/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/workbench/cloud_object.c
 *
 * PURPOSE:
 *   Implement provider-neutral cloud profiles and object inventory models.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/workbench/cloud_object.h"
#include "../../base/value_archive_internal.h"

#include <ctype.h>
#include <string.h>

/* Provide the object index operation used by this module and its client applications. */
static size_t object_index(
    const UmiWebWorkbenchCloudObjectModel *model,
    const char *bucket,
    const char *key)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->object_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->objects[index].bucket, bucket) == 0 &&
            strcmp(model->objects[index].key, key) == 0) return index;
    }
    return model->object_count;
}

/*
 * Provide the contains case insensitive operation used by this module and its client
 * applications.
 */
static int contains_case_insensitive(const char *text, const char *query)
{
    size_t length;
    const char *cursor;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || query[0] == '\0') return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0;
    length = strlen(query);
    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = text; *cursor != '\0'; ++cursor) {
        size_t index;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < length; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (cursor[index] == '\0' ||
                tolower((unsigned char)cursor[index]) !=
                tolower((unsigned char)query[index])) break;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index == length) return 1;
    }
    return 0;
}

/*
 * Initialise web workbench cloud profile from caller-provided values so later operations
 * receive a known state.
 */
void umi_web_workbench_cloud_profile_init(
    UmiWebWorkbenchCloudProfile *profile,
    const char *profile_id,
    const char *name,
    UmiWebWorkbenchCloudProvider provider)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL) return;
    memset(profile, 0, sizeof(*profile));
    (void)umi_web_workbench_copy_text(profile->profile_id,
        sizeof(profile->profile_id), profile_id != NULL ? profile_id : "cloud");
    (void)umi_web_workbench_copy_text(profile->name,
        sizeof(profile->name), name != NULL ? name : "Cloud Profile");
    profile->provider = provider;
    profile->verify_tls = true;
    profile->enabled = true;
}

/*
 * Check that web workbench cloud profile satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_web_workbench_cloud_profile_validate(
    const UmiWebWorkbenchCloudProfile *profile)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(profile->profile_id, '\0', sizeof(profile->profile_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(profile->name, '\0', sizeof(profile->name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(profile->region, '\0', sizeof(profile->region)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(profile->endpoint, '\0', sizeof(profile->endpoint)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(profile->secret_reference, '\0', sizeof(profile->secret_reference)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || profile->profile_id[0] == '\0' || profile->name[0] == '\0' ||
        profile->provider > UMI_WEB_WORKBENCH_CLOUD_LOCAL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (profile->provider != UMI_WEB_WORKBENCH_CLOUD_LOCAL &&
        profile->secret_reference[0] == '\0') return UMI_STATUS_PERMISSION_DENIED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (profile->endpoint[0] != '\0' && !profile->verify_tls &&
        strncmp(profile->endpoint, "https://", 8U) == 0) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    return UMI_STATUS_OK;
}

/*
 * Initialise web workbench cloud object model from caller-provided values so later
 * operations receive a known state.
 */
void umi_web_workbench_cloud_object_model_init(
    UmiWebWorkbenchCloudObjectModel *model,
    const UmiWebWorkbenchCloudProfile *profile)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    memset(model, 0, sizeof(*model));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile != NULL) model->profile = *profile;
    model->revision = 1U;
}

/*
 * Provide the web workbench cloud object upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_web_workbench_cloud_object_upsert(
    UmiWebWorkbenchCloudObjectModel *model,
    const UmiWebWorkbenchCloudObject *object)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || object == NULL || object->bucket[0] == '\0' ||
        object->key[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = object_index(model, object->bucket, object->key);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == model->object_count) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index >= UMI_WEB_WORKBENCH_MAX_CLOUD_ITEMS) {
            model->omitted_count++;
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        model->object_count++;
    }
    model->objects[index] = *object;
    model->revision++;
    return UMI_STATUS_OK;
}

/*
 * Remove web workbench cloud object while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_web_workbench_cloud_object_remove(
    UmiWebWorkbenchCloudObjectModel *model,
    const char *bucket,
    const char *key)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || bucket == NULL || key == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = object_index(model, bucket, key);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == model->object_count) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < model->object_count) {
        memmove(&model->objects[index], &model->objects[index + 1U],
            (model->object_count - index - 1U) * sizeof(model->objects[0]));
    }
    model->object_count--;
    memset(&model->objects[model->object_count], 0, sizeof(model->objects[0]));
    model->revision++;
    return UMI_STATUS_OK;
}

/*
 * Provide the web workbench cloud object query operation used by this module and its
 * client applications.
 */
size_t umi_web_workbench_cloud_object_query(
    const UmiWebWorkbenchCloudObjectModel *model,
    const char *bucket,
    const char *text,
    const UmiWebWorkbenchCloudObject **out_objects,
    size_t capacity)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || (out_objects == NULL && capacity > 0U)) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->object_count && count < capacity; ++index) {
        const UmiWebWorkbenchCloudObject *object = &model->objects[index];
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (bucket != NULL && bucket[0] != '\0' && strcmp(object->bucket, bucket) != 0) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!contains_case_insensitive(object->key, text) &&
            !contains_case_insensitive(object->content_type, text)) continue;
        out_objects[count++] = object;
    }
    return count;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiWebWorkbenchCloudProfileArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd672f8b3bf7c2998);
    schema = (schema ^ (uint64_t)sizeof(((UmiWebWorkbenchCloudProfile *)0)->profile_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWebWorkbenchCloudProfile *)0)->name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWebWorkbenchCloudProfile *)0)->region)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWebWorkbenchCloudProfile *)0)->endpoint)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWebWorkbenchCloudProfile *)0)->secret_reference)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiWebWorkbenchCloudProfileArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiWebWorkbenchCloudProfile *)0)->profile_id) - 1U +
        8U + sizeof(((UmiWebWorkbenchCloudProfile *)0)->name) - 1U +
        8U +
        8U + sizeof(((UmiWebWorkbenchCloudProfile *)0)->region) - 1U +
        8U + sizeof(((UmiWebWorkbenchCloudProfile *)0)->endpoint) - 1U +
        8U + sizeof(((UmiWebWorkbenchCloudProfile *)0)->secret_reference) - 1U +
        8U +
        8U;
}
static void UmiWebWorkbenchCloudProfileArchiveWrite(UmiArchiveWriter *writer, const UmiWebWorkbenchCloudProfile *value)
{
    UmiArchiveWriteText(writer, value->profile_id, sizeof(value->profile_id));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteSigned(writer, (int64_t)value->provider);
    UmiArchiveWriteText(writer, value->region, sizeof(value->region));
    UmiArchiveWriteText(writer, value->endpoint, sizeof(value->endpoint));
    UmiArchiveWriteText(writer, value->secret_reference, sizeof(value->secret_reference));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->verify_tls);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiWebWorkbenchCloudProfileArchiveRead(UmiArchiveReader *reader, UmiWebWorkbenchCloudProfile *value)
{
    UmiArchiveReadText(reader, value->profile_id, sizeof(value->profile_id));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    value->provider = (UmiWebWorkbenchCloudProvider)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->region, sizeof(value->region));
    UmiArchiveReadText(reader, value->endpoint, sizeof(value->endpoint));
    UmiArchiveReadText(reader, value->secret_reference, sizeof(value->secret_reference));
    value->verify_tls = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiWebWorkbenchCloudProfileArchiveValidate(const UmiWebWorkbenchCloudProfile *value)
{
    return umi_web_workbench_cloud_profile_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_web_workbench_cloud_profile_archive_encode, umi_web_workbench_cloud_profile_archive_decode,
    UmiWebWorkbenchCloudProfile, UmiWebWorkbenchCloudProfileArchiveSchema, UmiWebWorkbenchCloudProfileArchiveBound, UmiWebWorkbenchCloudProfileArchiveWrite, UmiWebWorkbenchCloudProfileArchiveRead, UmiWebWorkbenchCloudProfileArchiveValidate)
