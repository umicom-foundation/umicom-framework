/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/builtin_diagnostics.c
 *
 * PURPOSE:
 *   Enumerate and install normalized diagnostic parsers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/builtin_diagnostics.h"

#include "umicom/developer_productivity/diagnostic_parsers/parsers.h"

typedef const UmiDeveloperDiagnosticParser *(*ParserFactory)(void);

static const ParserFactory FACTORIES[] = {
    umi_developer_diagnostic_parser_gcc,
    umi_developer_diagnostic_parser_clang,
    umi_developer_diagnostic_parser_msvc,
    umi_developer_diagnostic_parser_cmake,
    umi_developer_diagnostic_parser_ninja,
    umi_developer_diagnostic_parser_linker,
    umi_developer_diagnostic_parser_ctest,
    umi_developer_diagnostic_parser_rust,
    umi_developer_diagnostic_parser_zig,
    umi_developer_diagnostic_parser_python,
    umi_developer_diagnostic_parser_java,
    umi_developer_diagnostic_parser_typescript
};

size_t umi_developer_builtin_diagnostic_parser_count(void)
{
    return sizeof(FACTORIES) / sizeof(FACTORIES[0]);
}

const UmiDeveloperDiagnosticParser *
umi_developer_builtin_diagnostic_parser_at(size_t index)
{
    return index < umi_developer_builtin_diagnostic_parser_count()
        ? FACTORIES[index]()
        : NULL;
}

UmiStatus umi_developer_builtin_diagnostics_register(
    UmiDeveloperDiagnosticRegistry *registry)
{
    size_t index;

    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    for (index = 0U;
         index < umi_developer_builtin_diagnostic_parser_count();
         ++index) {
        UmiStatus status = umi_developer_diagnostic_registry_register(
            registry,
            FACTORIES[index]());

        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
