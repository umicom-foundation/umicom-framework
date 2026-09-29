/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Public-header consumer and behavioural regression cases. */
#include "umicom/distribution/runtime/inventory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(expr) do { if (!(expr)) { (void)fprintf(stderr, "Failed at line %d: %s\n", __LINE__, #expr); return 1; } } while (0)
#define MAGIC "UMICOM-RELEASE-INVENTORY\t1\n"
#define CONTEXT "\t0123456789abcdef0123456789abcdef\t2f737263\t2f6275696c64\t4465627567\n"
#define CMAKE MAGIC "context\tcmake" CONTEXT
#define CTEST MAGIC "context\tctest" CONTEXT
#define ALPHA "test\t616c706861\t\tregistered\t\n"
#define BETA "test\t62657461\t\tregistered\t\n"
static UmiStatus Parse(const char *text, UmiReleaseInventory **out)
{ return UmiReleaseInventoryParse(text, strlen(text), out); }

static int Valid(void)
{
    const char input[] = CMAKE
        "header\t696e636c7564652f612e68\t62617365\tdeclared\t7368613235363d31\n"
        "target\t62617365\t2f737263\tconfigured\t\n" ALPHA;
    UmiReleaseInventory *value = NULL;
    REQUIRE(Parse(input, &value) == UMI_STATUS_OK);
    UmiReleaseInventorySummary summary = {0};
    REQUIRE(UmiReleaseInventorySummarise(value, &summary) == UMI_STATUS_OK);
    REQUIRE(summary.headers == 1U && summary.targets == 1U && summary.tests == 1U);
    REQUIRE(strcmp(UmiReleaseInventoryAt(value, 0U)->identity, "include/a.h") == 0);
    REQUIRE(strcmp(UmiReleaseInventoryAt(value, 0U)->detail, "sha256=1") == 0);
    REQUIRE(strcmp(UmiReleaseInventorySourceRoot(value), "/src") == 0);
    REQUIRE(strcmp(UmiReleaseInventoryBuildRoot(value), "/build") == 0);
    REQUIRE(strcmp(UmiReleaseInventoryConfiguration(value), "Debug") == 0);
    REQUIRE(UmiReleaseInventoryAt(value, 3U) == NULL);
    UmiReleaseInventoryDestroy(value);
    /* CRLF, no final newline, tabs, Unicode bytes and semicolons stay data. */
    value = NULL;
    REQUIRE(Parse(CMAKE "test\tceb43b095b5d\t\tregistered\t", &value) == UMI_STATUS_OK);
    REQUIRE(strcmp(UmiReleaseInventoryAt(value, 0U)->identity, "\xce\xb4;\t[]") == 0);
    UmiReleaseInventoryDestroy(value); value = NULL;
    REQUIRE(Parse("UMICOM-RELEASE-INVENTORY\t1\r\ncontext\tcmake"
        "\t0123456789abcdef0123456789abcdef\t2f737263\t2f6275696c64\t4465627567\r\n"
        "test\t61\t\tregistered\t\r\n", &value) == UMI_STATUS_OK);
    UmiReleaseInventoryDestroy(value);
    return 0;
}

static int Malformed(void)
{
    const char *const cases[] = {
        "", MAGIC, CMAKE "\n", CMAKE "test\t6\t\tregistered\t\n",
        CMAKE "test\tgg\t\tregistered\t\n", CMAKE "test\t00\t\tregistered\t\n",
        CMAKE "test\t\t\tregistered\t\n", CMAKE "other\t61\t\tregistered\t\n",
        CMAKE "test\t61\t\tunknown\t\n", CMAKE "test\t61\t\tregistered\t\textra\n",
        CMAKE "header\t61\t\tdeclared\t\n", CMAKE "header\t61\t62\tunassigned\t\n",
        CMAKE "target\t61\t\tconfigured\t\n", CMAKE "test\t61\t\tdisabled\t\n",
        CTEST "header\t61\t\tunassigned\t\n",
        MAGIC "context\tcmake\tbad\t2f\t2f\t61\n",
        MAGIC "context\tcmake\t0123456789abcdef0123456789abcdef\t\t2f\t61\n"
    };
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        UmiReleaseInventory *value = NULL;
        REQUIRE(Parse(cases[i], &value) == UMI_STATUS_PARSE_ERROR);
        REQUIRE(value == NULL);
    }
    UmiReleaseInventory *value = NULL;
    const char embedded[] = CMAKE ALPHA "\0";
    REQUIRE(UmiReleaseInventoryParse(embedded, sizeof(embedded) - 1U, &value) == UMI_STATUS_PARSE_ERROR);
    REQUIRE(value == NULL);
    return 0;
}

