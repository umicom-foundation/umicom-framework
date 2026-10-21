/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_sequence_shape.c
 * PURPOSE: Verify bounded rank-three sequence layout and checked offsets.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/sequence_shape.h"

#include <stdint.h>
#include <stdio.h>

#define VERIFY(ok) do { if (!(ok)) { \
    fprintf(stderr, "sequence layout verification failed, line %d: %s\n", \
        __LINE__, #ok); return 1; } } while (0)

int main(void)
{
    UmiAiTrainingSequenceShape shape = {11U, 12U, 13U};
    size_t rowCount = 991U;
    size_t channelCount = 992U;
    size_t offset = 999U;
    VERIFY(UmiAiTrainingSequenceShapeCreate(2U, 3U, 4U, &shape) == UMI_STATUS_OK);
    VERIFY(shape.batch == 2U && shape.time == 3U && shape.channels == 4U);
    VERIFY(UmiAiTrainingSequenceFlatten(&shape, &rowCount, &channelCount)
           == UMI_STATUS_OK);
    VERIFY(rowCount == 6U && channelCount == 4U);
    VERIFY(UmiAiTrainingSequenceOffset(&shape, 1U, 2U, 3U, &offset)
           == UMI_STATUS_OK);
    VERIFY(offset == 23U);
    VERIFY(UmiAiTrainingSequenceOffset(&shape, 0U, 1U, 2U, &offset)
           == UMI_STATUS_OK);
    VERIFY(offset == 6U);
    VERIFY(UmiAiTrainingSequenceOffset(&shape, 2U, 0U, 0U, &offset)
           == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(offset == 6U);
    VERIFY(UmiAiTrainingSequenceShapeCreate(2U, 3U, 900U, &shape)
           == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(shape.batch == 2U && shape.time == 3U && shape.channels == 4U);
    VERIFY(UmiAiTrainingSequenceShapeCreate(SIZE_MAX, 2U, 2U, &shape)
           == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(UmiAiTrainingSequenceShapeCreate(0U, 2U, 2U, &shape)
           == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(UmiAiTrainingSequenceFlatten(&shape, &rowCount, &rowCount)
           == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(rowCount == 6U && channelCount == 4U);
    puts("rank-three sequence shape verification passed");
    return 0;
}
