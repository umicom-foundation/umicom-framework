/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/cash_planning/test_file.c
 * PURPOSE: Retain existing outputs and reject malformed or relative plan files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/platform/input_file.h"
#undef CHECK
#include "../build_log/fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(path, root, "cash-caf\xc3\xa9.json");
    UmiCashPlan *p = Plan(), *loaded = NULL;
    Add(p, "rent", 3, 85000, 0);
    UmiOutputFileSnapshot receipt;
    if (strcmp(mode, "relative") == 0)
    {
        CHECK(UmiCashPlanSaveNew("relative.json", p, &receipt) != UMI_STATUS_OK && receipt.path[0] == '\0');
    }
    else if (strcmp(mode, "missing") == 0)
    {
        CHECK(UmiCashPlanLoad(path, &loaded) != UMI_STATUS_OK && loaded == NULL);
    }
    else if (strcmp(mode, "malformed") == 0)
    {
        UmiOutputFile *file = NULL;
        OK(UmiOutputFileCreate(path, &file));
        OK(UmiOutputFileWrite(file, "{bad", 4U));
        OK(UmiOutputFileClose(file));
        UmiOutputFileDestroy(file);
        CHECK(UmiCashPlanLoad(path, &loaded) != UMI_STATUS_OK && loaded == NULL);
    }
    else if (strcmp(mode, "round-trip") == 0 || strcmp(mode, "no-overwrite") == 0)
    {
        OK(UmiCashPlanSaveNew(path, p, &receipt));
        CHECK(receipt.closed && receipt.bytes_written > 0U);
        if (strcmp(mode, "no-overwrite") == 0)
        {
            Add(p, "later", 5, 500, 1);
            CHECK(UmiCashPlanSaveNew(path, p, &receipt) != UMI_STATUS_OK);
            OK(UmiCashPlanRemove(p, 1U));
        }
        OK(UmiCashPlanLoad(path, &loaded));
        Same(p, loaded);
    }
    else if (strcmp(mode, "export") == 0)
    {
        OK(UmiCashPlanExportNew(path, p, &receipt));
        unsigned char *bytes = NULL;
        size_t n = 0U;
        OK(UmiInputFileRead(path, UMI_CASH_PLAN_DOCUMENT_LIMIT, &bytes, &n));
        char *expected = NULL;
        size_t en = 0U;
        OK(UmiCashPlanCsv(p, &expected, &en));
        CHECK(n == en && memcmp(bytes, expected, n) == 0);
        UmiInputFileFree(bytes);
        UmiCashPlanFreeBytes(expected);
    }
    else
        return 2;
    UmiCashPlanDestroy(loaded);
    UmiCashPlanDestroy(p);
    return 0;
}
