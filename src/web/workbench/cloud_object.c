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

#include <ctype.h>
#include <string.h>

static size_t object_index(
    const UmiWebWorkbenchCloudObjectModel *model,
    const char *bucket,
    const char *key)
{
    size_t index;
    for (index = 0U; index < model->object_count; ++index) {
        if (strcmp(model->objects[index].bucket, bucket) == 0 &&
            strcmp(model->objects[index].key, key) == 0) return index;
    }
    return model->object_count;
}

static int contains_case_insensitive(const char *text, const char *query)
{
    size_t length;
    const char *cursor;
    if (query == NULL || query[0] == '\0') return 1;
    if (text == NULL) return 0;
    length = strlen(query);
    for (cursor = text; *cursor != '\0'; ++cursor) {
        size_t index;
        for (index = 0U; index < length; ++index) {
            if (cursor[index] == '\0' ||
                tolower((unsigned char)cursor[index]) !=
                tolower((unsigned char)query[index])) break;
        }
        if (index == length) return 1;
    }
    return 0;
}

void umi_web_workbench_cloud_profile_init(
    UmiWebWorkbenchCloudProfile *profile,
    const char *profile_id,
    const char *name,
    UmiWebWorkbenchCloudProvider provider)
{
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

UmiStatus umi_web_workbench_cloud_profile_validate(
    const UmiWebWorkbenchCloudProfile *profile)
{
    if (profile == NULL || profile->profile_id[0] == '\0' || profile->name[0] == '\0' ||
        profile->provider > UMI_WEB_WORKBENCH_CLOUD_LOCAL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (profile->provider != UMI_WEB_WORKBENCH_CLOUD_LOCAL &&
        profile->secret_reference[0] == '\0') return UMI_STATUS_PERMISSION_DENIED;
    if (profile->endpoint[0] != '\0' && !profile->verify_tls &&
        strncmp(profile->endpoint, "https://", 8U) == 0) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    return UMI_STATUS_OK;
}

void umi_web_workbench_cloud_object_model_init(
    UmiWebWorkbenchCloudObjectModel *model,
    const UmiWebWorkbenchCloudProfile *profile)
{
    if (model == NULL) return;
    memset(model, 0, sizeof(*model));
    if (profile != NULL) model->profile = *profile;
    model->revision = 1U;
}

UmiStatus umi_web_workbench_cloud_object_upsert(
    UmiWebWorkbenchCloudObjectModel *model,
    const UmiWebWorkbenchCloudObject *object)
{
    size_t index;
    if (model == NULL || object == NULL || object->bucket[0] == '\0' ||
        object->key[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = object_index(model, object->bucket, object->key);
    if (index == model->object_count) {
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

UmiStatus umi_web_workbench_cloud_object_remove(
    UmiWebWorkbenchCloudObjectModel *model,
    const char *bucket,
    const char *key)
{
    size_t index;
    if (model == NULL || bucket == NULL || key == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = object_index(model, bucket, key);
    if (index == model->object_count) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < model->object_count) {
        memmove(&model->objects[index], &model->objects[index + 1U],
            (model->object_count - index - 1U) * sizeof(model->objects[0]));
    }
    model->object_count--;
    memset(&model->objects[model->object_count], 0, sizeof(model->objects[0]));
    model->revision++;
    return UMI_STATUS_OK;
}

size_t umi_web_workbench_cloud_object_query(
    const UmiWebWorkbenchCloudObjectModel *model,
    const char *bucket,
    const char *text,
    const UmiWebWorkbenchCloudObject **out_objects,
    size_t capacity)
{
    size_t index;
    size_t count = 0U;
    if (model == NULL || (out_objects == NULL && capacity > 0U)) return 0U;
    for (index = 0U; index < model->object_count && count < capacity; ++index) {
        const UmiWebWorkbenchCloudObject *object = &model->objects[index];
        if (bucket != NULL && bucket[0] != '\0' && strcmp(object->bucket, bucket) != 0) continue;
        if (!contains_case_insensitive(object->key, text) &&
            !contains_case_insensitive(object->content_type, text)) continue;
        out_objects[count++] = object;
    }
    return count;
}
