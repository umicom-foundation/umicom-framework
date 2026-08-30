/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/topic.c
 *
 * PURPOSE:
 *   Implement owned topic metadata registration, duplicate protection and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/topic.h"

#include <stdlib.h>
#include <string.h>

#include "include/messaging_internal.h"

typedef struct UmiOwnedTopic {
    UmiTopicDescriptor descriptor;
    char *topic;
    char *schema;
} UmiOwnedTopic;

struct UmiTopicRegistry {
    UmiOwnedTopic *topics;
    size_t capacity;
    size_t count;
};

UmiStatus umi_topic_registry_create(size_t capacity,
                                    UmiTopicRegistry **out_registry)
{
    UmiTopicRegistry *registry;
    if (capacity == 0U || out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiTopicRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->topics = (UmiOwnedTopic *)calloc(capacity,
                                               sizeof(*registry->topics));
    if (registry->topics == NULL) {
        free(registry);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    registry->capacity = capacity;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_topic_registry_destroy(UmiTopicRegistry *registry)
{
    size_t index;
    if (registry == NULL) return;
    for (index = 0U; index < registry->count; ++index) {
        free(registry->topics[index].topic);
        free(registry->topics[index].schema);
    }
    free(registry->topics);
    free(registry);
}

UmiStatus umi_topic_registry_register(UmiTopicRegistry *registry,
                                      const UmiTopicDescriptor *topic)
{
    UmiOwnedTopic *entry;
    char *topic_copy;
    char *schema_copy;
    if (registry == NULL || topic == NULL || topic->topic == NULL ||
        topic->topic[0] == '\0' || topic->partitions == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_topic_registry_find(registry, topic->topic) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (registry->count >= registry->capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    topic_copy = umi_message_strdup(topic->topic);
    schema_copy = umi_message_strdup(topic->schema_id != NULL
                                     ? topic->schema_id : "");
    if (topic_copy == NULL || schema_copy == NULL) {
        free(topic_copy);
        free(schema_copy);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    entry = &registry->topics[registry->count++];
    entry->descriptor = *topic;
    entry->descriptor.structure_size = (uint32_t)sizeof(entry->descriptor);
    entry->topic = topic_copy;
    entry->schema = schema_copy;
    entry->descriptor.topic = topic_copy;
    entry->descriptor.schema_id = schema_copy;
    return UMI_STATUS_OK;
}

const UmiTopicDescriptor *umi_topic_registry_find(
    const UmiTopicRegistry *registry,
    const char *topic)
{
    size_t index;
    if (registry == NULL || topic == NULL) return NULL;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->topics[index].topic, topic) == 0) {
            return &registry->topics[index].descriptor;
        }
    }
    return NULL;
}

size_t umi_topic_registry_count(const UmiTopicRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
