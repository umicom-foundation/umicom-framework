/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/tape.h
 *
 * PURPOSE:
 *   Native C23 dense-tensor and reverse-mode automatic-differentiation
 *   foundation for small, inspectable neural-network training experiments.
 *
 * ARCHITECTURE:
 *   A training tape owns every tensor. No application owns a tensor buffer or
 *   bypasses the Framework to compute gradients. The tape has explicit capacity
 *   limits; no unbounded computational graph is created by this laboratory.
 *   Callers serialise access; a tape is not thread-safe.
 *   The original bounded training tape is retained and now supports
 *   trainable multi-head attention, layer normalisation and native decoder
 *   composition through separate Framework-owned modules. The limits below
 *   describe the educational trainer, not a production-scale model engine.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_TAPE_H
#define UMICOM_AI_TRAINING_TAPE_H

#include <stddef.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The first laboratory favours bounded allocations and repeatable CPU tests. */
#define UMI_AI_TRAINING_MAX_TENSORS 256U
/* This upper bound applies to each individual tensor, including logits and
 * attention probability storage. It is not an aggregate training RAM budget.
 * Shape-dependent buffers are still protected by the tape's allocation policy. */
#define UMI_AI_TRAINING_MAX_ELEMENTS 16384U

typedef struct UmiAiTrainingTape UmiAiTrainingTape;
typedef struct UmiAiTrainingTensor UmiAiTrainingTensor;

/* Tape destruction and memory accounting also own retained squared-error
 * selection bytes. These bytes follow the same rewind/rollback lifecycle. */

/* Create an empty tape. On failure, the caller's output pointer is unchanged.
 * A successful caller owns the tape and must destroy it exactly once. */
UmiStatus UmiAiTrainingTapeCreate(UmiAiTrainingTape **outTape);

/* Destroying the tape releases all tensors, gradients and saved target indices.
 * Every tensor handle becomes invalid at this point. NULL is permitted. */
void UmiAiTrainingTapeDestroy(UmiAiTrainingTape *tape);

/* Memory accounting includes the tape object, its live tensor metadata,
 * value and gradient allocations, token-index arrays and retained attention
 * probabilities. It excludes allocator metadata, temporary per-operator work
 * buffers, decoder objects and C call stacks. The high-water mark tracks only
 * fully committed graph nodes. It is NOT process RSS or a total-process cap.
 * budgetBytes=0 disables the persistent-graph budget (the default).
 * A positive budget must not be smaller than the current allocation.
 * Rejections preserve the existing graph, budget, and output values. */
typedef struct UmiAiTrainingTapeMemory {
    size_t currentBytes;
    size_t peakBytes;
    size_t budgetBytes;
    size_t tensorCount;
    size_t tensorValuesBytes;
    size_t tensorGradientBytes;
    size_t auxiliaryBytes;
} UmiAiTrainingTapeMemory;

UmiStatus UmiAiTrainingTapeMemoryInspect(
    const UmiAiTrainingTape *tape,
    UmiAiTrainingTapeMemory *outMemory);

/* Set or clear a limit for committed tape-owned memory. The limit is checked
 * immediately BEFORE a computed tensor node becomes visible on the tape.
 * Failed publication releases its temporary node and leaves output handles
 * unchanged. Temporary scratch allocations are outside this accounting. */
UmiStatus UmiAiTrainingTapeSetMemoryBudget(
    UmiAiTrainingTape *tape, size_t budgetBytes);

/* Drop operation results after the first retainLeafCount tensors. Each retained
 * tensor must be an original leaf. This invalidates only discarded handles and
 * permits the same leaf parameters to be used for the next training iteration.
 * A failed rewind leaves the entire tape unchanged. */
UmiStatus UmiAiTrainingTapeRewind(UmiAiTrainingTape *tape, size_t retainLeafCount);

/* Create a row-major (rows x columns) leaf tensor, copying exactly valueCount
 * finite float values. A trainable leaf can be updated by SGD; nontrainable
 * leaves still receive gradients when they participate in the graph.
 * The tape owns all buffers. Valid dimensions are nonzero and rows*columns must
 * not exceed UMI_AI_TRAINING_MAX_ELEMENTS. */
