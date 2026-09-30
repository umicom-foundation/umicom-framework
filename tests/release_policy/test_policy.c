/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_policy/test_policy.c
 * PURPOSE:
 *   Test explicit policy decisions, immutable editing and refusal paths.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: tests/release_policy/test_policy.c
 * Purpose: Test explicit policy decisions, immutable editing and refusal paths.
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/test_policy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { (void)fprintf(stderr, "line %d: %s\n", __LINE__, #expr); return 1; } } while (0)
#define CTX "\t0123456789abcdef0123456789abcdef\t2f737263\t2f6275696c64\t4465627567\n"
#define CONFIG "UMICOM-RELEASE-INVENTORY\t1\ncontext\tcmake" CTX
#define OBSERVED "UMICOM-RELEASE-INVENTORY\t1\ncontext\tctest" CTX
#define POLICY "UMICOM-RELEASE-TEST-POLICY\t1\ncontext\tpolicy" CTX
#define A "test\t61\t\tregistered\t\n"
#define B "test\t62\t\tregistered\t\n"
#define C "test\t63\t\tregistered\t\n"
#define REQUIRED_A "test\t61\t6f776e6572\trequired\t726561736f6e\n"
#define OPTIONAL_B "test\t62\t6f776e6572\toptional\t726561736f6e\n"
#define UNASSIGNED_A "test\t61\t\tunassigned\t\n"
static UmiStatus Inventory(const char *text, UmiReleaseInventory **out)
{ return UmiReleaseInventoryParse(text, strlen(text), out); }
static UmiStatus Policy(const char *text, UmiReleaseTestPolicy **out)
{ return UmiReleaseTestPolicyParse(text, strlen(text), out); }

static int Roundtrip(void)
{
    UmiReleaseInventory *configured = NULL;
    CHECK(Inventory(CONFIG B A, &configured) == UMI_STATUS_OK);
    char *text = NULL; size_t size = 999U;
    CHECK(UmiReleaseTestPolicyDraft(configured, &text, &size) == UMI_STATUS_OK && size == strlen(text));
    UmiReleaseTestPolicy *original = NULL;
    CHECK(UmiReleaseTestPolicyParse(text, size, &original) == UMI_STATUS_OK);
    UmiReleaseTestPolicyTextDestroy(text); text = NULL;
    CHECK(UmiReleaseTestPolicyCount(original) == 2U && UmiReleaseTestPolicyAt(original, 2U) == NULL);
    CHECK(strcmp(UmiReleaseTestPolicyAt(original, 0U)->name, "a") == 0);
    CHECK(UmiReleaseTestPolicyAt(original, 0U)->requirement == UMI_RELEASE_TEST_UNASSIGNED);
    CHECK(UmiReleaseTestPolicyEdit(original, "a", UMI_RELEASE_TEST_REQUIRED,
        "Owner \xce\xb4", "Reason\twith; separators\nretained", &text, &size) == UMI_STATUS_OK);
    UmiReleaseTestPolicy *edited = NULL;
    CHECK(UmiReleaseTestPolicyParse(text, size, &edited) == UMI_STATUS_OK);
    CHECK(UmiReleaseTestPolicyAt(original, 0U)->requirement == UMI_RELEASE_TEST_UNASSIGNED);
    CHECK(UmiReleaseTestPolicyAt(edited, 0U)->requirement == UMI_RELEASE_TEST_REQUIRED);
    CHECK(strcmp(UmiReleaseTestPolicyAt(edited, 0U)->owner, "Owner \xce\xb4") == 0);
    CHECK(strcmp(UmiReleaseTestPolicyAt(edited, 0U)->reason, "Reason\twith; separators\nretained") == 0);
    CHECK(UmiReleaseTestPolicyAt(edited, 1U)->requirement == UMI_RELEASE_TEST_UNASSIGNED);
    UmiReleaseTestPolicyTextDestroy(text); UmiReleaseTestPolicyDestroy(edited);
    UmiReleaseTestPolicyDestroy(original); UmiReleaseInventoryDestroy(configured);
    return 0;
}

static int ParseErrors(void)
{
    const char *const cases[] = {
        "", "UMICOM-RELEASE-TEST-POLICY\t2\n", POLICY "\n",
        POLICY "test\t61\t\trequired\t726561736f6e\n",
        POLICY "test\t61\t6f776e6572\toptional\t\n",
        POLICY "test\t61\t20\toptional\t09\n",
        POLICY "test\t61\t\tauto\t\n", POLICY "test\t6\t\tunassigned\t\n",
        POLICY "test\t00\t\tunassigned\t\n", POLICY "test\t\t\tunassigned\t\n",
        POLICY "header\t61\t\tunassigned\t\n", POLICY "test\t61\t\tunassigned\t\textra\n",
        CONFIG A,
        "UMICOM-RELEASE-TEST-POLICY\t1\ncontext\tpolicy\tbad\t2f\t2f\t61\n"
    };
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        UmiReleaseTestPolicy *policy = NULL;
        CHECK(Policy(cases[i], &policy) == UMI_STATUS_PARSE_ERROR && policy == NULL);
    }
    UmiReleaseTestPolicy *policy = NULL;
    CHECK(Policy(POLICY REQUIRED_A UNASSIGNED_A, &policy) == UMI_STATUS_ALREADY_EXISTS && policy == NULL);
    const char nul[] = POLICY "\0";
    CHECK(UmiReleaseTestPolicyParse(nul, sizeof(nul) - 1U, &policy) == UMI_STATUS_PARSE_ERROR);
    const char crlf[] = "UMICOM-RELEASE-TEST-POLICY\t1\r\ncontext\tpolicy"
        "\t0123456789abcdef0123456789abcdef\t2f\t2f\t61\r\ntest\t61\t\tunassigned\t";
    CHECK(Policy(crlf, &policy) == UMI_STATUS_OK);
    UmiReleaseTestPolicyDestroy(policy);
    return 0;
}

typedef struct Findings { size_t counts[7]; bool badName; } Findings;
static void Finding(void *context, UmiReleaseTestPolicyFinding kind, const char *name)
{
    Findings *findings = context;
    ++findings->counts[kind];
    findings->badName |= name == NULL || name[0] == '\0';
}
static int Decisions(void)
{
    UmiReleaseInventory *configured = NULL, *observed = NULL;
    UmiReleaseTestPolicy *policy = NULL;
    CHECK(Inventory(CONFIG A B, &configured) == UMI_STATUS_OK);
    CHECK(Inventory(OBSERVED A "test\t62\t\tdisabled-missing-command\t\n", &observed) == UMI_STATUS_OK);
    CHECK(Policy(POLICY REQUIRED_A OPTIONAL_B, &policy) == UMI_STATUS_OK);
    UmiReleaseTestPolicyAssessment result = {0}; Findings findings = {0};
    CHECK(UmiReleaseTestPolicyAssess(configured, observed, policy, Finding, &findings, &result) == UMI_STATUS_OK);
    CHECK(result.readyForExecution && result.requiredTests == 1U && result.optionalTests == 1U);
    CHECK(result.optionalUnavailable == 1U && findings.counts[UMI_RELEASE_POLICY_OPTIONAL_UNAVAILABLE] == 1U && !findings.badName);
    UmiReleaseInventoryDestroy(observed); observed = NULL;
    CHECK(Inventory(OBSERVED "test\t61\t\tdisabled\t\n" B, &observed) == UMI_STATUS_OK);
    CHECK(UmiReleaseTestPolicyAssess(configured, observed, policy, NULL, NULL, &result) == UMI_STATUS_OK);
    CHECK(!result.readyForExecution && result.requiredUnavailable == 1U);
    UmiReleaseTestPolicyDestroy(policy); policy = NULL;
    CHECK(Policy(POLICY "test\t61\t6f\toptional\t72\n" OPTIONAL_B, &policy) == UMI_STATUS_OK);
    CHECK(UmiReleaseTestPolicyAssess(configured, observed, policy, NULL, NULL, &result) == UMI_STATUS_OK);
    CHECK(!result.readyForExecution && result.requiredTests == 0U);
    UmiReleaseTestPolicyDestroy(policy); UmiReleaseInventoryDestroy(observed); UmiReleaseInventoryDestroy(configured);
    return 0;
}

static int Drift(void)
{
    UmiReleaseInventory *configured = NULL, *observed = NULL;
    UmiReleaseTestPolicy *policy = NULL;
    CHECK(Inventory(CONFIG A B, &configured) == UMI_STATUS_OK);
    CHECK(Inventory(OBSERVED A C, &observed) == UMI_STATUS_OK);
    CHECK(Policy(POLICY REQUIRED_A OPTIONAL_B, &policy) == UMI_STATUS_OK);
    UmiReleaseTestPolicyAssessment result = {0};
    CHECK(UmiReleaseTestPolicyAssess(configured, observed, policy, NULL, NULL, &result) == UMI_STATUS_OK);
    CHECK(!result.readyForExecution && result.missingRegistrations == 1U && result.addedRegistrations == 1U && result.optionalUnavailable == 1U);
    UmiReleaseTestPolicyDestroy(policy); policy = NULL;
    CHECK(Policy(POLICY UNASSIGNED_A "test\t63\t6f\toptional\t72\n", &policy) == UMI_STATUS_OK);
    CHECK(UmiReleaseTestPolicyAssess(configured, observed, policy, NULL, NULL, &result) == UMI_STATUS_OK);
    CHECK(result.missingRules == 1U && result.orphanRules == 1U && result.unassignedTests == 1U && !result.readyForExecution);
    UmiReleaseTestPolicyDestroy(policy); UmiReleaseInventoryDestroy(observed); UmiReleaseInventoryDestroy(configured);
    return 0;
}

static int ContextErrors(void)
{
    UmiReleaseInventory *configured = NULL, *observed = NULL;
    CHECK(Inventory(CONFIG A, &configured) == UMI_STATUS_OK);
    CHECK(Inventory(OBSERVED A, &observed) == UMI_STATUS_OK);
    const char *const markers[] = { "012345", "2f737263", "2f6275696c64", "4465627567" };
    for (size_t i = 0U; i < sizeof(markers) / sizeof(markers[0]); ++i) {
        char changed[] = POLICY REQUIRED_A;
        char *place = strstr(changed, markers[i]); CHECK(place != NULL); *place = '3';
        UmiReleaseTestPolicy *policy = NULL;
        CHECK(Policy(changed, &policy) == UMI_STATUS_OK);
        UmiReleaseTestPolicyAssessment result = { .expectedTests = 123U }; Findings findings = {0};
        CHECK(UmiReleaseTestPolicyAssess(configured, observed, policy, Finding, &findings, &result) == UMI_STATUS_INVALID_STATE);
        CHECK(result.expectedTests == 123U);
        for (size_t j = 0U; j < 7U; ++j) CHECK(findings.counts[j] == 0U);
        UmiReleaseTestPolicyDestroy(policy);
    }
    UmiReleaseTestPolicy *policy = NULL;
    CHECK(Policy(POLICY REQUIRED_A, &policy) == UMI_STATUS_OK);
    UmiReleaseTestPolicyAssessment result = { .expectedTests = 123U };
    CHECK(UmiReleaseTestPolicyAssess(observed, configured, policy, NULL, NULL, &result) == UMI_STATUS_INVALID_STATE);
    UmiReleaseInventoryDestroy(observed); observed = NULL;
    CHECK(Inventory(OBSERVED, &observed) == UMI_STATUS_OK);
    CHECK(UmiReleaseTestPolicyAssess(configured, observed, policy, NULL, NULL, &result) == UMI_STATUS_UNAVAILABLE);
    CHECK(result.expectedTests == 123U);
    UmiReleaseTestPolicyDestroy(policy); UmiReleaseInventoryDestroy(observed); UmiReleaseInventoryDestroy(configured);
    return 0;
}

static int EditErrors(void)
{
    UmiReleaseTestPolicy *policy = NULL; CHECK(Policy(POLICY REQUIRED_A, &policy) == UMI_STATUS_OK);
    char *text = NULL; size_t size = 321U;
    CHECK(UmiReleaseTestPolicyEdit(policy, "a*", UMI_RELEASE_TEST_OPTIONAL, "o", "r", &text, &size) == UMI_STATUS_NOT_FOUND);
    CHECK(text == NULL && size == 321U);
    CHECK(UmiReleaseTestPolicyEdit(policy, "a", UMI_RELEASE_TEST_REQUIRED, " ", "r", &text, &size) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiReleaseTestPolicyEdit(policy, "a", (UmiReleaseTestRequirement)99, "o", "r", &text, &size) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiReleaseTestPolicyEdit(policy, "a", UMI_RELEASE_TEST_UNASSIGNED, "", "", &text, &size) == UMI_STATUS_OK);
    UmiReleaseTestPolicy *reset = NULL; CHECK(UmiReleaseTestPolicyParse(text, size, &reset) == UMI_STATUS_OK);
    CHECK(UmiReleaseTestPolicyAt(reset, 0U)->requirement == UMI_RELEASE_TEST_UNASSIGNED);
    CHECK(UmiReleaseTestPolicyAt(policy, 0U)->requirement == UMI_RELEASE_TEST_REQUIRED);
    UmiReleaseTestPolicyTextDestroy(text); text = NULL; size = 321U;
    char *owner = malloc(UMI_RELEASE_INVENTORY_FIELD_LIMIT + 2U); CHECK(owner != NULL);
    memset(owner, 'x', UMI_RELEASE_INVENTORY_FIELD_LIMIT + 1U); owner[UMI_RELEASE_INVENTORY_FIELD_LIMIT + 1U] = '\0';
    CHECK(UmiReleaseTestPolicyEdit(policy, "a", UMI_RELEASE_TEST_REQUIRED, owner, "r", &text, &size) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(text == NULL && size == 321U); free(owner);
    UmiReleaseTestPolicy *out = NULL;
    CHECK(UmiReleaseTestPolicyParse("", UMI_RELEASE_INVENTORY_TEXT_LIMIT + 1U, &out) == UMI_STATUS_CAPACITY_EXCEEDED && out == NULL);
    CHECK(Policy(POLICY, &policy) == UMI_STATUS_INVALID_ARGUMENT);
    UmiReleaseTestPolicyDestroy(reset); UmiReleaseTestPolicyDestroy(policy);
    return 0;
}

static int Large(void)
{
    const size_t capacity = 1024U * 1024U;
    char *source = malloc(capacity); CHECK(source != NULL);
    size_t used = strlen(CONFIG); memcpy(source, CONFIG, used + 1U);
    for (size_t i = 0U; i < 4097U; ++i) {
        int n = snprintf(source + used, capacity - used, "test\t%02x%02x%02x%02x\t\tregistered\t\n",
            (unsigned)(48U + i / 1000U), (unsigned)(48U + (i / 100U) % 10U),
            (unsigned)(48U + (i / 10U) % 10U), (unsigned)(48U + i % 10U));
        CHECK(n > 0 && (size_t)n < capacity - used); used += (size_t)n;
    }
    UmiReleaseInventory *configured = NULL; CHECK(Inventory(source, &configured) == UMI_STATUS_OK); free(source);
    char *text = NULL; size_t size = 0U;
    CHECK(UmiReleaseTestPolicyDraft(configured, &text, &size) == UMI_STATUS_OK);
    UmiReleaseTestPolicy *policy = NULL; CHECK(UmiReleaseTestPolicyParse(text, size, &policy) == UMI_STATUS_OK);
    CHECK(UmiReleaseTestPolicyCount(policy) == 4097U && strcmp(UmiReleaseTestPolicyAt(policy, 4096U)->name, "4096") == 0);
    UmiReleaseTestPolicyTextDestroy(text); UmiReleaseTestPolicyDestroy(policy); UmiReleaseInventoryDestroy(configured);
    return 0;
}
static int Empty(void)
{
    UmiReleaseInventory *configured = NULL, *observed = NULL;
    CHECK(Inventory(CONFIG, &configured) == UMI_STATUS_OK);
    CHECK(Inventory(OBSERVED A, &observed) == UMI_STATUS_OK);
    char *text = NULL; size_t size = 77U;
    CHECK(UmiReleaseTestPolicyDraft(configured, &text, &size) == UMI_STATUS_UNAVAILABLE);
    CHECK(UmiReleaseTestPolicyDraft(observed, &text, &size) == UMI_STATUS_INVALID_STATE);
    CHECK(text == NULL && size == 77U);
    UmiReleaseInventoryDestroy(configured); configured = NULL;
    CHECK(Inventory(CONFIG A, &configured) == UMI_STATUS_OK);
    UmiReleaseTestPolicy *policy = NULL; CHECK(Policy(POLICY, &policy) == UMI_STATUS_OK);
    UmiReleaseTestPolicyAssessment result = {0};
    CHECK(UmiReleaseTestPolicyAssess(configured, observed, policy, NULL, NULL, &result) == UMI_STATUS_OK);
    CHECK(!result.readyForExecution && result.missingRules == 1U);
    UmiReleaseTestPolicyDestroy(policy); UmiReleaseInventoryDestroy(observed); UmiReleaseInventoryDestroy(configured);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (strcmp(argv[1], "roundtrip") == 0) return Roundtrip();
    if (strcmp(argv[1], "parse_errors") == 0) return ParseErrors();
    if (strcmp(argv[1], "decisions") == 0) return Decisions();
    if (strcmp(argv[1], "drift") == 0) return Drift();
    if (strcmp(argv[1], "context") == 0) return ContextErrors();
    if (strcmp(argv[1], "edit_errors") == 0) return EditErrors();
    if (strcmp(argv[1], "large") == 0) return Large();
    if (strcmp(argv[1], "empty") == 0) return Empty();
    return 2;
}
