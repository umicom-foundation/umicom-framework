/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/tape.c
 *
 * PURPOSE:
 *   Implement bounded float32 tensors and reverse-mode autodifferentiation
 *   for the first native language-model training laboratory.
 *
 * OWNERSHIP AND SAFETY:
 *   Tapes own every node and buffer. Operators validate shape, provenance and
 *   finite inputs before committing a new node. No global mutable state.
 *   Gradients are accumulated in reverse topological (creation) order.
 *   The implementation is deliberately scalar and CPU-based: no third-party
 *   scripting runtimes or hidden GPU dependencies are required.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/tape.h"

#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef enum UmiAiTrainingOperation {
    UMI_AI_TRAINING_LEAF,
    UMI_AI_TRAINING_ADD,
    UMI_AI_TRAINING_MATMUL,
    UMI_AI_TRAINING_RELU,
    UMI_AI_TRAINING_MSE,
    UMI_AI_TRAINING_CROSS_ENTROPY,
    UMI_AI_TRAINING_EMBEDDING,
    UMI_AI_TRAINING_CAUSAL_ATTENTION
} UmiAiTrainingOperation;

struct UmiAiTrainingTensor {
    UmiAiTrainingTape *owner;
    size_t id;
    size_t rows;
    size_t columns;
    size_t count;
    float *values;
    float *gradients;
    size_t *targets; /* only cross-entropy operation nodes allocate these */
    const UmiAiTrainingTensor *left;
    const UmiAiTrainingTensor *right;
    const UmiAiTrainingTensor *third; /* value operand of attention */
    size_t sequenceLength; /* tokens in each independent sequence */
    float *attentionWeights; /* owned [batch*time, time] cache */
    UmiAiTrainingOperation operation;
    bool trainable;
};

struct UmiAiTrainingTape {
    UmiAiTrainingTensor *nodes[UMI_AI_TRAINING_MAX_TENSORS];
    size_t count;
    bool hasGradients;
};

/* Verify a handle by its slot without dereferencing caller-provided memory
 * before checking it belongs to a live member of this tape. A stale handle
 * must not be used; as for other opaque C APIs, the caller must still preserve
 * the lifetime contract of handles returned by this service. */
static bool IsMember(const UmiAiTrainingTape *tape,
                     const UmiAiTrainingTensor *tensor)
{
    if (tape == NULL || tensor == NULL) return false;
    for (size_t i = 0U; i < tape->count; ++i) {
        if (tape->nodes[i] == tensor) return true;
    }
    return false;
}

static bool ShapeIsValid(size_t rows, size_t columns)
{
    return rows > 0U && columns > 0U &&
           columns <= UMI_AI_TRAINING_MAX_ELEMENTS &&
           rows <= UMI_AI_TRAINING_MAX_ELEMENTS / columns;
}

static bool CanStore(double value)
{
    return isfinite(value) && value >= -(double)FLT_MAX && value <= (double)FLT_MAX;
}

/* Nodes are allocated off-tape, calculated and validated, then published.
 * This guarantees allocation/numeric failure cannot leave partial graph nodes. */
static UmiAiTrainingTensor *AllocateNode(UmiAiTrainingTape *tape,
                                        size_t rows, size_t columns)
{
    UmiAiTrainingTensor *node;
    if (tape == NULL || tape->count >= UMI_AI_TRAINING_MAX_TENSORS ||
        !ShapeIsValid(rows, columns)) return NULL;
    node = calloc(1U, sizeof(*node));
    if (node == NULL) return NULL;
    node->count = rows * columns;
    node->values = calloc(node->count, sizeof(*node->values));
    node->gradients = calloc(node->count, sizeof(*node->gradients));
    if (node->values == NULL || node->gradients == NULL) {
        free(node->values);
        free(node->gradients);
        free(node);
        return NULL;
    }
    node->owner = tape;
    node->rows = rows;
    node->columns = columns;
    return node;
}

