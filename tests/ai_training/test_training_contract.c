/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_contract.c
 * PURPOSE: Check bounded tensor creation, validation and graph ownership.
 * AUTHOR: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/tape.h"
#include <math.h>
#include <stdio.h>

#define REQUIRE(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "contract test failed at line %d: %s\n", __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(void)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTape *otherTape = NULL;
    UmiAiTrainingTensor *matrix = NULL;
    UmiAiTrainingTensor *broadcast = NULL;
    UmiAiTrainingTensor *result = NULL;
    UmiAiTrainingTensor *foreign = NULL;
    UmiAiTrainingTensor *untouched = NULL;
    float values[4] = {1.0F, 2.0F, 3.0F, 4.0F};
    float row[2] = {5.0F, -3.0F};
    float output[4] = {-100.0F, -100.0F, -100.0F, -100.0F};
    size_t rows = 0U;
    size_t columns = 0U;
    const float nanValue[] = {NAN};

    REQUIRE(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTapeCreate(&otherTape) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorCreate(tape, 2U, 2U, values, 4U, 0, &matrix) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorCreate(tape, 1U, 2U, row, 2U, 1, &broadcast) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorCreate(otherTape, 1U, 2U, row, 2U, 1, &foreign) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorCreate(tape, 0U, 1U, row, 1U, 0, &untouched) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(UmiAiTrainingTensorCreate(tape, 1U, 1U, nanValue, 1U, 0, &untouched) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(untouched == NULL);
    REQUIRE(UmiAiTrainingAdd(tape, matrix, foreign, &untouched) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(untouched == NULL);
    REQUIRE(UmiAiTrainingAdd(tape, matrix, broadcast, &result) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorShape(result, &rows, &columns) == UMI_STATUS_OK);
    REQUIRE(rows == 2U && columns == 2U);
    REQUIRE(UmiAiTrainingTensorValues(result, output, 3U) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(output[0] == -100.0F);
    REQUIRE(UmiAiTrainingTensorValues(result, output, 4U) == UMI_STATUS_OK);
    REQUIRE(output[0] == 6.0F && output[1] == -1.0F);
    REQUIRE(output[2] == 8.0F && output[3] == 1.0F);
    REQUIRE(UmiAiTrainingSgdStep(tape, broadcast, 0.1F) == UMI_STATUS_INVALID_STATE);
    REQUIRE(UmiAiTrainingTapeRewind(tape, 3U) == UMI_STATUS_INVALID_STATE);
    REQUIRE(UmiAiTrainingTapeRewind(tape, 2U) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorValues(matrix, output, 4U) == UMI_STATUS_OK);
    REQUIRE(output[0] == 1.0F && output[3] == 4.0F);
    REQUIRE(UmiAiTrainingTapeRewind(tape, 9U) == UMI_STATUS_INVALID_ARGUMENT);
    UmiAiTrainingTapeDestroy(tape);
    UmiAiTrainingTapeDestroy(otherTape);
    UmiAiTrainingTapeDestroy(NULL);
    puts("contract tests passed");
    return 0;
}
