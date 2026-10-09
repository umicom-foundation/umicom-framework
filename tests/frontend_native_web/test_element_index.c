/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_element_index.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web element index.
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
#include "umicom/frontend/native_web/element_index.h"
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
    UmiNativeWebElementTree t; UmiNativeWebSemanticElement e; UmiNativeWebElementIndex idx; size_t pos=99U; umi_native_web_element_tree_init(&t); CHECK(umi_native_web_semantic_element_init(&e,"root","main")==UMI_STATUS_OK); CHECK(umi_native_web_element_tree_upsert(&t,&e)==UMI_STATUS_OK); CHECK(umi_native_web_element_index_build(&idx,&t)==UMI_STATUS_OK); CHECK(umi_native_web_element_index_find(&idx,"root",&pos)==UMI_STATUS_OK); CHECK(pos==0U);
    return 0;
}

#endif

#include <stdlib.h>

/* Only the tree's large backing storage needs the heap. The index and one
 * element are small working values, and their lookup contract is unchanged. */
static int CheckElementIndex(UmiNativeWebElementTree *tree)
{
    UmiNativeWebSemanticElement element;
    UmiNativeWebElementIndex index;
    size_t position = 99U;
    umi_native_web_element_tree_init(tree);
    CHECK(umi_native_web_semantic_element_init(&element, "root", "main") == UMI_STATUS_OK);
    CHECK(umi_native_web_element_tree_upsert(tree, &element) == UMI_STATUS_OK);
    CHECK(umi_native_web_element_index_build(&index, tree) == UMI_STATUS_OK);
    CHECK(umi_native_web_element_index_find(&index, "root", &position) == UMI_STATUS_OK);
    CHECK(position == 0U);
    CHECK(umi_native_web_element_index_find(&index, "missing", &position) == UMI_STATUS_NOT_FOUND);
    return 0;
}

int main(void)
{
    UmiNativeWebElementTree *tree = calloc(1U, sizeof *tree);
    if (tree == NULL) { fputs("Cannot allocate element index fixture\n", stderr); return 1; }
    int result = CheckElementIndex(tree);
    free(tree);
    return result;
}
