/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_task.c
 *
 * PURPOSE:
 *   Verify declarative tasks, bounded metadata and registry snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/build/task.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiBuildTaskRegistry *registry = NULL;
    UmiBuildTaskSnapshot configure;
    UmiBuildTaskSnapshot build;
    UmiBuildTaskSnapshot composite;
    UmiBuildTaskSnapshot found;
    UmiBuildTaskRegistrySnapshot snapshot;
    char message[256];

    assert(umi_build_task_registry_create(&registry) == UMI_STATUS_OK);
    umi_build_task_init(&configure, "configure", "Configure",
                        UMI_BUILD_TASK_CONFIGURE);
    assert(umi_build_task_set_command(
               &configure, "cmake --preset debug", ".") == UMI_STATUS_OK);
    assert(umi_build_task_add_input(&configure, "CMakeLists.txt") ==
           UMI_STATUS_OK);
    assert(umi_build_task_add_output(&configure, "build/CMakeCache.txt") ==
           UMI_STATUS_OK);
    assert(umi_build_task_add_environment(&configure, "UMICOM_PROFILE=debug") ==
           UMI_STATUS_OK);
    configure.flags |= UMI_BUILD_TASK_DEFAULT;

    umi_build_task_init(&build, "build", "Build", UMI_BUILD_TASK_BUILD);
    assert(umi_build_task_set_command(
               &build, "cmake --build build", ".") == UMI_STATUS_OK);
    assert(umi_build_task_add_dependency(&build, "configure") == UMI_STATUS_OK);
    build.maximum_attempts = 2U;

    umi_build_task_init(&composite, "verify", "Verify",
                        UMI_BUILD_TASK_COMPOSITE);
    assert(umi_build_task_add_dependency(&composite, "build") == UMI_STATUS_OK);

    assert(umi_build_task_validate(&configure, message, sizeof(message)) ==
           UMI_STATUS_OK);
    assert(umi_build_task_registry_upsert(registry, &configure) == UMI_STATUS_OK);
    assert(umi_build_task_registry_upsert(registry, &build) == UMI_STATUS_OK);
    assert(umi_build_task_registry_upsert(registry, &composite) == UMI_STATUS_OK);
    assert(umi_build_task_registry_find(registry, "build", &found) ==
           UMI_STATUS_OK);
    assert(found.dependency_count == 1U);
    assert(strcmp(found.dependencies[0], "configure") == 0);
    assert(umi_build_task_registry_snapshot(registry, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.task_count == 3U);
    assert(snapshot.default_count == 1U);
    assert(snapshot.composite_count == 1U);

    assert(umi_build_task_registry_remove(registry, "verify") == UMI_STATUS_OK);
    assert(umi_build_task_registry_count(registry) == 2U);
    umi_build_task_registry_destroy(registry);
    return 0;
}
