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

/*
 * Return the number of records represented by developer builtin diagnostic parser without
 * changing their state.
 */
size_t umi_developer_builtin_diagnostic_parser_count(void)
{
    return sizeof(FACTORIES) / sizeof(FACTORIES[0]);
}

/*
 * Find developer builtin diagnostic parser while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperDiagnosticParser *
umi_developer_builtin_diagnostic_parser_at(size_t index)
{
    return index < umi_developer_builtin_diagnostic_parser_count()
        ? FACTORIES[index]()
        : NULL;
}

/*
 * Add developer builtin diagnostics only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_developer_builtin_diagnostics_register(
    UmiDeveloperDiagnosticRegistry *registry)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_developer_builtin_diagnostic_parser_count();
         ++index) {
        UmiStatus status = umi_developer_diagnostic_registry_register(
            registry,
            FACTORIES[index]());

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
