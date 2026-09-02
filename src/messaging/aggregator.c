/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/aggregator.c
 *
 * PURPOSE:
 *   Implement bounded correlation groups, owned partial messages, completion views and explicit group release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/*
 * Initialise message aggregator from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_message_aggregator_create(size_t group_capacity,
                                        size_t messages_per_group,
                                        UmiMessageAggregator **out_aggregator)
{
    UmiMessageAggregator *aggregator;
    size_t index;
    /* Apply this branch only when its contract condition is satisfied. */
    if (group_capacity == 0U || messages_per_group == 0U ||
        out_aggregator == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_aggregator = NULL;
    aggregator = (UmiMessageAggregator *)calloc(1U, sizeof(*aggregator));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (aggregator == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    aggregator->groups = (UmiAggregateGroup *)calloc(group_capacity,
                                                      sizeof(*aggregator->groups));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (aggregator->groups == NULL) {
        free(aggregator);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < group_capacity; ++index) {
        aggregator->groups[index].messages =
            (UmiOwnedMessage *)calloc(messages_per_group,
                                      sizeof(UmiOwnedMessage));
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (aggregator->groups[index].messages == NULL) {
            /*
             * Continue only while work remains available; the loop body advances the state on each
             * pass.
             */
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

/* Provide the clear group operation used by this module and its client applications. */
static void clear_group(UmiAggregateGroup *group)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < group->count; ++index) {
        umi_message_dispose(&group->messages[index]);
    }
    group->correlation_id = 0U;
    group->count = 0U;
    group->used = 0;
}

/*
 * Release or reset state held by message aggregator so the same storage can be reused
 * safely.
 */
void umi_message_aggregator_destroy(UmiMessageAggregator *aggregator)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (aggregator == NULL) return;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < aggregator->group_capacity; ++index) {
        clear_group(&aggregator->groups[index]);
        free(aggregator->groups[index].messages);
    }
    free(aggregator->groups);
    free(aggregator);
}

/*
 * Initialise find or from caller-provided values so later operations receive a known
 * state.
 */
static UmiAggregateGroup *find_or_create(UmiMessageAggregator *aggregator,
                                         uint64_t correlation_id)
{
    size_t index;
    UmiAggregateGroup *free_group = NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < aggregator->group_capacity; ++index) {
        UmiAggregateGroup *group = &aggregator->groups[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (group->used && group->correlation_id == correlation_id) {
            return group;
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (!group->used && free_group == NULL) free_group = group;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (free_group != NULL) {
        free_group->used = 1;
        free_group->correlation_id = correlation_id;
        aggregator->group_count++;
    }
    return free_group;
}

/* Add message aggregator only after its inputs and available capacity have been checked. */
UmiStatus umi_message_aggregator_add(UmiMessageAggregator *aggregator,
                                     const UmiMessageEnvelope *message,
                                     UmiAggregateView *out_complete_group)
{
    UmiAggregateGroup *group;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (aggregator == NULL || message == NULL || message->correlation_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_complete_group != NULL) {
        (void)memset(out_complete_group, 0, sizeof(*out_complete_group));
    }
    group = find_or_create(aggregator, message->correlation_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (group == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (group->count >= aggregator->messages_per_group) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = umi_message_copy(message, &group->messages[group->count]);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    group->count++;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (group->count == aggregator->messages_per_group &&
        out_complete_group != NULL) {
        out_complete_group->correlation_id = group->correlation_id;
        out_complete_group->messages = group->messages;
        out_complete_group->count = group->count;
    }
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by message aggregator so the same storage can be reused
 * safely.
 */
UmiStatus umi_message_aggregator_release(UmiMessageAggregator *aggregator,
                                         uint64_t correlation_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (aggregator == NULL || correlation_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < aggregator->group_capacity; ++index) {
        UmiAggregateGroup *group = &aggregator->groups[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (group->used && group->correlation_id == correlation_id) {
            clear_group(group);
            aggregator->group_count--;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Return the number of records represented by message aggregator group without changing
 * their state.
 */
size_t umi_message_aggregator_group_count(
    const UmiMessageAggregator *aggregator)
{
    return aggregator != NULL ? aggregator->group_count : 0U;
}
