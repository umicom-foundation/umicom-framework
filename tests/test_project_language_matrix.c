/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_project_language_matrix.c
 *
 * PURPOSE:
 *   Implement the test project language matrix behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework polyglot project language matrix regression test.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT. */
#include <assert.h>
#include <string.h>

#include "umicom/project/language_matrix.h"

/*
 * Exercise make entry and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiProjectLanguageEntrySnapshot make_entry(
    const char *language_id, const char *roots, const char *standard,
    uint32_t compile_order, UmiProjectLanguageInteropPolicy interop)
{
    UmiProjectLanguageEntrySnapshot entry;
    umi_project_language_entry_init(&entry);
    strcpy(entry.language_id, language_id);
    strcpy(entry.source_roots, roots);
    strcpy(entry.standard, standard);
    entry.compile_order = compile_order;
    entry.interop_policy = interop;
    return entry;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiProjectLanguageMatrix *matrix = NULL;
    UmiProjectLanguageEntrySnapshot c_entry;
    UmiProjectLanguageEntrySnapshot cpp_entry;
    UmiProjectLanguageEntrySnapshot found;
    UmiProjectLanguageMatrixSnapshot snapshot;
    UmiProjectLanguageValidationReport report;

    assert(umi_project_language_matrix_create("umicom.framework", &matrix) ==
           UMI_STATUS_OK);
    c_entry = make_entry("c", "src;include", "C23", 0U,
                         UMI_PROJECT_LANGUAGE_INTEROP_C_ABI);
    cpp_entry = make_entry("cpp", "adapters/qt", "C++23", 1U,
                           UMI_PROJECT_LANGUAGE_INTEROP_C_ABI);
    assert(umi_project_language_matrix_upsert(matrix, &c_entry) == UMI_STATUS_OK);
    assert(umi_project_language_matrix_upsert(matrix, &cpp_entry) == UMI_STATUS_OK);
    assert(umi_project_language_matrix_set_primary(matrix, "c") == UMI_STATUS_OK);
    assert(umi_project_language_matrix_validate(matrix, &report) == UMI_STATUS_OK);
    assert(report.error_count == 0U);
    assert(report.primary_language_enabled != 0);
    assert(report.compile_order_is_unique != 0);

    assert(umi_project_language_matrix_snapshot(matrix, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.language_count == 2U);
    assert(snapshot.enabled_count == 2U);
    assert(snapshot.c_abi_count == 2U);
    assert(strcmp(snapshot.primary_language_id, "c") == 0);
    assert(umi_project_language_matrix_find(matrix, "cpp", &found) == UMI_STATUS_OK);
    assert(strcmp(found.standard, "C++23") == 0);
    assert(umi_project_language_matrix_remove(matrix, "cpp") == UMI_STATUS_OK);
    assert(umi_project_language_matrix_count(matrix) == 1U);
    assert(strcmp(umi_project_language_interop_policy_text(
                      UMI_PROJECT_LANGUAGE_INTEROP_C_ABI),
                  "c-abi") == 0);
    umi_project_language_matrix_destroy(matrix);
    return 0;
}
