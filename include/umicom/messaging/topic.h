/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/topic.h
 *
 * PURPOSE:
 *   Declare a canonical topic registry that binds topic names to schema identifiers, partitions and durability policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_TOPIC_H
#define UMICOM_MESSAGING_TOPIC_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTopicDescriptor {
    uint32_t structure_size;
    const char *topic;
    const char *schema_id;
    uint32_t partitions;
    int durable;
} UmiTopicDescriptor;

typedef struct UmiTopicRegistry UmiTopicRegistry;

UmiStatus umi_topic_registry_create(size_t capacity,
                                    UmiTopicRegistry **out_registry);
void umi_topic_registry_destroy(UmiTopicRegistry *registry);
UmiStatus umi_topic_registry_register(UmiTopicRegistry *registry,
                                      const UmiTopicDescriptor *topic);
const UmiTopicDescriptor *umi_topic_registry_find(
    const UmiTopicRegistry *registry,
    const char *topic);
size_t umi_topic_registry_count(const UmiTopicRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