UmiStatus UmiAiTrainingTensorCreate(UmiAiTrainingTape *tape,
                                   size_t rows, size_t columns,
                                   const float *values, size_t valueCount,
                                   int trainable,
                                   UmiAiTrainingTensor **outTensor);

/* Copy tensor data/gradients to caller-owned storage. capacity is in floats;
 * values beyond the tensor's element count are untouched. */
UmiStatus UmiAiTrainingTensorValues(const UmiAiTrainingTensor *tensor,
                                   float *outValues, size_t capacity);
UmiStatus UmiAiTrainingTensorGradients(const UmiAiTrainingTensor *tensor,
                                      float *outGradients, size_t capacity);

/* Read dimensions without exposing the tape-owned underlying buffer. */
UmiStatus UmiAiTrainingTensorShape(const UmiAiTrainingTensor *tensor,
                                  size_t *outRows, size_t *outColumns);

/* Element-wise add. Supports equal shapes and broadcasting a single row
 * (1 x columns) over a tensor's rows; gradients correctly sum broadcast rows. */
UmiStatus UmiAiTrainingAdd(UmiAiTrainingTape *tape,
                          const UmiAiTrainingTensor *left,
                          const UmiAiTrainingTensor *right,
                          UmiAiTrainingTensor **outTensor);

/* Matrix multiplication, left (M x K) times right (K x N). */
UmiStatus UmiAiTrainingMatmul(UmiAiTrainingTape *tape,
                             const UmiAiTrainingTensor *left,
                             const UmiAiTrainingTensor *right,
                             UmiAiTrainingTensor **outTensor);

/* Element-wise rectified linear unit: max(value, 0). */
UmiStatus UmiAiTrainingRelu(UmiAiTrainingTape *tape,
                           const UmiAiTrainingTensor *input,
                           UmiAiTrainingTensor **outTensor);

/* Mean squared error over equal-shaped prediction and target tensors.
 * Returns a 1x1 loss tensor. */
UmiStatus UmiAiTrainingMeanSquaredError(UmiAiTrainingTape *tape,
                                       const UmiAiTrainingTensor *prediction,
                                       const UmiAiTrainingTensor *target,
                                       UmiAiTrainingTensor **outLoss);

/* Mean squared error over individual values whose selection byte is 1.
 * Equal prediction/target shapes are required. selectedCount equals rows*columns;
 * each byte is 0 or 1, and at least one value must be selected. This is NOT a row
 * mask: callers expand a frame/pixel validity decision to every included channel.
 * The denominator is the number of selected values, not the full tensor size.
 * Both prediction and target receive opposite derivatives for selected values;
 * excluded values receive zero FROM THIS OPERATION. Other graph paths may still
 * contribute. An attention mask, frame decoder and image/video trainer are not
 * supplied by this scalar objective. All source tensors obey finite-value rules.
 *
 * The tape copies one byte per input value and accounts for it in auxiliaryBytes.
 * Callers may release their mask after return. Unmasked MeanSquaredError uses no
 * selection allocation and retains its all-value behavior. Allocation, numeric,
 * shape, capacity or mask refusal preserves the graph and outLoss. Arrays and
 * output handle storage remain separate, valid and stable during the call.
 * No input/target values are rewritten and no I/O occurs.
 */
UmiStatus UmiAiTrainingMaskedMeanSquaredError(UmiAiTrainingTape *tape,
    const UmiAiTrainingTensor *prediction, const UmiAiTrainingTensor *target,
    const unsigned char *selected, size_t selectedCount,
    UmiAiTrainingTensor **outLoss);


/* Normalized weighted squared error: sum(w[i] * (p[i]-t[i])^2) / sum(w).
 * Require one finite nonnegative float weight per tensor element and a positive
 * total weight. A zero weight removes that element's contribution. This is
 * NOT mean(w * error), row broadcasting, differentiable weights or an automatic
 * diffusion/SNR policy. Equal positive weights recover the unweighted objective;
 * binary weights recover the selected-element objective up to FP rounding.
 *
 * The tape copies weights into owned auxiliary storage and includes their
 * bytes in its graph budget and memory report. Callers may release their array
 * after success. Shapes, finite tensor values and provenance follow MSE rules.
 * Both operands receive opposite derivatives; shared graph paths still add.
 * Construction failure preserves the graph and outLoss. Inputs and output
 * handle storage must be valid, stable and separate during the call.
 */