static int Duplicate(void)
{
    const char *const cases[] = {
        CMAKE ALPHA ALPHA,
        CMAKE "header\t61\t\tunassigned\t\nheader\t61\t62\tdeclared\t\n",
        CMAKE "target\t61\t62\tconfigured\t\ntarget\t61\t63\tconfigured\t\n",
        CMAKE "source\t61\t62\tpresent\t\nsource\t61\t62\tunresolved\t\n"
    };
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        UmiReleaseInventory *value = NULL;
        REQUIRE(Parse(cases[i], &value) == UMI_STATUS_ALREADY_EXISTS && value == NULL);
    }
    UmiReleaseInventory *value = NULL;
    REQUIRE(Parse(CMAKE "source\t61\t62\tpresent\t\nsource\t61\t63\tpresent\t\n", &value) == UMI_STATUS_OK);
    REQUIRE(UmiReleaseInventoryCount(value) == 2U);
    UmiReleaseInventoryDestroy(value);
    return 0;
}

typedef struct Differences { size_t missing, added; bool wrong; } Differences;
static void Difference(void *context, UmiReleaseInventoryDifference kind, const char *name)
{
    Differences *counts = context;
    if (kind == UMI_RELEASE_INVENTORY_TEST_MISSING) {
        ++counts->missing; counts->wrong |= strcmp(name, "alpha") != 0;
    } else { ++counts->added; counts->wrong |= strcmp(name, "beta") != 0; }
}
static int Names(void)
{
    UmiReleaseInventory *left = NULL, *right = NULL;
    REQUIRE(Parse(CMAKE ALPHA, &left) == UMI_STATUS_OK);
    REQUIRE(Parse(CTEST BETA, &right) == UMI_STATUS_OK);
    UmiReleaseInventoryComparison result = {0}; Differences differences = {0};
    REQUIRE(UmiReleaseInventoryCompareTests(left, right, Difference, &differences, &result) == UMI_STATUS_OK);
    REQUIRE(result.expectedTests == result.observedTests && !result.namesMatch);
    REQUIRE(result.missingTests == 1U && result.addedTests == 1U);
    REQUIRE(differences.missing == 1U && differences.added == 1U && !differences.wrong);
    UmiReleaseInventoryDestroy(left); UmiReleaseInventoryDestroy(right);
    left = NULL; right = NULL;
    REQUIRE(Parse(CMAKE BETA ALPHA, &left) == UMI_STATUS_OK);
    REQUIRE(Parse(CTEST ALPHA BETA, &right) == UMI_STATUS_OK);
    REQUIRE(UmiReleaseInventoryCompareTests(left, right, NULL, NULL, &result) == UMI_STATUS_OK);
    REQUIRE(result.namesMatch && result.expectedTests == 2U);
    UmiReleaseInventoryDestroy(left); UmiReleaseInventoryDestroy(right);
    return 0;
}

static int Context(void)
{
    const char *const markers[] = { "012345", "2f737263", "2f6275696c64", "4465627567" };
    UmiReleaseInventory *left = NULL;
    REQUIRE(Parse(CMAKE ALPHA, &left) == UMI_STATUS_OK);
    for (size_t i = 0U; i < sizeof(markers) / sizeof(markers[0]); ++i) {
        char changed[] = CTEST ALPHA;
        char *place = strstr(changed, markers[i]); REQUIRE(place != NULL);
        *place = '3';
        UmiReleaseInventory *right = NULL;
        REQUIRE(Parse(changed, &right) == UMI_STATUS_OK);
        UmiReleaseInventoryComparison result = { .expectedTests = 777U };
        REQUIRE(UmiReleaseInventoryCompareTests(left, right, NULL, NULL, &result) == UMI_STATUS_INVALID_STATE);
        REQUIRE(result.expectedTests == 777U);
        UmiReleaseInventoryDestroy(right);
    }
    UmiReleaseInventoryComparison result = { .expectedTests = 777U };
    REQUIRE(UmiReleaseInventoryCompareTests(left, left, NULL, NULL, &result) == UMI_STATUS_INVALID_STATE);
    REQUIRE(result.expectedTests == 777U);
    UmiReleaseInventoryDestroy(left);
    return 0;
}

static int States(void)
{
    UmiReleaseInventory *left = NULL, *right = NULL;
    REQUIRE(Parse(CMAKE "header\t61\t\tunassigned\t\nsource\t61\t62\tunresolved\t\n" ALPHA BETA, &left) == UMI_STATUS_OK);
    REQUIRE(Parse(CTEST "test\t616c706861\t\tdisabled-missing-command\t\n" BETA, &right) == UMI_STATUS_OK);
    UmiReleaseInventorySummary summary = {0};
    REQUIRE(UmiReleaseInventorySummarise(left, &summary) == UMI_STATUS_OK);
    REQUIRE(summary.unassignedHeaders == 1U && summary.unresolvedSources == 1U);
    UmiReleaseInventoryComparison result = {0};
    REQUIRE(UmiReleaseInventoryCompareTests(left, right, NULL, NULL, &result) == UMI_STATUS_OK);
    REQUIRE(result.namesMatch && result.disabledTests == 1U && result.missingCommands == 1U);
    UmiReleaseInventoryDestroy(right); right = NULL;
    REQUIRE(Parse(CTEST, &right) == UMI_STATUS_OK);
    result.expectedTests = 999U;
    REQUIRE(UmiReleaseInventoryCompareTests(left, right, NULL, NULL, &result) == UMI_STATUS_UNAVAILABLE);
    REQUIRE(result.expectedTests == 999U);
    UmiReleaseInventoryDestroy(left); UmiReleaseInventoryDestroy(right);
    return 0;
}

