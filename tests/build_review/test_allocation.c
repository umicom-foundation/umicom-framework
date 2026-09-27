/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build_review/test_allocation.c
 *
 * PURPOSE:
 *   Inject native allocator failures at each new review-allocation boundary on supported linkers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/review.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
static unsigned failAt, allocation;
void *__real_calloc(size_t count, size_t size);
void *__real_malloc(size_t size);
void *__wrap_calloc(size_t count, size_t size)
{
    if (failAt != 0U && ++allocation == failAt) return NULL;
    return __real_calloc(count, size);
}
void *__wrap_malloc(size_t size)
{
    if (failAt != 0U && ++allocation == failAt) return NULL;
    return __real_malloc(size);
}
int main(int argc, char **argv)
{
    UmiBuildHistory *history = NULL;
    UmiBuildReview *review = NULL;
    UmiStatus status;
    char *text = NULL;
    size_t length = 0U;
    if (argc != 2) return 2;
    if (umi_build_history_create(2U, &history) != UMI_STATUS_OK) return 1;
    if (strcmp(argv[1], "first") == 0 || strcmp(argv[1], "second") == 0) {
        failAt = strcmp(argv[1], "first") == 0 ? 1U : 2U;
        status = UmiBuildReviewCapture(history, 2U, &review);
    } else if (strcmp(argv[1], "render") == 0) {
        if (UmiBuildReviewImportLog("notes.c:1:1: error: invalid", strlen("notes.c:1:1: error: invalid"), &review) != UMI_STATUS_OK) {
            umi_build_history_destroy(history); return 1;
        }
        failAt = 1U;
        status = UmiBuildReviewRender(review, 0U, NULL, &text, &length);
    } else { umi_build_history_destroy(history); return 2; }
    failAt = 0U;
    UmiBuildReviewTextFree(text);
    UmiBuildReviewDestroy(review);
    umi_build_history_destroy(history);
    if (status != UMI_STATUS_OUT_OF_MEMORY || text != NULL || length != 0U) {
        fprintf(stderr, "Unexpected allocation-failure status %d\n", (int)status); return 1;
    }
    return 0;
}
