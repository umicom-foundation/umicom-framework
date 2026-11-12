/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/source_contracts/test_setup_composition.c
 *
 * PURPOSE:
 *   Exercise Source Contracts and Setup Centre through public APIs in one native executable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/source_contracts/source_contracts.h"
#include "umicom/setup_centre/setup.h"
#include <stdio.h>
#include <string.h>

#define REQUIRE(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *roots[] = {argv[1]};
    UmiSourceContractReport source_report = {0};
    UmiSetupReport setup_report = {0};
    UmiSetupBundle *bundle = NULL;

    /* The maintained source copy has valid metadata. Its audit exercises real
     * enumeration, bounded reads, token storage and release through the public
     * library, pulling both text.c and files.c into the executable. */
    REQUIRE(UmiSourceContractAudit(roots, 1U, false, &source_report) == 0);
    REQUIRE(source_report.files_checked == 1U);
    REQUIRE(source_report.findings == 0U && source_report.input_errors == 0U);

    /* The deliberately incomplete catalogue is data, never an executable.
     * Opening it exercises Setup Centre's different path, read and text
     * contracts from the same process. No install, copy or launch is requested. */
    REQUIRE(UmiSetupValidateRelative("bin/example.exe") == UMI_STATUS_OK);
    REQUIRE(UmiSetupValidateRelative("../example.exe") != UMI_STATUS_OK);
    REQUIRE(UmiSetupBundleOpen(argv[1], &bundle, &setup_report) == UMI_STATUS_PARSE_ERROR);
    REQUIRE(bundle == NULL);
    REQUIRE(setup_report.status == UMI_STATUS_PARSE_ERROR);
    REQUIRE(setup_report.outputCreated == 0 && setup_report.completed == 0);
    REQUIRE(strstr(setup_report.detail, "catalogue") != NULL);

    /* A second audit checks that refused setup parsing has not disturbed the
     * independently owned source scanner or its allocation lifetime. */
    source_report = (UmiSourceContractReport){0};
    REQUIRE(UmiSourceContractAudit(roots, 1U, false, &source_report) == 0);
    REQUIRE(source_report.files_checked == 1U);
    return 0;
}
