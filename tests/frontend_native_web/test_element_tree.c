/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_element_tree.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web element tree.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "umicom/frontend/native_web/element_tree.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* The previous automatic-storage fixture is retained for review. Its large
 * aggregate values can exhaust a native thread stack before the first check.
 * The replacement owns those same records on the heap, checks allocation and
 * releases them after the checks, including a failed assertion path. */
#if 0
int main(void)
{
    UmiNativeWebElementTree t; UmiNativeWebSemanticElement a,b; umi_native_web_element_tree_init(&t); CHECK(umi_native_web_semantic_element_init(&a,"root","main") == UMI_STATUS_OK); CHECK(umi_native_web_element_tree_upsert(&t,&a) == UMI_STATUS_OK); CHECK(umi_native_web_semantic_element_init(&b,"child","section") == UMI_STATUS_OK); CHECK(umi_native_web_copy_text(b.parent_id,sizeof(b.parent_id),"root") == UMI_STATUS_OK); CHECK(umi_native_web_element_tree_upsert(&t,&b) == UMI_STATUS_OK); CHECK(t.count==2U); CHECK(umi_native_web_element_tree_remove(&t,"root") == UMI_STATUS_BUSY);
    return 0;
}

#endif

#include <stdlib.h>

/* The tree owns a bounded array of elements; parent and removal semantics stay
 * under test even though its backing memory is no longer on the process stack. */
static int CheckElementTree(UmiNativeWebElementTree *tree)
{
    UmiNativeWebSemanticElement root, child;
    umi_native_web_element_tree_init(tree);
    CHECK(umi_native_web_semantic_element_init(&root, "root", "main") == UMI_STATUS_OK);
    CHECK(umi_native_web_element_tree_upsert(tree, &root) == UMI_STATUS_OK);
    CHECK(umi_native_web_semantic_element_init(&child, "child", "section") == UMI_STATUS_OK);
    CHECK(umi_native_web_copy_text(child.parent_id, sizeof child.parent_id, "root") == UMI_STATUS_OK);
    CHECK(umi_native_web_element_tree_upsert(tree, &child) == UMI_STATUS_OK);
    CHECK(tree->count == 2U);
    CHECK(umi_native_web_element_tree_remove(tree, "root") == UMI_STATUS_BUSY);
    CHECK(umi_native_web_element_tree_remove(tree, "child") == UMI_STATUS_OK);
    CHECK(umi_native_web_element_tree_remove(tree, "root") == UMI_STATUS_OK);
    CHECK(tree->count == 0U);
    return 0;
}

int main(void)
{
    UmiNativeWebElementTree *tree = calloc(1U, sizeof *tree);
    if (tree == NULL) { fputs("Cannot allocate element tree fixture\n", stderr); return 1; }
    int result = CheckElementTree(tree);
    free(tree);
    return result;
}
