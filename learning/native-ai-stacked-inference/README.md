# Native multi-layer inference and evaluation

**Umicom Framework · Umicom Foundation · Sammy Hegab · MIT**

This optional C23 laboratory uses the existing `Umicom::AiStackedDecoder` model,
checkpoint encoder, tokeniser, seeded nucleus sampler and training tape. It
provides a full-context streaming interface, atomic one-shot token and byte
output, read-only held-out evaluation and reproducibility evidence. It does
not replace any pre-existing decoder or modify the stacked model checkpoint.

The session uses a complete BOS-left-padded context for short prompts. After
every sampled token, it shifts the context and recomputes all Transformer
layers. **There is no multi-layer KV cache or inference acceleration claim.**
Its reported computation counts are logical layer-token rows, not measured
floating-point operations, memory bandwidth, wall-clock time or throughput.
The 64-output-token limit belongs to this bounded teaching implementation.

For the byte tokenizer, the output range excludes IDs 0–3 (PAD/BOS/EOS/UNK).
Consequently there is no automatic end-of-sequence stop. Outputs may contain
arbitrary bytes and must not be treated as C strings or executable code.

`UmiAiTrainingStackedEvaluationRun` reports selected-token loss using the
**existing** training-tape cross-entropy, computes selected-target top-one
accuracy from the authoritative logits and produces an ordered-case SHA-256.
The comparison service evaluates two identically shaped models against the
same held-out windows; its numeric accept/reject result is not a deployment
or security authorisation.

Hashes detect changes to input bytes or model checkpoints. They do **not**
establish publisher authenticity, intellectual-property rights, redaction,
source approval or freedom from training/evaluation contamination. All
source-governance checks and any required signed release approval belong to
existing Framework security and dataset services. No filenames, dataset text
or model outputs are written by these APIs.

## Standalone CMake integration

Configure the `learning/native-ai-stacked-inference` directory with a C23
compiler and `BUILD_TESTING=ON`, build the generated targets, then run CTest
with the label `stacked` or the name prefix `framework.ai.stacked_inference.`.
The demonstration runs on synthetic token IDs and prints only aggregate
measurements. A passing CTest is a bounded numerical/lifetime regression;
it does not qualify a production financial AI application.
