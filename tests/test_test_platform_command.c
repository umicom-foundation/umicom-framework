/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_test_platform_command.c
 *
 * PURPOSE:
 *   Implement the test test platform command behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework professional Test Explorer command catalogue tests.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/test_platform/command.h"

int main(void)
{
    size_t index;
    assert(umi_test_platform_command_count() == 40U);
    for (index = 0U; index < umi_test_platform_command_count(); ++index) {
        const UmiTestPlatformCommandDescriptor *command =
            umi_test_platform_command_at(index);
        size_t comparison;
        assert(command != NULL);
        assert(command->struct_size == (uint32_t)sizeof(*command));
        assert(command->api_version == UMI_TEST_PLATFORM_COMMAND_API_VERSION);
        assert(command->id[0] != '\0' && command->label[0] != '\0');
        assert(umi_test_platform_command_find(command->id) == command);
        assert(umi_test_platform_command_for_kind(command->kind) == command);
        for (comparison = index + 1U;
             comparison < umi_test_platform_command_count(); ++comparison) {
            assert(strcmp(command->id,
                          umi_test_platform_command_at(comparison)->id) != 0);
        }
    }
    assert(umi_test_platform_command_find("test.coverage-all") != NULL);
    assert(umi_test_platform_command_find("test.missing") == NULL);
    return 0;
}