UmiStatus UmiAiTrainingWeightedMeanSquaredError(UmiAiTrainingTape *tape,
    const UmiAiTrainingTensor *prediction, const UmiAiTrainingTensor *target,
    const float *weights, size_t weightCount, UmiAiTrainingTensor **outLoss);

/* Average categorical negative log-likelihood over logits (B x V).
 * Targets contain one class index per row, in [0, V). Uses a numerically
 * stable log-sum-exp calculation; no softmax probabilities are materialised.
 * Returns a 1x1 loss tensor. This is a teaching operator, not an LLM. */
UmiStatus UmiAiTrainingCrossEntropy(UmiAiTrainingTape *tape,
                                   const UmiAiTrainingTensor *logits,
                                   const size_t *targets, size_t targetCount,
                                   UmiAiTrainingTensor **outLoss);

/* Average the same categorical objective over rows whose selected byte is 1.
 * selectedCount and targetCount must equal logits rows; every mask byte is 0/1,
 * every target remains in [0,columns), and at least one row must be selected.
 * Excluded rows contribute zero to this operation's logit gradient. Other
 * graph paths can still contribute gradients to those rows or shared weights.
 * This is a LOSS mask, not an attention mask or a complete fine-tuning adapter.
 *
 * Targets and selection are copied into one tape-owned target allocation and
 * can be released after return. The existing graph budget includes that whole
 * allocation. No tensor layout, public handle or legacy all-row API changes.
 * Refusal preserves the graph and outLoss. Caller arrays and output handle
 * storage must remain valid, separate and unchanged throughout this call.
 */
UmiStatus UmiAiTrainingMaskedCrossEntropy(UmiAiTrainingTape *tape,
    const UmiAiTrainingTensor *logits, const size_t *targets, size_t targetCount,
    const unsigned char *selected, size_t selectedCount, UmiAiTrainingTensor **outLoss);

/* Reverse the recorded operations to differentiate a scalar loss. This clears
 * all prior gradients, so repeat Backward only for the currently built graph. */
UmiStatus UmiAiTrainingBackward(UmiAiTrainingTape *tape,
                               const UmiAiTrainingTensor *loss);

/* Stochastic-gradient-descent parameter update: weight -= rate * gradient.
 * Only original trainable leaf tensors can be updated. All candidate values
 * are checked before any weight is changed. Rewind before computing a fresh
 * forward pass with updated parameters. */
UmiStatus UmiAiTrainingSgdStep(UmiAiTrainingTape *tape,
                              UmiAiTrainingTensor *parameter,
                              float learningRate);

/* Gather rows from a [vocabulary, channels] embedding parameter table.
 * Input token IDs are copied into the tape node and may be released after
 * this call. The output is [tokenCount, channels]. The backward pass scatters
 * and SUMS gradients into table rows, including repeated token IDs.
 * No gradient exists with respect to the discrete token IDs. */
UmiStatus UmiAiTrainingEmbeddingLookup(UmiAiTrainingTape *tape,
                                      const UmiAiTrainingTensor *embeddingTable,
                                      const size_t *tokenIds, size_t tokenCount,
                                      UmiAiTrainingTensor **outTensor);

/* Trainable, single-head scaled dot-product CAUSAL self-attention.
 * Query, key and value matrices have [batch*time, channels] shape;
 * query/key channel widths must agree, while value width may differ.
 * sequenceLength is 'time': each sequence is handled independently and each
 * output token reads ONLY keys/values at earlier or equal time positions.
 *
 * A bounded [batch*time, time] probability cache is saved on the tape for
 * reverse-mode differentiation. The operator uses a stable masked softmax
 * and returns a [batch*time, valueChannels] tensor. The output pointer stays
 * unchanged on failure. It is not a complete transformer or multi-head layer. */
UmiStatus UmiAiTrainingCausalAttention(UmiAiTrainingTape *tape,
                                      const UmiAiTrainingTensor *queries,
                                      const UmiAiTrainingTensor *keys,
                                      const UmiAiTrainingTensor *values,
                                      size_t sequenceLength,
                                      UmiAiTrainingTensor **outTensor);


