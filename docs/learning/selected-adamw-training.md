![Umicom](../../resources/brand/umicom-logo.svg)

# Keep AdamW state consistent with selected training targets

The native decoder provides two explicit AdamW extensions in `umicom/ai/training/optimizer.h`. `UmiAiTrainingDecoderTrainAdamWMaskedStep` selects targets for the objective while retaining every input as causal context. `UmiAiTrainingDecoderTrainAdamWPaddedStep` additionally excludes padded input positions from attention. Neither function changes the existing unpadded call automatically.

## Prepare the data and optimizer

1. Create the decoder with its existing configuration API. Token arrays contain whole sequences of the configured context length. Inputs and already-shifted targets must all be valid vocabulary IDs, even at excluded positions.
2. Create AdamW with `UmiAiTrainingDecoderAdamWCreate` before any weight update. It owns moment arrays and borrows this decoder's parameter handles. Destroy the optimizer before the decoder.
3. Provide one binary byte per token for each selection. Every scored row must also be attended. At least one row must be scored. Prompt rows may be attended without contributing directly to the objective.
4. Keep the arrays readable and unchanged until the synchronous call returns. Serialize access to each model and optimizer. Never share one optimizer between independently created models.

~~~c
const size_t input[6] = {1, 2, 3, 4, 5, 6};
const size_t target[6] = {2, 3, 4, 5, 6, 7};
const unsigned char attention[6] = {1, 0, 1, 0, 1, 1};
const unsigned char objective[6] = {0, 0, 1, 0, 0, 1};
float loss = 0.0F;
UmiStatus status = UmiAiTrainingDecoderTrainAdamWPaddedStep(
    model, optimizer, input, target, 6,
    attention, 6, objective, 6, 0.02F, &loss);
~~~

Here context length is three, so six positions represent two independent sequences. The mean objective divides by two scored rows. Padding does not renumber absolute positions. The positive, finite learning rate scales the update; it does not change which rows contribute.

## Understand the transaction

Framework validates the model/optimizer binding, step counters, IDs, lengths and selection policy before constructing the graph. The shared transformer and masked cross-entropy produce the same objective used by padded evaluation. Backward propagation computes gradients. Framework then rewinds temporary graph nodes before asking the existing AdamW service to commit.

A successful call updates model weights, first and second moments, and both step counters once. It publishes the pre-update loss. Rejected inputs, graph-budget failures and rejected weight updates leave those values and the caller's loss variable unchanged. Gradients retained on leaf tensors are scratch state, not part of that rollback promise.

A token mask does not freeze parameters. An unscored prompt can influence an answer through attention. A parameter with zero current gradient can also change because AdamW retains previous moments and applies decoupled weight decay. Use a separate, explicitly supported parameter-freezing policy when that is required.

## Resume deliberately

A model-only checkpoint stores weights but not AdamW moments. Use the combined `UmiAiTrainingAdamWCheckpointEncode` / `Decode` APIs when preserving optimizer state. Decoding returns two independently owned handles. Keep the original data order, attention/objective masks, learning-rate schedule and step policy separately: the checkpoint does not store the training dataset or its permissions.

Exact same-build continuation in the small native laboratory does not establish bitwise portability between compilers, hardware or future math implementations. Validate the complete environment before making such a claim.

## Build the focused checks

From the Framework repository root, with CMake, Ninja and a C23 toolchain available:

~~~powershell
cmake -S tests/training_decoder_mask -B build/decoder-selection -G Ninja
cmake --build build/decoder-selection --parallel 2
ctest --test-dir build/decoder-selection --output-on-failure
~~~

Stop on a failing command. The tests compare complete weights-and-moments checkpoints, check selected loss against evaluation, change excluded IDs, reject malformed masks and mismatched optimizers, refuse early and late graph budgets, repeat updates, and resume combined checkpoints.

The decoder graph budget covers its committed tape storage. AdamW moment arrays, checkpoint buffers, temporary optimizer storage and allocator overhead require additional memory. These APIs use dense tensors; padding masks do not make storage sparse. <!-- Earlier scope retained: Masked microbatch accumulation, clipped masked updates, application UI integration, installed SDK qualification and pretrained-model adaptation are separate work. -->
Selected microbatch accumulation is described below. Clipped selected updates, application UI integration, installed SDK qualification and pretrained-model adaptation remain separate work.

If a step fails, retain its status before retrying. Check lengths, binary selection bytes and optimizer ownership first. If an optimizer is stale because weights were updated through SGD, recreating an empty optimizer is a new training history, not exact continuation. Restore a consistent combined checkpoint or start a deliberately new experiment.

## Accumulate selected objectives across smaller graphs

Include `umicom/ai/training/gradient_accumulation.h` to use
`UmiAiTrainingDecoderTrainAdamWMaskedAccumulated` or
`UmiAiTrainingDecoderTrainAdamWPaddedAccumulated`. The latter takes both attention
and objective selections. Supply complete sequences and a positive maximum
number of sequences per microbatch. The final group can contain fewer sequences.

The reducer weights each microbatch mean loss and mean gradient by that group's
selected-target count. It divides by the selected count over the entire effective
batch, then commits exactly one AdamW update. An individual group with no scored
target needs no graph; the whole batch must still contain at least one target.
All IDs and masks are validated even in skipped groups. No sequence is compacted
or renumbered, and no update occurs between microbatches.

Graph storage is rewound after each used group. The reducer additionally allocates
one double and one float per parameter element, plus one float scratch array
for the largest parameter. These arrays are outside the tape budget and tape
memory report. AdamW and allocator overhead remain additional costs. Smaller
graphs can reduce activation peaks without reducing the model's parameter count.

Preserve the microbatch size with data ordering, masks and learning-rate policy
when resuming combined checkpoints. Different partitions can change floating-point
operation order. The selected accumulated calls currently do not clip gradients.

The focused test directory above also exercises selected accumulation: independent
weighted means, complete-graph comparison, legacy parity, empty groups, malformed
data, bound optimizer ownership, graph-budget refusal, final-update failure,
counter exhaustion, repeated graph release and checkpoint continuation.

## Plan reducer payloads before allocating a model

`UmiAiTrainingAccumulationPlanCompute`, declared in
`umicom/ai/training/capacity_plan.h`, accepts a decoder configuration, an
effective sequence count and a requested microbatch sequence count. It performs
no model allocation or training. The nested `graph` member is the existing
capacity report for the largest actual group. Separate members report double
accumulators, reduced float gradients, largest-parameter scratch and their
combined logical payload.

For five sequences and a requested group of two, the maximum number of graphs
is three and the final group contains one sequence. A requested group larger
than the effective batch is allowed within the accumulated-token limit; its
actual graph contains only the effective batch. Objective masks may skip empty
groups, so `maximumGraphs` is an upper bound rather than an observed execution
count. Rejected requests preserve the caller's report.

This does not include every activation, optimizer temporary, moment allocation,
input array or allocator cost. It is neither a safe total-memory budget nor a
physical memory reservation. Preserve the model geometry and exact source
revision when comparing a planned payload with live observations.
