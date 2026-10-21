# Auditing a trained native decoder

**Sammy Hegab · Umicom Foundation · MIT**  
**Canonical implementation:** Umicom Framework, ISO C23

## What problem does this solve?

A next-token model can produce apparently convincing text while having very
poor held-out accuracy. A checkpoint may also be evaluated with the wrong
source files or training configuration. Therefore a model's output must not be
reported without identifying the specific model, the reviewed data and the
experimental settings used to measure it.

`model_audit.h` and `model_audit.c` add an evidence-gathering service to the
existing C23 training library. It reuses the **existing** checkpoint codec,
source manifest, decoder logits and held-out evaluation. No separate trainer,
model implementation, scripting engine or dataset reader is introduced.

## Evidence recorded

The returned `UmiAiTrainingModelAudit` contains:

| Field | What it measures |
| --- | --- |
| `modelFingerprint` | Exact bytes of the Framework's canonical in-memory checkpoint encoding, including parameters and completed steps. |
| `datasetFingerprint` | Ordered source names, train/evaluation assignments, sizes and immutable source-byte fingerprints. |
| `planFingerprint` | Architecture, optimiser settings, evaluation settings and all declared random seeds. |
| `combinedFingerprint` | A deterministic identity of those three fingerprints and completed steps. |
| `heldOutCrossEntropy` | Mean next-token cross-entropy across deterministic held-out windows using `UmiAiTrainingExperimentEvaluate`. |
| `heldOutPerplexity` | `exp(heldOutCrossEntropy)` on that same fixed sample. |
| `topOneCorrect` | Number of target tokens ranked first by the model's logits. |
| `topFiveCorrect` | Number of target tokens ranked among the five highest logits. |
| `evaluatedTokens` | Total number of evaluated next-token targets. |

The accuracy counts use the **same held-out windows** as the canonical
cross-entropy service. When two candidate tokens have identical logits, the
lower token ID ranks first. The denominator includes all sampled target
positions, not just the final token of each context window. Repeated samples
are possible in the bounded, seeded evaluation regime.

Model parameters and optimiser steps are fingerprinted **before and after**
measurement. If their canonical checkpoint bytes change during evaluation,
`UmiAiTrainingModelAuditEvaluate` refuses to publish a result. All errors
preserve the caller's existing report.

## Security boundary

All four identifiers use FNV-1a64 with explicit byte ordering. These values
are useful for detecting accidental source, configuration or checkpoint
changes. **FNV is not collision-resistant and is not an authentication or
integrity-control mechanism against an attacker.** Do not accept checkpoints
or datasets merely because their fingerprints match an untrusted record.
The existing checkpoint trailer is likewise not an authentication signature.
Approved storage, access control, signed provenance and rights-cleared source
review are separate engineering requirements.

An already loaded dataset is an immutable memory snapshot. To detect changes
made to the source files on disk, load the manifest again before comparing
fingerprints. The loader's path grammar is not a filesystem sandbox: symlinks
and external directory permissions require independent controls.

## Build and run on Windows

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"

cmake -S ".\framework\learning\native-llm" `
    -B ".\build\windows-ucrt64-native-llm-debug" `
    -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build ".\build\windows-ucrt64-native-llm-debug" --parallel 2
ctest --test-dir ".\build\windows-ucrt64-native-llm-debug" `
    --output-on-failure --no-tests=error
```

Train using an explicitly reviewed plan and save to a **new** checkpoint
filename. Create the output directory first if necessary; do not overwrite
previous learning results.

```powershell
New-Item -ItemType Directory -Force "C:\umicom\_local\models" | Out-Null

& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-experiment.exe" `
    ".\framework\learning\native-llm\fixtures\experiment.conf" `
    "C:\umicom\_local\models\source-decoder-example.umi"
```

Audit that exact checkpoint using the same plan and manifest:

```powershell
& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-audit.exe" `
    ".\framework\learning\native-llm\fixtures\experiment.conf" `
    "C:\umicom\_local\models\source-decoder-example.umi"
```

The tool prints a 16-digit hexadecimal combined fingerprint. Supply that
fingerprint as a third argument on a later read-only audit to check whether
model, reviewed source snapshots and plan still match. Mismatches produce a
nonzero exit status. The expected fingerprint must originate from a trusted
record; this comparison alone is not a security approval.

```powershell
& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-audit.exe" `
    ".\framework\learning\native-llm\fixtures\experiment.conf" `
    "C:\umicom\_local\models\source-decoder-example.umi" `
    "<previously-reviewed-16-hex-digit-fingerprint>"
```

Replace the angle-bracket placeholder with the actual reported hexadecimal
value. The native audit tool reads checkpoint and source bytes but never
writes files, executes generated code or requests a remote model provider.

## Read the source and repeat an experiment

- `include/umicom/ai/training/model_audit.h` describes lifetime and evidence contracts.
- `src/ai/training/model_audit.c` implements platform-independent identifiers and next-token ranking.
- `learning/native-llm/audit_model.c` is a thin console consumer.
- `tests/ai_training/test_training_model_audit.c` independently verifies accuracy bounds, held-out metric agreement, deterministic results and unchanged weights.
- `tests/ai_training/test_training_audit_changes.c` rejects changed plan settings, different evaluation partitions and changed model weights.
- `tests/ai_training/test_training_audit_rejections.c` verifies invalid-input handling and failure-atomic caller outputs.

The report does **not** mean that a model understands C23, Calypso or Umicom
Framework. The illustrated native decoder uses one small transformer block
and a limited source corpus. A properly evaluated coding expert requires
larger rights-cleared datasets, strong domain benchmarks, meaningful held-out
examples, calibration checks, and verification against compiler/tests/source.

The present decoder still recomputes its rolling context during generation.
This work improves *evaluation evidence* rather than claiming a new KV cache,
faster token generation, cryptographic provenance or complete model governance.