/* Capture the current graph size. A saved count can be passed to Rollback
 * to discard only nodes created by a failed composed operation. No memory
 * outside this tape changes and outCount is unchanged on failure. */
UmiStatus UmiAiTrainingTapeCheckpoint(const UmiAiTrainingTape *tape,
                                     size_t *outCount);

/* Discard any suffix of the graph, including operator results. Unlike Rewind,
 * the retained prefix need not consist solely of leaves. Retained nodes keep
 * their identities. Discarded handles become invalid; caller must release
 * them. A successful rollback clears previous gradient-readiness state. */
UmiStatus UmiAiTrainingTapeRollback(UmiAiTrainingTape *tape,
                                   size_t retainCount);

/* Multi-head causal self-attention over [batch*time, channels] matrices.
 * All Q/K/V matrices have the same dimensions, and channels must divide
 * evenly into headCount contiguous heads. Every head is independently
 * scaled by sqrt(channels/headCount) and masked by sequenceLength.
 * The output is [batch*time, channels] in the original head order. The tape
 * retains probabilities for reverse-mode differentiation and guarantees
 * no cross-sequence or future-token access. Failed calls publish no node. */
UmiStatus UmiAiTrainingMultiHeadCausalAttention(
    UmiAiTrainingTape *tape,
    const UmiAiTrainingTensor *queries,
    const UmiAiTrainingTensor *keys,
    const UmiAiTrainingTensor *values,
    size_t sequenceLength,
    size_t headCount,
    UmiAiTrainingTensor **outTensor);

/* Causal attention with one copied binary selection byte per input row.
 * Shapes, head partition and cache limits match MultiHeadCausalAttention.
 * selectedCount must equal batch*time and each byte is 0 or 1. An excluded
 * query produces an all-zero output; excluded keys and values are absent from
 * every softmax and weighted sum. Selected queries always retain their own
 * key, so their softmax is nonempty even with leading padding or interior gaps.
 * An entirely excluded sequence (or batch) is permitted and produces zeros.
 * No future or cross-sequence access is introduced by the mask.
 *
 * Backward contributes zero to excluded Q/K/V rows through this operation.
 * Other graph paths can still contribute gradients. This is an ATTENTION mask,
 * not a loss mask: callers separately select the objective's valid positions.
 * Input tensors must still contain finite values under the usual tape contract.
 * The node owns its copied selection until rollback/rewind/destroy; its bytes
 * count toward the existing persistent graph budget. Temporary score storage
 * retains the existing bound. Refusal preserves graph and outTensor. No decoder,
 * tokenizer or model checkpoint format is changed by this primitive.
 */
UmiStatus UmiAiTrainingMaskedMultiHeadCausalAttention(
    UmiAiTrainingTape *tape, const UmiAiTrainingTensor *queries,
    const UmiAiTrainingTensor *keys, const UmiAiTrainingTensor *values,
    size_t sequenceLength, size_t headCount,
    const unsigned char *selected, size_t selectedCount,
    UmiAiTrainingTensor **outTensor);

/* Per-row layer normalisation with learned affine scale and bias:
 *   y = ((x - mean) / sqrt(variance + epsilon)) * gamma + beta
 * input is [rows, channels]; gamma and beta are each [1, channels].
 * epsilon must be positive and finite. Backpropagation calculates gradients
 * for input, gamma and beta, including the case of shared input operands. */
UmiStatus UmiAiTrainingLayerNorm(UmiAiTrainingTape *tape,
                                const UmiAiTrainingTensor *input,
                                const UmiAiTrainingTensor *gamma,
                                const UmiAiTrainingTensor *beta,
                                float epsilon,
                                UmiAiTrainingTensor **outTensor);

/* Update several trainable leaves as one transaction. Validation covers every
 * parameter and every candidate float BEFORE a single value is committed.
 * Duplicate handles and non-leaves are rejected. An unsuccessfully completed
 * call leaves all parameters unchanged; output buffers are not involved.
 * The caller owns the array of handles; tape owns the parameter buffers.
 * No worker may change the tape concurrently. */
UmiStatus UmiAiTrainingSgdStepMany(UmiAiTrainingTape *tape,
                                   UmiAiTrainingTensor *const *parameters,
                                   size_t parameterCount,
                                   float learningRate);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_TAPE_H */

