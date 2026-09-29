/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Fail parser and serializer allocations without publishing partial objects. */
#include "umicom/distribution/runtime/test_policy.h"
#include <stdlib.h>
#include <string.h>
void *__real_malloc(size_t size);
void *__real_calloc(size_t count, size_t size);
void *__wrap_malloc(size_t size);
void *__wrap_calloc(size_t count, size_t size);
static bool failing;
static size_t remaining;
void *__wrap_malloc(size_t size)
{ if (failing && remaining-- == 0U) return NULL; return __real_malloc(size); }
void *__wrap_calloc(size_t count, size_t size)
{ if (failing && remaining-- == 0U) return NULL; return __real_calloc(count, size); }
int main(void)
{
    const char policyText[] = "UMICOM-RELEASE-TEST-POLICY\t1\ncontext\tpolicy\t0123456789abcdef0123456789abcdef\t2f\t2f\t61\ntest\t61\t\tunassigned\t\n";
    for (size_t i = 0U; i < 3U; ++i) {
        UmiReleaseTestPolicy *policy = NULL; remaining = i; failing = true;
        UmiStatus status = UmiReleaseTestPolicyParse(policyText, strlen(policyText), &policy); failing = false;
        if (status != UMI_STATUS_OUT_OF_MEMORY || policy != NULL) { UmiReleaseTestPolicyDestroy(policy); return 1; }
    }
    UmiReleaseTestPolicy *policy = NULL;
    if (UmiReleaseTestPolicyParse(policyText, strlen(policyText), &policy) != UMI_STATUS_OK) return 1;
    char *text = NULL; size_t length = 777U; remaining = 0U; failing = true;
    UmiStatus status = UmiReleaseTestPolicyEdit(policy, "a", UMI_RELEASE_TEST_REQUIRED, "owner", "reason", &text, &length);
    failing = false;
    UmiReleaseTestPolicyDestroy(policy);
    if (status != UMI_STATUS_OUT_OF_MEMORY || text != NULL || length != 777U) { UmiReleaseTestPolicyTextDestroy(text); return 1; }
    const char inventoryText[] = "UMICOM-RELEASE-INVENTORY\t1\ncontext\tcmake\t0123456789abcdef0123456789abcdef\t2f\t2f\t61\ntest\t61\t\tregistered\t\n";
    UmiReleaseInventory *inventory = NULL;
    if (UmiReleaseInventoryParse(inventoryText, strlen(inventoryText), &inventory) != UMI_STATUS_OK) return 1;
    remaining = 0U; failing = true;
    status = UmiReleaseTestPolicyDraft(inventory, &text, &length); failing = false;
    UmiReleaseInventoryDestroy(inventory);
    if (status != UMI_STATUS_OUT_OF_MEMORY || text != NULL || length != 777U) { UmiReleaseTestPolicyTextDestroy(text); return 1; }
    return 0;
}
