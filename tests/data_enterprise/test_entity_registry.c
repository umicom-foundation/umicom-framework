/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_entity_registry.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the entity registry enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/entity_registry.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataEntityRegistry registry;
    UmiDataEntityDescriptor item;
    UmiDataEntityDescriptor found;
    umi_data_entity_registry_init(&registry);
    CHECK(umi_data_entity_descriptor_init(&item,"Order","orders","id") == UMI_STATUS_OK);
    CHECK(umi_data_entity_registry_add(&registry, &item) == UMI_STATUS_OK);
    CHECK(umi_data_entity_registry_add(&registry, &item) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_data_entity_registry_find(&registry, item.entity_id, &found) == UMI_STATUS_OK);
    CHECK(strcmp(found.entity_id, item.entity_id) == 0);
    CHECK(umi_data_entity_registry_count(&registry) == 1U);
    return 0;
}