static int Large(void)
{
    const size_t capacity = 1024U * 1024U;
    char *text = malloc(capacity); REQUIRE(text != NULL);
    size_t used = strlen(CMAKE); memcpy(text, CMAKE, used + 1U);
    for (size_t i = 0U; i < 4097U; ++i) {
        /* Four hex-encoded non-NUL decimal bytes give 4,097 distinct names. */
        size_t digits[] = { i / 1000U, (i / 100U) % 10U, (i / 10U) % 10U, i % 10U };
        int written = snprintf(text + used, capacity - used, "test\t%02x%02x%02x%02x\t\tregistered\t\n",
            (unsigned)(48U + digits[0]), (unsigned)(48U + digits[1]),
            (unsigned)(48U + digits[2]), (unsigned)(48U + digits[3]));
        REQUIRE(written > 0 && (size_t)written < capacity - used); used += (size_t)written;
    }
    UmiReleaseInventory *left = NULL, *right = NULL;
    REQUIRE(UmiReleaseInventoryParse(text, used, &left) == UMI_STATUS_OK);
    memcpy(text + strlen(MAGIC "context\t"), "ctest", 5U);
    REQUIRE(UmiReleaseInventoryParse(text, used, &right) == UMI_STATUS_OK);
    free(text); /* Parsed objects own their bytes. */
    UmiReleaseInventoryComparison result = {0};
    REQUIRE(UmiReleaseInventoryCompareTests(left, right, NULL, NULL, &result) == UMI_STATUS_OK);
    REQUIRE(result.namesMatch && result.expectedTests == 4097U);
    REQUIRE(strcmp(UmiReleaseInventoryAt(left, 4096U)->identity, "4096") == 0);
    UmiReleaseInventoryDestroy(left); UmiReleaseInventoryDestroy(right);
    return 0;
}

static int Bounds(void)
{
    UmiReleaseInventory *value = NULL;
    REQUIRE(UmiReleaseInventoryParse("", UMI_RELEASE_INVENTORY_TEXT_LIMIT + 1U, &value) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(Parse(CMAKE ALPHA, &value) == UMI_STATUS_OK);
    UmiReleaseInventory *saved = value;
    REQUIRE(Parse(CMAKE BETA, &value) == UMI_STATUS_INVALID_ARGUMENT && value == saved);
    REQUIRE(UmiReleaseInventoryParse(NULL, 0U, &value) == UMI_STATUS_INVALID_ARGUMENT);
    UmiReleaseInventorySummary summary = { .tests = 123U };
    REQUIRE(UmiReleaseInventorySummarise(NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT && summary.tests == 123U);
    UmiReleaseInventoryDestroy(value); value = NULL;
    size_t encoded = (UMI_RELEASE_INVENTORY_FIELD_LIMIT + 1U) * 2U;
    size_t prefix = strlen(CMAKE "test\t");
    char *oversized = malloc(prefix + encoded + 32U); REQUIRE(oversized != NULL);
    memcpy(oversized, CMAKE "test\t", prefix);
    memset(oversized + prefix, '6', encoded);
    strcpy(oversized + prefix + encoded, "\t\tregistered\t\n");
    REQUIRE(Parse(oversized, &value) == UMI_STATUS_CAPACITY_EXCEEDED && value == NULL);
    free(oversized);
    size_t line = strlen(ALPHA), count = UMI_RELEASE_INVENTORY_RECORD_LIMIT + 1U;
    size_t total = strlen(CMAKE) + count * line;
    char *rows = malloc(total + 1U); REQUIRE(rows != NULL);
    memcpy(rows, CMAKE, strlen(CMAKE));
    for (size_t i = 0U; i < count; ++i) memcpy(rows + strlen(CMAKE) + i * line, ALPHA, line);
    rows[total] = '\0';
    REQUIRE(Parse(rows, &value) == UMI_STATUS_CAPACITY_EXCEEDED && value == NULL);
    free(rows);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (strcmp(argv[1], "valid") == 0) return Valid();
    if (strcmp(argv[1], "malformed") == 0) return Malformed();
    if (strcmp(argv[1], "duplicate") == 0) return Duplicate();
    if (strcmp(argv[1], "names") == 0) return Names();
    if (strcmp(argv[1], "context") == 0) return Context();
    if (strcmp(argv[1], "states") == 0) return States();
    if (strcmp(argv[1], "large") == 0) return Large();
    if (strcmp(argv[1], "bounds") == 0) return Bounds();
    return 2;
}
