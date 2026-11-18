![Umicom](../../resources/brand/umicom-logo.svg)

# Train selected targets while excluding padded context

The native decoder accepts two independent binary selections. Attention selection marks real context positions. Objective selection marks predictions to score. Use the explicit padded APIs when a batch contains padding; the existing `EvaluateMasked` and `TrainMaskedStep` interfaces remain objective-only.

A selection byte is exactly zero or one. Both arrays contain one byte per input token. Every scored row must also be attended, and at least one row must be scored. A prompt can be attended without being scored. This prevents an answer-only objective from accidentally hiding the prompt.

## Prepare a small batch

The decoder uses fixed-length sequences packed consecutively. With context length three, six input IDs form two independent sequences. Positions remain `index % contextLength`; padding does not compact or renumber them. If your model expects a different position policy, prepare data explicitly rather than assuming that a mask changes positions.

~~~c
const size_t input_ids[6] = {1, 2, 3, 4, 5, 6};
const size_t target_ids[6] = {2, 3, 4, 5, 6, 7};
const unsigned char attention[6] = {1, 0, 1, 0, 1, 1};
const unsigned char objective[6] = {0, 0, 1, 0, 0, 1};
~~~

This deliberately includes interior and leading padding so the policy is visible. All IDs, including placeholders and excluded targets, must be in the vocabulary. The model does not infer padding from an ID value: the same numeric ID can be a real token in one position and a placeholder in another. Callers supply already-shifted targets and matching token counts. No tokenizer or chat-template policy is added by these calls.

## Evaluate before updating weights

1. Create a decoder using its existing configuration API. Keep the configured context length consistent with the arrays.
2. Keep the input IDs, targets and both selection arrays readable and unchanged until the synchronous call returns. Keep output storage separate from them.
3. Call the padded evaluation API and check its status before using the returned loss.
4. Compare only objectives with the same selection and reduction policy. The mean below divides by two selected target rows across the batch, not by six input positions or by the number of sequences.

~~~c
float loss = 0.0F;
UmiStatus status = UmiAiTrainingDecoderEvaluatePadded(
    model, input_ids, target_ids, 6,
    attention, 6, objective, 6, &loss);
if (status != UMI_STATUS_OK) {
    /* The previous loss variable remains unchanged. Inspect the inputs. */
}
~~~

Evaluation leaves model weights and completed-step count unchanged. Temporary tensors are released back to the parameter prefix before return. A memory-budget refusal does not leave a partially built graph attached to the model.

## Commit a small SGD step

~~~c
status = UmiAiTrainingDecoderTrainPaddedStep(
    model, input_ids, target_ids, 6,
    attention, 6, objective, 6, 0.03F, &loss);
~~~

A successful call returns the pre-update loss, commits one multi-parameter SGD update and increments the step count once. SGD means stochastic gradient descent: parameters move opposite their calculated objective derivatives, scaled by the learning rate. The rate must be positive and finite. Framework validates candidate parameter updates before committing any of them. A refusal preserves weights, step count and the caller's loss variable.

Do not compare the returned pre-update loss with an assumed post-update value. Call evaluation again when you need the latter. A smaller objective on this fixed artificial batch demonstrates overfitting to a fixture, not improvement on unseen text.

## Read selected logits

`UmiAiTrainingDecoderLogitsPadded` uses the same attention policy and returns one vocabulary-width row per input position. Capacity is measured in floats. Excluded rows are filled with zeros for predictable output storage. Those zeros are placeholders, not predictions or a uniform distribution to sample. Keep the attention array available so consumers can distinguish valid rows.

~~~c
float logits[6 * 8]; /* Example vocabulary size: eight. */
status = UmiAiTrainingDecoderLogitsPadded(
    model, input_ids, 6, attention, 6, logits, 6 * 8);
~~~

An all-zero attention array is accepted for logits and produces all-zero placeholder output. It cannot form a valid padded training objective: no nonempty objective can be a subset of an empty attention selection. Rejected logits calls leave the entire output buffer unchanged. Requests for a next-token prediction must choose a valid selected row; the existing `PredictNext` API keeps its original unpadded contract.

## Follow the shared implementation

The decoder reuses its embedding lookups and absolute position IDs, then calls `UmiAiTrainingTransformerBlockForwardMasked`. That composition selects the canonical tape attention operator. Layer normalization, projections, residual connections and feed-forward operations remain the existing Framework services. No application-local tensor engine is required.

An excluded transformer row still has a residual/feed-forward value internally. The block itself does not promise zero output rows. The public padded-logits API deliberately replaces those excluded output rows with zeros before publication. Selected outputs do not depend on excluded input rows through the block, because its other operations are row-local and attention excludes the padded keys and values.

All-selected attention preserves the existing calculation order. The original unmasked and objective-only API behavior remains available. Checkpoints still encode the same model parameters and step count; save the tokenizer, mask policy, data order and position convention separately when reproducing a training run.

## Bound memory and qualify a consumer

The existing decoder graph budget accounts for retained tape storage, including each copied attention mask. It does not cover every temporary allocation, stack buffer, allocator overhead or operating-system memory cost. Masked rows are still stored and processed in several operations. Padding selection supplies correct semantics; it is not sparse-memory execution or permission to load an oversized model.

From the Framework repository root, use a focused build directory and stop on errors:

~~~powershell
cmake -S tests/training_decoder_mask -B build/decoder-padding -G Ninja
cmake --build build/decoder-padding --parallel 2
ctest --test-dir build/decoder-padding --output-on-failure
~~~

The suite checks selected-logit reference loss, padding-ID and excluded-target invariance, matching SGD updates, all-selected compatibility, independent sequences, causal isolation, invalid selections, finite rates, graph-budget rollback, repeated steps and checkpoint continuation. It also retains the prior whole-decoder and transformer regressions. Running this focused composition does not qualify a full Framework application, installed SDK, GPU backend or pretrained-model importer.

If configuration reports a missing source or compilation reports an absent API, reconcile matching headers and implementations. If a mask is rejected, check every byte and both counts, then ensure every objective byte of one has an attention byte of one. If a budget is exceeded, inspect the committed and peak graph reports and your batch geometry; do not disable checks to hide retained storage growth.

<!-- Historical optimizer scope, retained for review. Explicit selected AdamW APIs now extend this interface.
The padded APIs currently cover logits, evaluation and one-batch SGD. Existing AdamW and accumulated-gradient entry points retain their earlier contracts. No automatic dispatch converts an unpadded call into a padded one. Add future optimizer or application integration through these shared services, with explicit policy and regression coverage.
-->

The original padded APIs cover logits, evaluation and one-batch SGD. [Selected AdamW training](selected-adamw-training.md) adds explicit objective-only and padded AdamW steps through the same shared policies. Accumulated-gradient entry points retain their earlier unpadded contracts. No automatic dispatch converts an unpadded call into a padded one.

## Prepare a byte-vocabulary prompt and completion

[Native byte-pair preparation](byte-pair-preparation.md) provides an explicit
BOS/prompt/completion/EOS teaching format, already-shifted targets, right
padding and separate attention/loss selections. Reuse it with the matching
260-entry vocabulary instead of repeating token-shifting code in applications.
It does not replace a pretrained model’s tokenizer or chat template.
