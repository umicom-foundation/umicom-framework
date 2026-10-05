/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/cash_planning/test_document.c
 * PURPOSE: Reject ambiguous cash documents and retain exact money values through transfer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
/* Replace a known spelling in a valid document to exercise one refusal without
 * duplicating the production encoder. The mutation never touches the source. */
static char *Mutate(const char *source, const char *from, const char *to)
{
    const char *at = strstr(source, from);
    CHECK(at != NULL);
    size_t prefix = (size_t)(at - source), length = strlen(source) - strlen(from) + strlen(to);
    char *text = malloc(length + 1U);
    CHECK(text != NULL);
    memcpy(text, source, prefix);
    memcpy(text + prefix, to, strlen(to));
    strcpy(text + prefix + strlen(to), at + strlen(from));
    return text;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiCashPlan *p = Plan(), *loaded = NULL;
    Add(p, "rent", 3, 85000, 0);
    if (strcmp(mode, "maximum") == 0)
        for (size_t i = 1U; i < UMI_CASH_PLAN_CAPACITY; ++i)
        {
            char id[48];
            (void)snprintf(id, sizeof id, "id-%zu", i);
            Add(p, id, 4, 1, 1);
        }
    if (strcmp(mode, "escaping") == 0)
    {
        UmiCashPlanEntry e = Entry("quote", 4, 0, 1);
        strcpy(e.label, "=SUM(1,2) \"caf\xc3\xa9\" \\");
        OK(UmiCashPlanAdd(p, &e));
    }
    size_t size = 0U;
    char *bytes = Encode(p, &size), *bad = NULL;
    if (strcmp(mode, "round-trip") == 0 || strcmp(mode, "maximum") == 0 || strcmp(mode, "escaping") == 0)
    {
        OK(UmiCashPlanDecode(bytes, size, &loaded));
        Same(p, loaded);
    }
    else if (strcmp(mode, "csv") == 0)
    {
        char *csv = NULL;
        size_t n = 0U;
        OK(UmiCashPlanCsv(p, &csv, &n));
        CHECK(n == strlen(csv) && strstr(csv, "\"inflow_minor\"") &&
              strstr(csv, "\"activity\",\"0\",\"85000\",\"15000\",\"-5000\",\"GBP\",\"2\",\"1\""));
        CHECK(strstr(csv, "\"opening\"") && strstr(csv, "\"closing\""));
        UmiCashPlanFreeBytes(csv);
    }
    else if (strcmp(mode, "csv-overflow") == 0)
    {
        UmiCashPlanConfig c = Config();
        c.opening.minor_units = INT64_MIN;
        OK(UmiCashPlanSetConfig(p, &c));
        char *csv = (char *)1;
        size_t n = 99U;
        CHECK(UmiCashPlanCsv(p, &csv, &n) != UMI_STATUS_OK && csv == NULL && n == 0U);
    }
    else
    {
        if (strcmp(mode, "unknown") == 0)
            bad = Mutate(bytes, "\"title\"", "\"unexpected\"");
        else if (strcmp(mode, "duplicate") == 0)
            bad = Mutate(bytes, "\"title\"", "\"format\"");
        else if (strcmp(mode, "missing") == 0)
            bad = Mutate(bytes, "\"scale\":2,", "");
        else if (strcmp(mode, "format") == 0)
            bad = Mutate(bytes, "umicom.cash-plan", "other.cash-plan");
        else if (strcmp(mode, "wrong-kind") == 0)
            bad = Mutate(bytes, "\"openingMinor\":100000", "\"openingMinor\":\"100000\"");
        else if (strcmp(mode, "negative") == 0)
            bad = Mutate(bytes, "85000", "-1");
        else if (strcmp(mode, "invalid-date") == 0)
            bad = Mutate(bytes, "2028-02-03", "2028-02-30");
        else if (strcmp(mode, "duplicate-id") == 0)
        {
            Add(p, "second", 4, 1, 1);
            UmiCashPlanFreeBytes(bytes);
            bytes = Encode(p, &size);
            bad = Mutate(bytes, "\"id\":\"second\"", "\"id\":\"rent\"");
        }
        else if (strcmp(mode, "boolean") == 0)
            bad = Mutate(bytes, "true", "1");
        else if (strcmp(mode, "fractional") == 0)
            bad = Mutate(bytes, "85000", "85000.0");
        else if (strcmp(mode, "overflow") == 0)
            bad = Mutate(bytes, "85000", "9223372036854775808");
        else if (strcmp(mode, "embedded-nul") == 0)
            bad = Mutate(bytes, "Household cash", "Household\\u0000cash");
        else if (strcmp(mode, "trailing") == 0)
            bad = Mutate(bytes, "}]}", "}]} true");
        else if (strcmp(mode, "truncated") == 0)
        {
            bad = malloc(size + 1U);
            CHECK(bad != NULL);
            memcpy(bad, bytes, size + 1U);
            bad[size - 3U] = '\0';
        }
        else
            return 2;
        CHECK(UmiCashPlanDecode(bad, strlen(bad), &loaded) != UMI_STATUS_OK && loaded == NULL);
        UmiCashPlanForecast *r = calloc(1U, sizeof *r);
        CHECK(r != NULL);
        OK(UmiCashPlanProject(p, r));
        CHECK(r->minimum_minor == 15000);
        free(r);
    }
    free(bad);
    UmiCashPlanFreeBytes(bytes);
    UmiCashPlanDestroy(loaded);
    UmiCashPlanDestroy(p);
    return 0;
}
