/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_schema_registry.c
 *
 * PURPOSE:
 *   Verify message schema registration, compatibility and payload validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSchemaRegistry *registry = NULL;
    UmiSchemaDescriptor schema = {
        sizeof(UmiSchemaDescriptor), "test.event", 2U,
        UMI_MESSAGE_EVENT, 64U, UMI_SCHEMA_BACKWARD
    };
    UmiMessageEnvelope message;
    assert(umi_schema_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_schema_registry_register(registry, &schema) == UMI_STATUS_OK);
    assert(umi_schema_registry_count(registry) == 1U);
    umi_message_envelope_init(&message, UMI_MESSAGE_EVENT, "test", "ok");
    message.schema_id = "test.event";
    message.schema_version = 1U;
    assert(umi_schema_registry_validate(registry, &message) == UMI_STATUS_OK);
    message.kind = UMI_MESSAGE_COMMAND;
    assert(umi_schema_registry_validate(registry, &message) == UMI_STATUS_INVALID_ARGUMENT);
    umi_schema_registry_destroy(registry);
    return 0;
}
