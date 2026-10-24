# Native LLM — Checkpoint-bound inference admission

**Umicom Framework / Umicom Foundation**  
**Author:** Sammy Hegab  
**Language:** C23  
**Scope:** Native training laboratory, Batches 79–80  
**Licence:** MIT

## 1. Engineering objective

Batch 77–78 introduced a portable **128-byte position sidecar** recording the
canonical decoder checkpoint's SHA-256, learned-absolute positional strategy,
rollover rebuild policy and selected generation context. Merely declaring those
fields does not prevent an application from accidentally bypassing the record
when it loads a decoder or starts token generation. Batch 79–80 enforces that
compatibility boundary in three real execution paths.

1. **Checkpoint admission:** compare exact original checkpoint bytes against
   the separate sidecar *before* invoking the established checkpoint decoder;
   verify the resulting decoder's identity *after* loading.
2. **Token generation admission:** check checkpoint identity and context before
   generation, delegate all inference and sampling to an existing generator,
   and verify identity again before publishing output.
3. **Benchmark admission:** permit paired prefix benchmarks only for their
   explicitly documented growing-unpadded context; maintain the independent
   decoder/benchmark parity and checkpoint checks from Batch 75–76.

The existing standalone decoder, checkpoint format, learned absolute positions,
training kernels, BOS-left-padded generator, KV cache and unpadded-prefix
inference pathway remain unchanged.

This is **compatibility admission, not a new positional-encoding model or a
measured inference speedup**. Caching after a rolling context still requires
full-window rebuilds for the current learned-absolute-position decoder.

## 2. Placement and dependency ownership

```text
Umicom Applications
└── framework
    ├── include/umicom/ai/training/position_admission.h  (public C ABI)
    ├── src/ai/training/position_admission.c          (orchestration)
    ├── cmake/UmicomNativeLlmPositionAdmission.cmake  (opt-in composition)
    ├── tests/ai_training/test_training_position_admission_*.c
    └── learning/native-llm/prefix-work/inspect_position_admission.c
```

The new service is **Framework-owned**: Studio, Trader, Umicom LLM and future
consumers may call its APIs without copying identity, loader, generator or
benchmark algorithms. It is activated only by the opt-in prefix laboratory
CMake project; the much larger consolidated application build is unaffected
until that integration is reviewed and explicitly enabled.

## 3. Public C23 operations

### 3.1 `UmiAiTrainingPositionBoundCheckpointDecode`

```c
UmiStatus UmiAiTrainingPositionBoundCheckpointDecode(
    const uint8_t *checkpointBytes, size_t checkpointLength,
    const uint8_t *sidecarBytes, size_t sidecarLength,
    UmiAiTrainingPositionContext requestedContext,
    UmiAiTrainingDecoder **outModel);
```

**Order of operations:**

1. Refuse null pointers, a zero or oversized checkpoint, and a sidecar whose
   byte count is not precisely `UMI_AI_POSITION_CONTRACT_RECORD_BYTES`.
2. Decode and validate the sidecar using the existing `PositionContractDecode`
   service. That service checks its magic, version, supported decoder shapes,
   positional-encoding identifier, permitted context mode and sidecar digest.
3. Require the requested context to match the sidecar's declared context.
4. Require the checkpoint size to match the exact length declared by the
   sidecar, then hash the **original bytes** with `UmiSha256Init`,
   `UmiSha256Update` and `UmiSha256Final`.
5. Only after all earlier checks pass, call the existing decoder checkpoint
   codec. No incompatible checkpoint is parsed into a model first.
6. Verify the decoded model against the sidecar with
   `UmiAiTrainingPositionContractVerify`; this re-encodes the canonical model
   checkpoint and checks the effective identity and policy.
7. On success publish one caller-owned decoder; otherwise destroy any staged
   decoder and leave `*outModel` unchanged.

**Threat boundary:** a sidecar is unsigned. An attacker who controls both
checkpoint and sidecar can rehash both, so a matching pair is **not** proof of
publisher identity or model safety. Validate authorised source, signatures,
model-release approvals and local path policy *outside* this method. The method
works on caller-supplied bytes and never reads a filename.

### 3.2 `UmiAiTrainingPositionBoundGenerate`

```c
UmiStatus UmiAiTrainingPositionBoundGenerate(
    UmiAiTrainingDecoder *model,
    const UmiAiTrainingPositionContract *approvedContract,
    UmiAiTrainingPositionContext requestedContext,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy,
    size_t generatedTokenCount, uint64_t seed,
    size_t *outTokenIds, size_t capacity, size_t *outCount,
    UmiAiTrainingPositionGenerationEvidence *outEvidence);
```

The requested context deliberately selects one of two **different** inference
policies:

| Requested context | Existing inference owner | Work counters |
|---|---|---|
| `BOS_LEFT_PADDED` | `UmiAiTrainingDecoderGenerateNucleus` | Unavailable (reported as such) |
| `GROWING_UNPADDED` | `UmiAiTrainingDecoderGeneratePrefixNucleus` | Real KV-cache work from Framework |

The original generator's empty-prompt allowance remains intact. The
unpadded-prefix generator still requires a non-empty prompt no longer than
the configured context. The existing maximum of 256 generated tokens remains.

Generation stages token IDs and evidence on the stack. It checks the model's
approved checkpoint identity **before and after** the complete operation, then
publishes tokens, token count and evidence together on success. A failure
cannot publish a partial token sequence or a misleading result. Outputs must
not overlap one another, and access to the borrowed model must be serialised.

