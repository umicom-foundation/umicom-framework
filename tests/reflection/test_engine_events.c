/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/reflection/test_engine_events.c
 *
 * PURPOSE:
 *   Verify that reflection changes reach subscribers and are recorded by the
 *   reusable durable change broker before they become visible.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

#include "umicom/messaging/message.h"
#include "umicom/platform/directory.h"
#include "umicom/platform/filesystem.h"
#include "umicom/reflection/reflection.h"

typedef struct ReflectionEventCapture {
    size_t count;
    char topic[UMI_REFLECTION_ID_CAPACITY];
} ReflectionEventCapture;

/* Copy the notification while it is valid instead of retaining bus memory. */
static void capture_event(const UmiMessageEnvelope *event, void *user_data)
{
    ReflectionEventCapture *capture = (ReflectionEventCapture *)user_data;

    capture->count += 1U;
    (void)snprintf(capture->topic,
                   sizeof(capture->topic),
                   "%s",
                   event->name);
}

/* Report one failed expectation and let main perform normal cleanup. */
static int require_true(int condition, int line)
{
    if (!condition) {
        (void)fprintf(stderr, "reflection event failure at line %d\n", line);
        return 0;
    }
    return 1;
}

#define REQUIRE(CONDITION)                                                   \
    do {                                                                     \
        if (!require_true((CONDITION), __LINE__)) {                          \
            result = 1;                                                      \
            goto cleanup;                                                    \
        }                                                                    \
    } while (0)

int main(void)
{
    UmiChangeBroker *broker = NULL;
    UmiReflectionEngine *engine = NULL;
    UmiChangeBrokerConfig broker_config;
    UmiReflectionEngineConfig engine_config;
    UmiReflectionResourceDescriptor resource;
    ReflectionEventCapture capture;
    UmiFileInfo journal_info;
    char temporary_directory[UMI_PATH_CAPACITY] = {0};
    char journal_name[96] = {0};
    char journal_path[UMI_PATH_CAPACITY] = {0};
    size_t replay_count = 0U;
    uint64_t replay_checkpoint = 0U;
    int result = 0;

    (void)memset(&capture, 0, sizeof(capture));
    REQUIRE(umi_fs_temp_directory(
                temporary_directory,
                sizeof(temporary_directory)) == UMI_STATUS_OK);
    (void)snprintf(journal_name,
                   sizeof(journal_name),
                   "umicom-reflection-%llu.journal",
                   (unsigned long long)umi_message_next_id());
    REQUIRE(umi_fs_join(journal_path,
                        sizeof(journal_path),
                        temporary_directory,
                        journal_name) == UMI_STATUS_OK);

    broker_config = umi_change_broker_config_default();
    broker_config.journal_path = journal_path;
    broker_config.source_id = "umicom.reflection.test";
    REQUIRE(umi_change_broker_create(&broker_config, &broker) == UMI_STATUS_OK);
    REQUIRE(umi_change_broker_subscribe(
                broker,
                UMI_REFLECTION_TOPIC_RESOURCE_REGISTERED,
                capture_event,
                &capture) == UMI_STATUS_OK);

    engine_config = umi_reflection_engine_config_default();
    engine_config.change_broker = broker;
    engine_config.require_change_record = 1;
    REQUIRE(umi_reflection_engine_create(&engine_config, &engine) ==
            UMI_STATUS_OK);

    (void)memset(&resource, 0, sizeof(resource));
    resource.structure_size = (uint32_t)sizeof(resource);
    resource.kind = UMI_REFLECTION_RESOURCE_LAYOUT;
    resource.revision = 1U;
    (void)snprintf(resource.resource_id,
                   sizeof(resource.resource_id),
                   "%s",
                   "layout.test.dynamic");
    (void)snprintf(resource.module_id,
                   sizeof(resource.module_id),
                   "%s",
                   "org.umicom.test");
    (void)snprintf(resource.display_name,
                   sizeof(resource.display_name),
                   "%s",
                   "Dynamic Test Layout");
    (void)snprintf(resource.description,
                   sizeof(resource.description),
                   "%s",
                   "Layout used to verify reflected change publication.");
    (void)snprintf(resource.resource_uri,
                   sizeof(resource.resource_uri),
                   "%s",
                   "memory://layout.test.dynamic");
    (void)snprintf(resource.media_type,
                   sizeof(resource.media_type),
                   "%s",
                   "umicom.layout");

    REQUIRE(umi_reflection_engine_register_resource(engine, &resource) ==
            UMI_STATUS_OK);
    REQUIRE(capture.count == 1U);
    REQUIRE(strcmp(capture.topic,
                   UMI_REFLECTION_TOPIC_RESOURCE_REGISTERED) == 0);
    REQUIRE(umi_change_broker_last_sequence(broker) == 1U);

    /* Reopening the same durable channel must continue after its last record
     * rather than publishing another event with sequence one. */
    umi_reflection_engine_destroy(engine);
    engine = NULL;
    umi_change_broker_destroy(broker);
    broker = NULL;
    REQUIRE(umi_change_broker_create(&broker_config, &broker) == UMI_STATUS_OK);
    REQUIRE(umi_change_broker_last_sequence(broker) == 1U);
    REQUIRE(umi_change_broker_subscribe(
                broker,
                UMI_REFLECTION_TOPIC_RESOURCE_REGISTERED,
                capture_event,
                &capture) == UMI_STATUS_OK);
    REQUIRE(umi_change_broker_publish(
                broker,
                UMI_REFLECTION_TOPIC_RESOURCE_REGISTERED,
                "continued",
                2U) == UMI_STATUS_OK);
    REQUIRE(umi_change_broker_last_sequence(broker) == 2U);
    REQUIRE(umi_change_broker_replay_after(broker,
                                           1U,
                                           &replay_count,
                                           &replay_checkpoint) ==
            UMI_STATUS_OK);
    REQUIRE(replay_count == 1U);
    REQUIRE(replay_checkpoint == 2U);

cleanup:
    umi_reflection_engine_destroy(engine);
    umi_change_broker_destroy(broker);
    if (result == 0) {
        if (!require_true(
                umi_directory_stat(journal_path, &journal_info) ==
                    UMI_STATUS_OK,
                __LINE__) ||
            !require_true(journal_info.size > 0U, __LINE__)) {
            result = 1;
        }
    }
    /* The journal served its test purpose and must not accumulate in temp. */
    if (journal_path[0] != '\0') {
        (void)remove(journal_path);
    }
    return result;
}
