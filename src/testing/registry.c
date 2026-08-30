/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/registry.c
 *
 * PURPOSE:
 *   Implement thread-safe test-suite registration without taking ownership of suites.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/registry.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"

struct UmiTestRegistry {
    UmiTestSuite *suites[UMI_TEST_REGISTRY_MAX_SUITES];
    size_t count;
    UmiMutex *mutex;
};

UmiStatus umi_test_registry_create(UmiTestRegistry **out_registry)
{
    UmiTestRegistry *registry;
    if (out_registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_registry = NULL;
    registry = (UmiTestRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL ||
        umi_mutex_create(&registry->mutex) != UMI_STATUS_OK) {
        if (registry != NULL) {
            free(registry);
        }
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_test_registry_destroy(UmiTestRegistry *registry)
{
    if (registry != NULL) {
        umi_mutex_destroy(registry->mutex);
        free(registry);
    }
}

UmiStatus umi_test_registry_add(UmiTestRegistry *registry,
                                UmiTestSuite *suite)
{
    if (registry == NULL || suite == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(registry->mutex);
    {
        size_t index;
        for (index = 0U; index < registry->count; ++index) {
            if (strcmp(umi_test_suite_id(registry->suites[index]),
                       umi_test_suite_id(suite)) == 0) {
                (void)umi_mutex_unlock(registry->mutex);
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }
    if (registry->count >= UMI_TEST_REGISTRY_MAX_SUITES) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->suites[registry->count++] = suite;
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_test_registry_remove(UmiTestRegistry *registry,
                                   const char *suite_id)
{
    size_t index;
    if (registry == NULL || suite_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(registry->mutex);
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(umi_test_suite_id(registry->suites[index]),
                   suite_id) == 0) {
            size_t remaining = registry->count - index - 1U;
            if (remaining > 0U) {
                (void)memmove(&registry->suites[index],
                              &registry->suites[index + 1U],
                              remaining * sizeof(registry->suites[0]));
            }
            registry->count -= 1U;
            (void)umi_mutex_unlock(registry->mutex);
            return UMI_STATUS_OK;
        }
    }
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_NOT_FOUND;
}

size_t umi_test_registry_count(const UmiTestRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

UmiTestSuite *umi_test_registry_at(UmiTestRegistry *registry,
                                   size_t index)
{
    return registry != NULL && index < registry->count
        ? registry->suites[index]
        : NULL;
}

UmiTestSuite *umi_test_registry_find(UmiTestRegistry *registry,
                                     const char *suite_id)
{
    size_t index;
    if (registry == NULL || suite_id == NULL) {
        return NULL;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(umi_test_suite_id(registry->suites[index]),
                   suite_id) == 0) {
            return registry->suites[index];
        }
    }
    return NULL;
}