### 3.3 `UmiAiTrainingPositionBoundBenchmark`

```c
UmiStatus UmiAiTrainingPositionBoundBenchmark(
    UmiAiTrainingDecoder *model,
    const UmiAiTrainingPositionContract *approvedContract,
    UmiAiTrainingPositionContext requestedContext,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy,
    const UmiAiTrainingPrefixBenchmarkPlan *plan,
    UmiAiTrainingPrefixBenchmarkResult *outResult);
```

Only `GROWING_UNPADDED` is accepted. The paired benchmark evaluates a
right-BOS-padded **canonical** reference at the last real prefix row. Applying
that benchmark to the historical BOS-left-padded generator would silently
compare two different models of token position; the new gate refuses this.

All warming, alternating execution order, token-by-token parity, CPU/wall
measurement, token-row counters and checkpoint immutability stay inside
`UmiAiTrainingPrefixBenchmarkRun`. This gate only requires an approved model
before and after the run and checks that the established benchmark reported
an unchanged checkpoint.

## 4. Walkthrough: loading and generating under a declared policy

```c
#include "umicom/ai/training/position_admission.h"

/* Preconditions: checkpoint and sidecar were obtained from an APPROVED source,
 * and their byte buffers remain owned by the caller. */
UmiAiTrainingDecoder *model = NULL;
UmiStatus status = UmiAiTrainingPositionBoundCheckpointDecode(
    checkpointBytes, checkpointLength,
    sidecarBytes, sidecarLength,
    UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED, &model);
if (status != UMI_STATUS_OK) {
    /* Refuse the model; do not silently select another generation policy. */
    return status;
}

UmiAiTrainingPositionContract declaration = {0};
status = UmiAiTrainingPositionContractDecode(
    sidecarBytes, sidecarLength, &declaration);
if (status == UMI_STATUS_OK) {
    size_t tokenIds[6] = {0};
    size_t produced = 0U;
    UmiAiTrainingPositionGenerationEvidence evidence = {0};
    const UmiAiTrainingNucleusOptions policy = {1.0F, 1U, 1.0F};
    const size_t prompt[] = {109U, 114U, 120U}; /* byte IDs for int */
    status = UmiAiTrainingPositionBoundGenerate(
        model, &declaration,
        UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED,
        prompt, 3U, &policy, 6U, UINT64_C(79),
        tokenIds, 6U, &produced, &evidence);
    /* tokenIds and evidence are meaningful ONLY if status == UMI_STATUS_OK. */
}
UmiAiTrainingDecoderDestroy(model);
return status;
```

The service does not run generated output as source code. For byte-token
convenience, callers may use the already available Framework reversible byte
tokeniser. The raw IDs 0–3 are reserved; decoding must be explicit.

## 5. Why position-independent projections alone do not solve rollover

The existing educational decoder uses learned absolute positional embeddings
and a single pre-LN transformer block. A retained token changes position
whenever the oldest token is evicted; therefore its old cached key/value
projections are no longer mathematically valid.

A future model using relative or rotary positional semantics could avoid
**some** rebasing, but this does not automatically make old retained K/V rows
valid for a finite sliding attention window. In a multi-layer transformer,
retained hidden states may still depend on evicted earlier tokens. Attention
mask semantics and layer-history dependencies must be specified and tested.
An alternative architecture requires its own checkpoint family, training,
model metadata, independent reference logits and numerical-parity evidence.

**Never relabel existing absolute-position weights as rotary-position weights.**

## 6. Focused validation and limitations

The package adds three test source files and one executable smoke test:

- `position_admission_roundtrip`: approved model, sidecar, bound checkpoint
  load, both generation modes, direct API token parity and benchmark call.
- `position_admission_rejections`: truncated/modified checkpoint, malformed
  sidecar, policy mismatch, invalid prompt, insufficient capacity, unchanged
  caller outputs and unmodified decoder pointer after errors.
- `position_admission_identity`: two shape-identical decoders with different
  seeds/checkpoints; reject a foreign declaration and unsupported strategy.
- `position_admission_demo`: thin native C23 consumer exercises the round trip.

Isolated GCC/Clang Release and ASan/UBSan checks can validate these C23
orchestration contracts with API-compatible decoder substitutes and true
SHA-256. That **does not prove** correct linkage or numerical behaviour of
the full native production decoder. Use the complete Windows UCRT64 prefix
lab to validate with the real Framework code. Keep any substitute/harness
implementation outside the public delivery and all production repositories.

## 7. Integration commands (interactive PowerShell)

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path

cmake -S '.\framework\learning\native-llm\prefix-work' `
  -B '.\build\windows-ucrt64-native-llm-prefix-debug' `
  -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
if ($LASTEXITCODE -ne 0) { throw 'Native LLM configure failed.' }

cmake --build '.\build\windows-ucrt64-native-llm-prefix-debug' `
  --target umicom-native-llm-position-admission `
           umicom-ai-training-position-admission-roundtrip-test `
           umicom-ai-training-position-admission-rejections-test `
           umicom-ai-training-position-admission-identity-test --parallel 2
if ($LASTEXITCODE -ne 0) { throw 'Native LLM compile failed.' }

ctest --test-dir '.\build\windows-ucrt64-native-llm-prefix-debug' `
  -R '^framework\.ai\.training\.position_admission_' `
  --output-on-failure --no-tests=error
if ($LASTEXITCODE -ne 0) { throw 'Position admission CTest failed.' }
```

This document records architectural and validation contracts, not a claim
that the entire Windows build or future positional model is complete.