static void FreeNode(UmiAiTrainingTensor *node)
{
    if (node == NULL) return;
    free(node->targets);
    free(node->attentionWeights);
    free(node->gradients);
    free(node->values);
    free(node);
}

static UmiStatus PublishNode(UmiAiTrainingTape *tape, UmiAiTrainingTensor *node,
                            UmiAiTrainingTensor **outTensor)
{
    if (tape == NULL || node == NULL || outTensor == NULL ||
        tape->count >= UMI_AI_TRAINING_MAX_TENSORS) return UMI_STATUS_INVALID_ARGUMENT;
    node->id = tape->count;
    tape->nodes[tape->count++] = node;
    tape->hasGradients = false;
    *outTensor = node;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingTapeCreate(UmiAiTrainingTape **outTape)
{
    UmiAiTrainingTape *newTape;
    if (outTape == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    newTape = calloc(1U, sizeof(*newTape));
    if (newTape == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *outTape = newTape;
    return UMI_STATUS_OK;
}

void UmiAiTrainingTapeDestroy(UmiAiTrainingTape *tape)
{
    if (tape == NULL) return;
    for (size_t i = 0U; i < tape->count; ++i) FreeNode(tape->nodes[i]);
    free(tape);
}

UmiStatus UmiAiTrainingTapeRewind(UmiAiTrainingTape *tape,
                                 size_t retainLeafCount)
{
    if (tape == NULL || retainLeafCount > tape->count) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < retainLeafCount; ++i) {
        if (tape->nodes[i]->operation != UMI_AI_TRAINING_LEAF) {
            return UMI_STATUS_INVALID_STATE;
        }
    }
    for (size_t i = retainLeafCount; i < tape->count; ++i) {
        FreeNode(tape->nodes[i]);
        tape->nodes[i] = NULL;
    }
    tape->count = retainLeafCount;
    tape->hasGradients = false;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingTensorCreate(UmiAiTrainingTape *tape,
                                   size_t rows, size_t columns,
                                   const float *values, size_t valueCount,
                                   int trainable,
                                   UmiAiTrainingTensor **outTensor)
{
    UmiAiTrainingTensor *node;
    if (tape == NULL || outTensor == NULL || values == NULL ||
        !ShapeIsValid(rows, columns) || valueCount != rows * columns) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (tape->count >= UMI_AI_TRAINING_MAX_TENSORS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (size_t i = 0U; i < valueCount; ++i) {
        if (!isfinite(values[i])) return UMI_STATUS_INVALID_ARGUMENT;
    }
    node = AllocateNode(tape, rows, columns);
    if (node == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(node->values, values, valueCount * sizeof(*values));
    node->operation = UMI_AI_TRAINING_LEAF;
    node->trainable = trainable != 0;
    return PublishNode(tape, node, outTensor);
}

UmiStatus UmiAiTrainingTensorValues(const UmiAiTrainingTensor *tensor,
                                   float *outValues, size_t capacity)
{
    if (tensor == NULL || outValues == NULL || capacity < tensor->count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memcpy(outValues, tensor->values, tensor->count * sizeof(*outValues));
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingTensorGradients(const UmiAiTrainingTensor *tensor,
                                      float *outGradients, size_t capacity)
{
    if (tensor == NULL || outGradients == NULL || capacity < tensor->count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memcpy(outGradients, tensor->gradients, tensor->count * sizeof(*outGradients));
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingTensorShape(const UmiAiTrainingTensor *tensor,
                                  size_t *outRows, size_t *outColumns)
{
    if (tensor == NULL || outRows == NULL || outColumns == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *outRows = tensor->rows;
    *outColumns = tensor->columns;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingAdd(UmiAiTrainingTape *tape,
                          const UmiAiTrainingTensor *left,
                          const UmiAiTrainingTensor *right,
                          UmiAiTrainingTensor **outTensor)
{
    UmiAiTrainingTensor *node;
    size_t rows;
    if (!IsMember(tape, left) || !IsMember(tape, right) || outTensor == NULL ||
        left->columns != right->columns ||
        (left->rows != right->rows && left->rows != 1U && right->rows != 1U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    rows = left->rows > right->rows ? left->rows : right->rows;
    if (tape->count >= UMI_AI_TRAINING_MAX_TENSORS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    node = AllocateNode(tape, rows, left->columns);
    if (node == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t r = 0U; r < rows; ++r) {
        for (size_t c = 0U; c < node->columns; ++c) {
            size_t i = r * node->columns + c;
            size_t li = (left->rows == 1U ? 0U : r) * left->columns + c;
            size_t ri = (right->rows == 1U ? 0U : r) * right->columns + c;
            double sum = (double)left->values[li] + (double)right->values[ri];
            if (!CanStore(sum)) { FreeNode(node); return UMI_STATUS_INVALID_ARGUMENT; }
            node->values[i] = (float)sum;
        }
    }
    node->left = left;
    node->right = right;
    node->operation = UMI_AI_TRAINING_ADD;
    return PublishNode(tape, node, outTensor);
}

UmiStatus UmiAiTrainingMatmul(UmiAiTrainingTape *tape,
                             const UmiAiTrainingTensor *left,
                             const UmiAiTrainingTensor *right,
                             UmiAiTrainingTensor **outTensor)
{
    UmiAiTrainingTensor *node;
    if (!IsMember(tape, left) || !IsMember(tape, right) || outTensor == NULL ||
        left->columns != right->rows) return UMI_STATUS_INVALID_ARGUMENT;
    if (!ShapeIsValid(left->rows, right->columns)) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (tape->count >= UMI_AI_TRAINING_MAX_TENSORS) return UMI_STATUS_CAPACITY_EXCEEDED;
    node = AllocateNode(tape, left->rows, right->columns);
    if (node == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t r = 0U; r < node->rows; ++r) {
        for (size_t c = 0U; c < node->columns; ++c) {
            double sum = 0.0;
            for (size_t k = 0U; k < left->columns; ++k) {
                sum += (double)left->values[r * left->columns + k] *
                       (double)right->values[k * right->columns + c];
            }
            if (!CanStore(sum)) { FreeNode(node); return UMI_STATUS_INVALID_ARGUMENT; }
            node->values[r * node->columns + c] = (float)sum;
        }
    }
    node->left = left;
    node->right = right;
    node->operation = UMI_AI_TRAINING_MATMUL;
    return PublishNode(tape, node, outTensor);
}

UmiStatus UmiAiTrainingRelu(UmiAiTrainingTape *tape,
                           const UmiAiTrainingTensor *input,
                           UmiAiTrainingTensor **outTensor)
{
    UmiAiTrainingTensor *node;
    if (!IsMember(tape, input) || outTensor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (tape->count >= UMI_AI_TRAINING_MAX_TENSORS) return UMI_STATUS_CAPACITY_EXCEEDED;
    node = AllocateNode(tape, input->rows, input->columns);
    if (node == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t i = 0U; i < input->count; ++i) {
        node->values[i] = input->values[i] > 0.0F ? input->values[i] : 0.0F;
    }
    node->left = input;
    node->operation = UMI_AI_TRAINING_RELU;
    return PublishNode(tape, node, outTensor);
}

UmiStatus UmiAiTrainingMeanSquaredError(UmiAiTrainingTape *tape,
                                       const UmiAiTrainingTensor *prediction,
                                       const UmiAiTrainingTensor *target,
                                       UmiAiTrainingTensor **outLoss)
{
    UmiAiTrainingTensor *node;
    double mse = 0.0;
    if (!IsMember(tape, prediction) || !IsMember(tape, target) || outLoss == NULL ||
        prediction->rows != target->rows || prediction->columns != target->columns) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (tape->count >= UMI_AI_TRAINING_MAX_TENSORS) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < prediction->count; ++i) {
        double delta = (double)prediction->values[i] - (double)target->values[i];
        mse += (delta * delta) / (double)prediction->count;
        if (!CanStore(mse)) return UMI_STATUS_INVALID_ARGUMENT;
    }
    node = AllocateNode(tape, 1U, 1U);
    if (node == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    node->values[0] = (float)mse;
    node->left = prediction;
    node->right = target;
    node->operation = UMI_AI_TRAINING_MSE;
    return PublishNode(tape, node, outLoss);
}

UmiStatus UmiAiTrainingCrossEntropy(UmiAiTrainingTape *tape,
                                   const UmiAiTrainingTensor *logits,
                                   const size_t *targets, size_t targetCount,
                                   UmiAiTrainingTensor **outLoss)
{
    UmiAiTrainingTensor *node;
    double average = 0.0;
    if (!IsMember(tape, logits) || targets == NULL || outLoss == NULL ||
        targetCount != logits->rows) return UMI_STATUS_INVALID_ARGUMENT;
    if (tape->count >= UMI_AI_TRAINING_MAX_TENSORS) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t r = 0U; r < logits->rows; ++r) {
        double maxLogit = (double)logits->values[r * logits->columns];
        double expSum = 0.0;
        if (targets[r] >= logits->columns) return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t c = 1U; c < logits->columns; ++c) {
            double candidate = (double)logits->values[r * logits->columns + c];
            if (candidate > maxLogit) maxLogit = candidate;
        }
        for (size_t c = 0U; c < logits->columns; ++c) {
            expSum += exp((double)logits->values[r * logits->columns + c] - maxLogit);
        }
        average += (maxLogit + log(expSum) -
                    (double)logits->values[r * logits->columns + targets[r]]) /
                   (double)logits->rows;
        if (!CanStore(average)) return UMI_STATUS_INVALID_ARGUMENT;
    }
    node = AllocateNode(tape, 1U, 1U);
    if (node == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    node->targets = malloc(targetCount * sizeof(*targets));
    if (node->targets == NULL) { FreeNode(node); return UMI_STATUS_OUT_OF_MEMORY; }
    memcpy(node->targets, targets, targetCount * sizeof(*targets));
    node->values[0] = (float)average;
    node->left = logits;
    node->operation = UMI_AI_TRAINING_CROSS_ENTROPY;
    return PublishNode(tape, node, outLoss);
}

/* Token embeddings use an integer-indexed gather rather than a one-hot
 * [tokens, vocabulary] matrix. Their reverse-mode derivative is a scatter-add
 * into the original embedding table, so repeated indices remain correct. */
UmiStatus UmiAiTrainingEmbeddingLookup(UmiAiTrainingTape *tape,
                                      const UmiAiTrainingTensor *embeddingTable,
                                      const size_t *tokenIds, size_t tokenCount,
                                      UmiAiTrainingTensor **outTensor)
{
    UmiAiTrainingTensor *node;
    if (!IsMember(tape, embeddingTable) || tokenIds == NULL ||
        outTensor == NULL || tokenCount == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (!ShapeIsValid(tokenCount, embeddingTable->columns)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (tape->count >= UMI_AI_TRAINING_MAX_TENSORS) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t t = 0U; t < tokenCount; ++t) {
        if (tokenIds[t] >= embeddingTable->rows) return UMI_STATUS_INVALID_ARGUMENT;
    }
    node = AllocateNode(tape, tokenCount, embeddingTable->columns);
    if (node == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    node->targets = malloc(tokenCount * sizeof(*node->targets));
    if (node->targets == NULL) { FreeNode(node); return UMI_STATUS_OUT_OF_MEMORY; }
    memcpy(node->targets, tokenIds, tokenCount * sizeof(*tokenIds));
    for (size_t t = 0U; t < tokenCount; ++t) {
        memcpy(node->values + t * node->columns,
               embeddingTable->values + tokenIds[t] * embeddingTable->columns,
               node->columns * sizeof(*node->values));
    }
    node->left = embeddingTable;
    node->operation = UMI_AI_TRAINING_EMBEDDING;
    return PublishNode(tape, node, outTensor);
}

/* Causal attention sees a rank-three logical sequence [batch,time,channels]
 * through the existing row-major [batch*time,channels] tape allocation.
 * Every row derives its own sequence boundary from sequenceLength. Inputs
 * must be finite; the dot product is evaluated with double intermediates and
 * scores are normalised by subtracting the row maximum before exp(). */
UmiStatus UmiAiTrainingCausalAttention(UmiAiTrainingTape *tape,
                                      const UmiAiTrainingTensor *queries,
                                      const UmiAiTrainingTensor *keys,
                                      const UmiAiTrainingTensor *values,
                                      size_t sequenceLength,
                                      UmiAiTrainingTensor **outTensor)
{
    UmiAiTrainingTensor *node;
    double *scores;
    double scale;
    if (!IsMember(tape, queries) || !IsMember(tape, keys) ||
        !IsMember(tape, values) || outTensor == NULL || sequenceLength == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (queries->rows != keys->rows || keys->rows != values->rows ||
        queries->columns != keys->columns ||
        queries->rows % sequenceLength != 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (!ShapeIsValid(queries->rows, values->columns) ||
        sequenceLength > UMI_AI_TRAINING_MAX_ELEMENTS / queries->rows) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (tape->count >= UMI_AI_TRAINING_MAX_TENSORS) return UMI_STATUS_CAPACITY_EXCEEDED;
    scale = sqrt((double)queries->columns);
    node = AllocateNode(tape, queries->rows, values->columns);
    if (node == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    node->attentionWeights = calloc(queries->rows * sequenceLength,
                                    sizeof(*node->attentionWeights));
    scores = malloc(sequenceLength * sizeof(*scores));
    if (node->attentionWeights == NULL || scores == NULL) {
        free(scores);
        FreeNode(node);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t row = 0U; row < queries->rows; ++row) {
        size_t batchStart = (row / sequenceLength) * sequenceLength;
        size_t currentTime = row % sequenceLength;
        double maximum = -INFINITY;
        double denominator = 0.0;
        for (size_t past = 0U; past <= currentTime; ++past) {
            double dot = 0.0;
            size_t keyRow = batchStart + past;
            for (size_t channel = 0U; channel < queries->columns; ++channel) {
                dot += (double)queries->values[row * queries->columns + channel] *
                       (double)keys->values[keyRow * keys->columns + channel];
            }
            scores[past] = dot / scale;
            if (scores[past] > maximum) maximum = scores[past];
        }
        for (size_t past = 0U; past <= currentTime; ++past) {
            denominator += exp(scores[past] - maximum);
        }
        for (size_t past = 0U; past <= currentTime; ++past) {
            double probability = exp(scores[past] - maximum) / denominator;
            node->attentionWeights[row * sequenceLength + past] = (float)probability;
        }
        for (size_t channel = 0U; channel < values->columns; ++channel) {
            double result = 0.0;
            for (size_t past = 0U; past <= currentTime; ++past) {
                size_t valueRow = batchStart + past;
                result += (double)node->attentionWeights[row * sequenceLength + past] *
                          (double)values->values[valueRow * values->columns + channel];
            }
            if (!CanStore(result)) {
                free(scores);
                FreeNode(node);
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            node->values[row * node->columns + channel] = (float)result;
        }
    }
    free(scores);
    node->left = queries;
    node->right = keys;
    node->third = values;
    node->sequenceLength = sequenceLength;
    node->operation = UMI_AI_TRAINING_CAUSAL_ATTENTION;
    return PublishNode(tape, node, outTensor);
}

/* Accumulation is explicit so a tensor shared by two operators receives both
 * contributions. Using double intermediates avoids silent float overflow in
 * intermediate dot products and derivative products. */
static bool Accumulate(float *destination, double contribution)
{
    double total;
    if (!CanStore(contribution)) return false;
    total = (double)*destination + contribution;
    if (!CanStore(total)) return false;
    *destination = (float)total;
    return true;
}

static bool BackwardNode(UmiAiTrainingTensor *node)
{
    UmiAiTrainingTensor *left = (UmiAiTrainingTensor *)node->left;
    UmiAiTrainingTensor *right = (UmiAiTrainingTensor *)node->right;
    switch (node->operation) {
    case UMI_AI_TRAINING_LEAF:
        return true;
    case UMI_AI_TRAINING_ADD:
        for (size_t r = 0U; r < node->rows; ++r) {
            for (size_t c = 0U; c < node->columns; ++c) {
                size_t i = r * node->columns + c;
                size_t li = (left->rows == 1U ? 0U : r) * left->columns + c;
                size_t ri = (right->rows == 1U ? 0U : r) * right->columns + c;
                if (!Accumulate(&left->gradients[li], (double)node->gradients[i]) ||
                    !Accumulate(&right->gradients[ri], (double)node->gradients[i])) return false;
            }
        }
        return true;
    case UMI_AI_TRAINING_MATMUL:
        for (size_t r = 0U; r < node->rows; ++r) {
            for (size_t c = 0U; c < node->columns; ++c) {
                double upstream = (double)node->gradients[r * node->columns + c];
                for (size_t k = 0U; k < left->columns; ++k) {
                    size_t li = r * left->columns + k;
                    size_t ri = k * right->columns + c;
                    if (!Accumulate(&left->gradients[li], upstream * (double)right->values[ri]) ||
                        !Accumulate(&right->gradients[ri], upstream * (double)left->values[li])) {
                        return false;
                    }
                }
            }
        }
        return true;
    case UMI_AI_TRAINING_RELU:
        for (size_t i = 0U; i < node->count; ++i) {
            if (left->values[i] > 0.0F &&
                !Accumulate(&left->gradients[i], (double)node->gradients[i])) return false;
        }
        return true;
    case UMI_AI_TRAINING_MSE:
        for (size_t i = 0U; i < left->count; ++i) {
            double delta = (double)left->values[i] - (double)right->values[i];
            double gradient = 2.0 * delta * (double)node->gradients[0] / (double)left->count;
            if (!Accumulate(&left->gradients[i], gradient) ||
                !Accumulate(&right->gradients[i], -gradient)) return false;
        }
        return true;
    case UMI_AI_TRAINING_CROSS_ENTROPY:
        for (size_t r = 0U; r < left->rows; ++r) {
            double maxLogit = (double)left->values[r * left->columns];
            double expSum = 0.0;
            for (size_t c = 1U; c < left->columns; ++c) {
                double value = (double)left->values[r * left->columns + c];
                if (value > maxLogit) maxLogit = value;
            }
            for (size_t c = 0U; c < left->columns; ++c) {
                expSum += exp((double)left->values[r * left->columns + c] - maxLogit);
            }
            for (size_t c = 0U; c < left->columns; ++c) {
                double probability = exp((double)left->values[r * left->columns + c] - maxLogit) / expSum;
                double indicator = c == node->targets[r] ? 1.0 : 0.0;
                double gradient = (probability - indicator) *
                                  (double)node->gradients[0] / (double)left->rows;
                if (!Accumulate(&left->gradients[r * left->columns + c], gradient)) return false;
            }
        }
        return true;
    case UMI_AI_TRAINING_EMBEDDING:
        /* A discrete token index is not differentiable. Embedding rows share
         * gradients across every occurrence of the same token. */
        for (size_t token = 0U; token < node->rows; ++token) {
            size_t tableRow = node->targets[token];
            for (size_t channel = 0U; channel < node->columns; ++channel) {
                size_t sourceIndex = token * node->columns + channel;
                size_t tableIndex = tableRow * left->columns + channel;
                if (!Accumulate(&left->gradients[tableIndex],
                                (double)node->gradients[sourceIndex])) return false;
            }
        }
        return true;
    case UMI_AI_TRAINING_CAUSAL_ATTENTION: {
        UmiAiTrainingTensor *values = (UmiAiTrainingTensor *)node->third;
        double scale = sqrt((double)left->columns);
        for (size_t row = 0U; row < node->rows; ++row) {
            size_t batchStart = (row / node->sequenceLength) * node->sequenceLength;
            size_t currentTime = row % node->sequenceLength;
            double expectedDot = 0.0;
            /* Differentiating softmax yields p_j * (dp_j - sum_k p_k dp_k).
             * The first pass calculates the shared expected derivative. */
            for (size_t past = 0U; past <= currentTime; ++past) {
                double dot = 0.0;
                size_t valueRow = batchStart + past;
                for (size_t channel = 0U; channel < values->columns; ++channel) {
                    dot += (double)node->gradients[row * node->columns + channel] *
                           (double)values->values[valueRow * values->columns + channel];
                }
                expectedDot += (double)node->attentionWeights[row * node->sequenceLength + past] * dot;
            }
            for (size_t past = 0U; past <= currentTime; ++past) {
                size_t keyRow = batchStart + past;
                double probability = (double)node->attentionWeights[row * node->sequenceLength + past];
                double dot = 0.0;
                for (size_t channel = 0U; channel < values->columns; ++channel) {
                    size_t index = keyRow * values->columns + channel;
                    double upstream = (double)node->gradients[row * node->columns + channel];
                    dot += upstream * (double)values->values[index];
                    if (!Accumulate(&values->gradients[index], probability * upstream)) return false;
                }
                double scoreGradient = probability * (dot - expectedDot) / scale;
                for (size_t channel = 0U; channel < left->columns; ++channel) {
                    size_t queryIndex = row * left->columns + channel;
                    size_t keyIndex = keyRow * right->columns + channel;
                    if (!Accumulate(&left->gradients[queryIndex],
                                    scoreGradient * (double)right->values[keyIndex]) ||
                        !Accumulate(&right->gradients[keyIndex],
                                    scoreGradient * (double)left->values[queryIndex])) return false;
                }
            }
        }
        return true;
    }
    }
    return false;
}

UmiStatus UmiAiTrainingBackward(UmiAiTrainingTape *tape,
                               const UmiAiTrainingTensor *loss)
{
    if (!IsMember(tape, loss) || loss->count != 1U) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < tape->count; ++i) {
        memset(tape->nodes[i]->gradients, 0,
               tape->nodes[i]->count * sizeof(*tape->nodes[i]->gradients));
    }
    tape->hasGradients = false;
    ((UmiAiTrainingTensor *)loss)->gradients[0] = 1.0F;
    /* Computing gradients through each node in reverse creation order is the
     * reverse-mode chain rule. Edges only point to earlier nodes. */
    for (size_t next = tape->count; next > 0U; --next) {
        if (!BackwardNode(tape->nodes[next - 1U])) return UMI_STATUS_INVALID_STATE;
    }
    tape->hasGradients = true;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingSgdStep(UmiAiTrainingTape *tape,
                              UmiAiTrainingTensor *parameter,
                              float learningRate)
{
    if (!IsMember(tape, parameter) || !parameter->trainable ||
        parameter->operation != UMI_AI_TRAINING_LEAF ||
        !isfinite(learningRate) || learningRate <= 0.0F) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* A valid call made before Backward is a lifecycle error, not malformed
     * data. Keep this distinction visible for Studio and the training console. */
    if (!tape->hasGradients) return UMI_STATUS_INVALID_STATE;
    /* A two-phase update avoids partially changing a parameter array when
     * a later element would overflow float32. */
    for (size_t i = 0U; i < parameter->count; ++i) {
        double updated = (double)parameter->values[i] -
                         (double)learningRate * (double)parameter->gradients[i];
        if (!CanStore(updated)) return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (size_t i = 0U; i < parameter->count; ++i) {
        parameter->values[i] = (float)((double)parameter->values[i] -
                        (double)learningRate * (double)parameter->gradients[i]);
    }
    return UMI_STATUS_OK;
}
