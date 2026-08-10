/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/aggregator.c
 *
 * PURPOSE:
 *   Implement bounded correlation groups, owned partial messages, completion views and explicit group release.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/aggregator.h"

#include <stdlib.h>
#include <string.h>

typedef struct UmiAggregateGroup {
    uint64_t correlation_id;
    UmiOwnedMessage *messages;
    size_t count;
    int used;
} UmiAggregateGroup;

struct UmiMessageAggregator {
    UmiAggregateGroup *groups;
    size_t group_capacity;
    size_t messages_per_group;
    size_t group_count;
};

UmiStatus umi_message_aggregator_create(size_t group_capacity,
                                        size_t messages_per_group,
                                        UmiMessageAggregator **out_aggregator)
{
    UmiMessageAggregator *aggregator;
    size_t index;
    if (group_capacity == 0U || messages_per_group == 0U ||
        out_aggregator == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_aggregator = NULL;
    aggregator = (UmiMessageAggregator *)calloc(1U, sizeof(*aggregator));
    if (aggregator == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    aggregator->groups = (UmiAggregateGroup *)calloc(group_capacity,
                                                      sizeof(*aggregator->groups));
    if (aggregator->groups == NULL) {
        free(aggregator);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (index = 0U; index < group_capacity; ++index) {
        aggregator->groups[index].messages =
            (UmiOwnedMessage *)calloc(messages_per_group,
                                      sizeof(UmiOwnedMessage));
        if (aggregator->groups[index].messages == NULL) {
            while (index > 0U) free(aggregator->groups[--index].messages);
            free(aggregator->groups);
            free(aggregator);
            return UMI_STATUS_OUT_OF_MEMORY;
        }
    }
    aggregator->group_capacity = group_capacity;
    aggregator->messages_per_group = messages_per_group;
    *out_aggregator = aggregator;
    return UMI_STATUS_OK;
}

static void clear_group(UmiAggregateGroup *group)
{
    size_t index;
    for (index = 0U; index < group->count; ++index) {
        umi_message_dispose(&group->messages[index]);
    }
    group->correlation_id = 0U;
    group->count = 0U;
    group->used = 0;
}

void umi_message_aggregator_destroy(UmiMessageAggregator *aggregator)
{
    size_t index;
    if (aggregator == NULL) return;
    for (index = 0U; index < aggregator->group_capacity; ++index) {
        clear_group(&aggregator->groups[index]);
        free(aggregator->groups[index].messages);
    }
    free(aggregator->groups);
    free(aggregator);
}

static UmiAggregateGroup *find_or_create(UmiMessageAggregator *aggregator,
                                         uint64_t correlation_id)
{
    size_t index;
    UmiAggregateGroup *free_group = NULL;
    for (index = 0U; index < aggregator->group_capacity; ++index) {
        UmiAggregateGroup *group = &aggregator->groups[index];
        if (group->used && group->correlation_id == correlation_id) {
            return group;
        }
        if (!group->used && free_group == NULL) free_group = group;
    }
    if (free_group != NULL) {
        free_group->used = 1;
        free_group->correlation_id = correlation_id;
        aggregator->group_count++;
    }
    return free_group;
}

UmiStatus umi_message_aggregator_add(UmiMessageAggregator *aggregator,
                                     const UmiMessageEnvelope *message,
                                     UmiAggregateView *out_complete_group)
{
    UmiAggregateGroup *group;
    UmiStatus status;
    if (aggregator == NULL || message == NULL || message->correlation_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (out_complete_group != NULL) {
        (void)memset(out_complete_group, 0, sizeof(*out_complete_group));
    }
    group = find_or_create(aggregator, message->correlation_id);
    if (group == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (group->count >= aggregator->messages_per_group) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = umi_message_copy(message, &group->messages[group->count]);
    if (status != UMI_STATUS_OK) return status;
    group->count++;
    if (group->count == aggregator->messages_per_group &&
        out_complete_group != NULL) {
        out_complete_group->correlation_id = group->correlation_id;
        out_complete_group->messages = group->messages;
        out_complete_group->count = group->count;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_message_aggregator_release(UmiMessageAggregator *aggregator,
                                         uint64_t correlation_id)
{
    size_t index;
    if (aggregator == NULL || correlation_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < aggregator->group_capacity; ++index) {
        UmiAggregateGroup *group = &aggregator->groups[index];
        if (group->used && group->correlation_id == correlation_id) {
            clear_group(group);
            aggregator->group_count--;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

size_t umi_message_aggregator_group_count(
    const UmiMessageAggregator *aggregator)
{
    return aggregator != NULL ? aggregator->group_count : 0U;
}
