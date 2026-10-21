# Native Effective-Batch Experiments

**Author:** Sammy Hegab · **Umicom Foundation** · **Licence:** MIT  
**Implementation:** Umicom Framework C23 training services

## Purpose

The native experiment coordinator can combine multiple bounded decoder
microbatches into one reproducible optimiser step. No application-local neural
mathematics, Python trainer, secondary optimiser or checkpoint format is
introduced. The existing experiment APIs and their configuration layout remain
unchanged; this capability uses explicit opt-in entry points.

The authoritative components are:

| Responsibility | Existing or new Framework contract |
| --- | --- |
| Dataset snapshots and reproducible sampling | `dataset_manifest.h` |
| Reusable transformer decoder and single-batch training | `decoder.h` |
| Mean-gradient reduction across microbatches | `gradient_accumulation.h` |
| Reproducible experiment lifecycle | `experiment.h` |
| Opt-in experiment options | `experiment_accumulation.h` |
| Reviewed plan parsing and location-independent manifests | `experiment_plan.h` |
| Checkpoint persistence | `checkpoint.h` |

## Reviewed plan

The reference fixture is `fixtures/accumulated-experiment.conf`. Its source
manifest remains relative to the plan file rather than the working directory.
The two new keys are `effective_sequences` and `microbatch_sequences`.
Both are required for accumulation; omitting both selects the established
experiment path. The `batch_sequences` field still controls **held-out
sampling**, independently of effective training-batch size.

Training draws from the approved TRAIN partition. Larger effective batches are
assembled from multiple bounded dataset sampler calls. The selection index
includes the absolute optimiser step and the draw ordinal, so the same plan,
seed, data and initial checkpoint reproduce the same source windows.

Training calls the existing `UmiAiTrainingDecoderTrainAccumulated` once per
optimiser step. Gradients are combined with token-count weighting; temporary
forward/backward graphs are rewound after each microbatch. The optimiser
performs one failure-atomic multi-parameter update for the full effective
batch. Held-out evaluation uses the original sampled evaluation service.

## Resource bounds and warnings

- The experiment layer bounds the effective batch to 64 complete contexts.
- The underlying source sampler handles four sequences per call; the
  coordinator joins multiple deterministic draws as necessary.
- The chosen microbatch must fit the actual decoder/tensor capacity planner.
- The checkpoint format does not bind the training data, learning rate or
  accumulation options. Verify those before approving a resume.
- Floating-point reduction is deterministic with fixed iteration order, but
  numerical equivalence to a differently ordered large batch is approximate.
- These are educational byte-decoder models; measured next-byte loss is not
  evidence of C23 correctness or production suitability.

## PowerShell build and tests

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"

cmake -S ".\framework\learning\native-llm" `
    -B ".\build\windows-ucrt64-native-llm-debug" `
    -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON

cmake --build ".\build\windows-ucrt64-native-llm-debug" --parallel 2
ctest --test-dir ".\build\windows-ucrt64-native-llm-debug" `
    --output-on-failure --no-tests=error

& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-experiment.exe" `
    ".\framework\learning\native-llm\fixtures\accumulated-experiment.conf"
```

The executable writes no checkpoint unless explicitly supplied a new
checkpoint destination. Keep experiment snapshots in an approved local
workspace such as `C:\umicom\_local\`, not in the tracked source tree.

## Test coverage

The associated native tests check strict parsing, rejected shapes, deterministic
repeated runs, exact checkpoint continuation, observer cancellation and
failure-atomic model/result publication. The legacy experiment tests remain
registered and unchanged.
