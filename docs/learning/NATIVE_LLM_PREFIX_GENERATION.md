# Umicom Native LLM — Opt-in Growing-Prefix Generation

**Development Batch 73–74 | 23 October 2026**  
**Author:** Sammy Hegab · Umicom Foundation  
**Implementation:** Native C23 in Umicom Framework · MIT licence

## Why this capability exists

The original bounded educational decoder uses a fixed context and learned
absolute positional embeddings. Its established generator left-pads short
prompts with BOS until the context is full, then samples from the final row.
This is a stable, existing behavioural contract; it must not be silently
changed just to make a KV cache look faster.

Batch 69–70 qualified K/V reuse for *successive growing prefixes*; Batch
71–72 compared a full prefix and its first rollover against the independent
canonical decoder. This batch adds a deliberately **opt-in autoregressive
session** whose prompt begins at position zero. No artificial BOS prefix is
injected. While there is still room in the context, one previously sampled
pending token is appended each time another output token is requested. After
the context fills, learned absolute positions require the existing KV cache
to rebuild the full window on every append. The mathematical owner is the
original `UmiAiTrainingKvCache`, not a new inference kernel.

**This mode is not token-compatible with the old BOS-left-padded generator**
for short prompts. They implement different position policies. Do not use
output from one policy as a regression oracle for the other. Neither mode
changes model weights or saved decoder checkpoint bytes.

## New Framework API

Public header: `include/umicom/ai/training/prefix_generation.h`

- `UmiAiTrainingPrefixGenerationWorkEstimateBuild`: predict canonical
  full-context and actual cache token-row work using the existing KV heap
  allocation planner; rejection leaves the report unchanged.
- `UmiAiTrainingPrefixGenerationSessionCreate`: prefill an explicit,
  nonempty prefix of 1..contextLength tokens at learned positions 0..P-1.
- `UmiAiTrainingPrefixGenerationSessionNext`: sample one token by reusing
  the current logits and the Framework nucleus sampler. The preceding sample
  is appended only when the next pull occurs.
- `UmiAiTrainingPrefixGenerationSessionNextByte`: return a raw byte for a
  260-token decoder; never assume UTF-8 or NUL termination.
- `UmiAiTrainingPrefixGenerationSessionInspect`: report generated-token count
  and actual cache rows, rebuilds and retained tokens.
- `UmiAiTrainingPrefixGenerationSessionDestroy`: release all session-owned
  memory; this is the cancellation mechanism.
- `UmiAiTrainingDecoderGeneratePrefixNucleus`: one-shot adapter that stages
  the entire output, verifies predicted versus observed cache work, then
  publishes tokens, count and optional statistics atomically.

Old public generation APIs and checkpoints are untouched. The same model
must not be trained, destroyed or used concurrently with a session. A
session is stale after successful model training; destroy and recreate it.

## Exact work formula

Let **C** be decoder context length, **P** the initial nonempty prompt length,
and **G** the requested generated-token count. In this educational contract,
`1 <= P <= C` and `1 <= G <= 256`. The final generated token stays pending,
so exactly **A = G - 1** appends are performed for G samples.

- Stable-position append count: `S = min(A, C - P)`
- Evict-and-rebuild append count: `R = A - S`
- Canonical full-context rows: `C * G`
- Actual incremental rows: `P + S + (C * R)`
- Full-window rebuilds: `R`
- Rows avoided: `(C * G) - (P + S + C * R)`
- Retained tokens after G samples: `min(C, P + G - 1)`

Example: **C = 8, P = 3, G = 12**. A = 11, S = 5, R = 6. Canonical decoder
work is 96 token rows. Cached session work is 3 + 5 + 6×8 = **56 rows**.
The session avoids **40 rows (41.7%)** and performs **six full-window
rebuilds**. This saving is a count of computed decoder token-rows, not a
measured CPU speedup. For **P = C = 8, G = 12**, cache and canonical counts
are both 96 rows, because every append rolls the full learned-position
window. The previously measured old BOS-left-padded generator is unchanged.

## Independent canonical comparison

The original `UmiAiTrainingDecoderLogits` accepts exactly C input positions.
For a prompt shorter than C, build a temporary context as the actual retained
prefix followed by harmless BOS *right padding* to length C. Read the logits
at prefix position **P−1**, **not** from the final padded position C−1.
Causal attention prevents later padding positions from affecting the logits
for earlier real tokens. Use the same Framework sampler and seed to produce
canonical reference tokens. After a reference sample, extend the prefix if
space remains or evict its oldest token if full.

These tests compare (a) streaming prefix output, (b) the independent canonical
right-padded decoder reference, and (c) the one-shot prefix output. The
existing positional-qualification tests still compare *all vocabulary logits*
for a complete prefix and first rollover. The tests must fail rather than
publish an incorrect performance report if observed rows or output tokens
are inconsistent with the relevant contract.

## Build on the developer's Windows machine

Open PowerShell, with `C:\umicom\Umicom-Applications` checked out and the
Batch 69–74 Framework packages merged in order:

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path

cmake -S '.\framework\learning\native-llm\prefix-work' `
  -B '.\build\windows-ucrt64-native-llm-prefix-debug' `
  -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
if ($LASTEXITCODE -ne 0) { throw 'Native prefix configure failed.' }

cmake --build '.\build\windows-ucrt64-native-llm-prefix-debug' `
  --target umicom-native-llm-prefix-generation `
           umicom-ai-training-prefix-generation-parity-test `
           umicom-ai-training-prefix-generation-rejections-test --parallel 2
if ($LASTEXITCODE -ne 0) { throw 'Native prefix build failed.' }

ctest --test-dir '.\build\windows-ucrt64-native-llm-prefix-debug' `
  -R '^framework\.ai\.training\.prefix_generation_' `
  --output-on-failure --no-tests=error
if ($LASTEXITCODE -ne 0) { throw 'Native prefix tests failed.' }
```

Once the focused tests pass, build and test the entire prefix laboratory, then
run the executable. Perform the consolidated Umicom Applications build and
checks before committing Framework and the parent repository.

## Acceptance requirements and limitations

1. All new code is native C23 and owned by Umicom Framework; no Python,
   PowerShell runtime helper, alternative AI model or new tensor implementation.
2. Complete existing code, authorship, licences and public ABI remain intact.
3. The original BOS-left-padding mode and old checkpoints remain unchanged.
4. Prefix output must match independent right-padded canonical token selection
   for reviewed configurations and seeds, including the first rollover.
5. Real cache counters must agree exactly with the estimate; no timing-based
   claim of improved throughput is made without real CPU and wall-clock data.
6. Rejected calls do not mutate caller-owned outputs. Session cancellation
   releases the cache and RNG; stale model sessions are not silently reused.
7. Full GCC/Clang production-decoder and Windows UCRT64 validation are still
   required on the developer machine before promotion.

Next: measure larger workloads and consider **explicit, checkpoint-versioned
positional-encoding evolution**. Do not replace learned absolute positions
silently or reuse old K/V vectors after their tokens shift position.
