# Umicom Native AI — reviewed training experiments (Batch 07)

**Author:** Sammy Hegab · **Organisation:** Umicom Foundation · **Licence:** MIT

The authoritative implementation remains in Umicom Framework, written in C23. This guide describes a thin native training consumer. The original source trainer, decoder trainer, CMake targets, mathematical operators, checkpoints and corpus code are preserved unchanged.

## Framework-owned contracts

| Component | Public header | Native source |
| --- | --- | --- |
| Deterministic experiment execution and sampled evaluation | `include/umicom/ai/training/experiment.h` | `src/ai/training/experiment.c` |
| Reviewed, versioned experiment-plan parsing | `include/umicom/ai/training/experiment_plan.h` | `src/ai/training/experiment_plan.c` |
| Source file manifest and immutable dataset | `include/umicom/ai/training/dataset_manifest.h` | Existing `src/ai/training/dataset_manifest.c` |
| Decoder and training | `include/umicom/ai/training/decoder.h` | Existing `src/ai/training/decoder.c` |
| Model checkpoints | `include/umicom/ai/training/checkpoint.h` | Existing `src/ai/training/checkpoint.c` |

## Plan file

A sample lives in `fixtures/experiment.conf` and selects `fixtures/manifest.txt` *relative to the plan*, not the terminal's current directory. Key/value parsing is strict and bounded. No shell, SQL, environment substitutions, absolute manifest paths, path traversals or dynamically loaded code are evaluated.

You can copy the plan alongside a reviewed manifest and change `steps` (1..512), `eval_interval`, `eval_batches`, learning rate and seed values. The 260-token vocabulary and limited C23 tensor tape restrict some combinations. `batch_sequences=2` with context 8 and vocabulary 260 is intentionally rejected because 2 × 8 × 260 = 4,160 exceeds the tape's 4,096-element bound.

## Build and run on Windows

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S ".\framework\learning\native-llm" `
    -B ".\build\windows-ucrt64-native-llm-debug" -G Ninja `
    -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build ".\build\windows-ucrt64-native-llm-debug" --parallel 2
ctest --test-dir ".\build\windows-ucrt64-native-llm-debug" `
    --output-on-failure --no-tests=error
& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-experiment.exe" `
    ".\framework\learning\native-llm\fixtures\experiment.conf"
```

Only provide the next argument if you want to write a **new** checkpoint. A third argument supplies an **existing input checkpoint** for resume; it is read-only and never changed:

```text
umicom-native-llm-experiment <plan-file> [NEW-output-checkpoint] [EXISTING-resume-checkpoint]
```

Resuming is reproducible only when the verified dataset, initialisation and training settings (including the learning rate and train seed) are identical. The decoder checkpoint format does not contain source-file fingerprints or full experiment metadata; manual verification of provenance is necessary. All checksum values are accidental-corruption indicators, not authenticity proofs.

## Tests and interpretation

Three new CTest cases verify the parser, run determinism and cancellation. The deterministic run test compares checkpoint **bytes** for uninterrupted training and a 2+2-step resumed run; it also checks that the original checkpoint model is unchanged. The full isolated suite contains 36 tests in this batch.

The CLI shows: source-file fingerprints, sampled baseline held-out cross-entropy, an observation at each configured interval, sampled perplexity `exp(cross_entropy)` and a final summary. It cannot prove that the model understands source code. These fixed few evaluation windows are *not* an entire held-out corpus or an independent coding benchmark. Changing the evaluation seed changes the sample and may change the result.

## Safety and non-goals

The educational parser limits a plan file to 4,096 bytes. The existing corpus component retains the 1 MiB per-document limit, and source-manifest datasets retain the original 16-file limit. The CLI does not execute imported C sources or run a compiler as part of training. The manifest grammar is **not** a filesystem sandbox: users must review local paths and symlinks and must not ingest secrets, private records or unauthorised source material.

This batch does not introduce AdamW, neural Mixture-of-Experts, large-corpus streaming, GPU acceleration, a full-corpus evaluator, or a production coding expert. A future native experiment receipt can bind dataset fingerprints and training settings to checkpoint provenance before automated resume is allowed.
