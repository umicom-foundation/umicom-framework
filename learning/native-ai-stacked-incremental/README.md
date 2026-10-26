# Native stacked incremental inference — C23 laboratory

This opt-in Umicom Framework laboratory exercises a separately qualified
multi-layer, learned-absolute-position Transformer inference policy. The
original decoder, checkpoint codec, full-context generation, and training
tape remain authoritative and unchanged.

`UmiAiTrainingStackedKvStepPropose` calculates only the new token's projections
while reading earlier actual K/V rows from `UmiAiTrainingStackedKvStore`.
Every neural primitive is an established Framework tape operation. The
historical `UmiAiTrainingMultiHeadCausalAttention` calculates **all query rows**
in the temporary K/V matrix, even though only the final row is consumed.
Consequently fewer dense projection rows **do not** establish better wall-clock
performance. Learned-position rollover remains a canonical full-context rebuild.

`UmiAiTrainingStackedKvIncrementalSession` composes full-context prefill,
incremental prefix growth, canonical rollover fallback, and the original
Framework nucleus sampler. The deterministic regression compares the emitted
token IDs with `UmiAiTrainingStackedPrefixGenerateNucleus`; the separate
logit-level regression compares every vocabulary logit against canonical
`UmiAiTrainingStackedDecoderLogits` at each eligible prefix length.

## Build and qualify

Configure with CMake and Ninja from `learning/native-ai-stacked-incremental`,
build all targets, and run CTest with
`-R '^framework\.ai\.stacked_incremental\.'`. Test results must be recorded
against the **real** Framework training tape; isolated interface stubs cannot
establish numerical parity. Repeat in Release and sanitiser-enabled builds.

## Scope

- This is not a general-purpose language model, a GPU backend, or a fused
  single-query attention kernel.
- The checkpoint must remain immutable and applications must serialise access
  to each model and its inference session.
- A reduction in projected rows is not a timing measurement; report actual
  native profiling separately before making optimisation claims.
- Content rights and training-source approval remain separately governed by
  the established Framework admission services.
