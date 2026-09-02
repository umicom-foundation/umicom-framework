/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/topic.h
 *
 * PURPOSE:
 *   Declare a canonical topic registry that binds topic names to schema identifiers, partitions and durability policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_TOPIC_H
#define UMICOM_MESSAGING_TOPIC_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the topic descriptor data shared with callers of this public contract.
 */
typedef struct UmiTopicDescriptor {
    uint32_t structure_size;
    const char *topic;
    const char *schema_id;
    uint32_t partitions;
    int durable;
} UmiTopicDescriptor;

/**
 * Represent the topic registry data shared with callers of this public contract.
 */
typedef struct UmiTopicRegistry UmiTopicRegistry;

/**
 * Initialise topic registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_topic_registry_create(size_t capacity,
                                    UmiTopicRegistry **out_registry);
/**
 * Release or reset state held by topic registry so the same storage can be reused safely.
 */
void umi_topic_registry_destroy(UmiTopicRegistry *registry);
/**
 * Add topic registry only after its inputs and available capacity have been checked.
 */
UmiStatus umi_topic_registry_register(UmiTopicRegistry *registry,
                                      const UmiTopicDescriptor *topic);
/**
 * Find topic registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiTopicDescriptor *umi_topic_registry_find(
    const UmiTopicRegistry *registry,
    const char *topic);
/**
 * Return the number of records represented by topic registry without changing their state.
 */
size_t umi_topic_registry_count(const UmiTopicRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
