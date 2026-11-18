![Umicom](../../resources/brand/umicom-logo.svg)

# Prepare a native byte-pair training example

Use `UmiAiTrainingBytePairPrepare` from
`umicom/ai/training/byte_pair_sequence.h` when teaching completion-only
training with the native 260-entry byte vocabulary. The implementation belongs
to the existing `Umicom::AiTraining` target. It allocates no heap memory and
performs no file access, network request or model update.

This is an explicit educational format. It is not a downloaded model's
chat template, tokenizer or fine-tuning adapter.

## Follow one record

For prompt `Q:` and completion `A`, the full stream is:

~~~text
BOS Q : A EOS
  1 85 62 69 2
~~~

The byte tokenizer maps each input byte to byte + 4. IDs 0 through 3 are
reserved. No separator or text normalization is added. Include desired
newlines or labels in the prompt itself. Explicit byte lengths include embedded
NUL bytes; this interface does not validate UTF-8.

~~~c
const uint8_t prompt[] = {'Q', ':'};
const uint8_t completion[] = {'A'};
UmiAiTrainingBytePairSequence sample;
UmiStatus status = UmiAiTrainingBytePairPrepare(
    prompt, sizeof prompt, completion, sizeof completion, 8U, &sample);
~~~

On success, `context_length` is 8, `valid_rows` is 4 and
`selected_rows` is 2. The owned arrays are:

| Row | Input | Target | Attention | Selected loss |
| --- | --- | --- | --- | --- |
| 0 | BOS (1) | Q (85) | 1 | 0 |
| 1 | Q (85) | colon (62) | 1 | 0 |
| 2 | colon (62) | A (69) | 1 | 1 |
| 3 | A (69) | EOS (2) | 1 | 1 |
| 4 through 7 | PAD (0) | PAD (0) | 0 | 0 |

Targets are already shifted once. EOS is a selected target, not an additional
input row. Prompt positions remain causal context even when their own targets
are not scored. Their embeddings can receive gradients through later selected
predictions. See [padded decoder training](padded-decoder-training.md) for the
separate attention and objective contracts.

## Enforce the bounds

1. Choose context length 1 through `UMI_AI_DECODER_MAX_CONTEXT` (currently 32).
2. Supply a nonempty completion. An empty prompt is allowed and may use NULL.
3. Require prompt bytes + completion bytes + 1 <= context length. The extra
   prediction row supplies EOS. An overlong pair is refused, never truncated.
4. Keep input spans valid and stable for the call, disjoint from the output.
   The two read-only source spans may overlap each other.
5. Check the status before using the result. All refusals leave output
   unchanged. Invalid arguments return INVALID_ARGUMENT; a pair that cannot
   fit returns CAPACITY_EXCEEDED.

All unused storage slots have PAD IDs and zero masks, including slots beyond
the chosen context. Only context_length rows belong to the example. The result
retains no pointers into the original text. Keep it alive during a synchronous
decoder call and use a matching vocabulary/context configuration.

## Qualify preparation separately from learning

From the Framework source root, run each command separately and stop on errors:

~~~powershell
$env:Path = "C:/msys64/ucrt64/bin;$env:Path"
cmake -S tests/training_byte_pair -B build/training-byte-pair -G Ninja -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe
cmake --build build/training-byte-pair --parallel 2
ctest --test-dir build/training-byte-pair --output-on-failure
~~~

The cases cover golden IDs, empty prompts, binary bytes, padding, exact limits,
refusals, input lifetime, an independent sequence oracle and all 256 byte
values. They do not train a model.

Bits2Banking's `labs/framework/masked-language-loss/byte_pair.c` composes this
service with the production padded decoder. It repeats one sample for forty
SGD steps, checks lower finite selected loss and checks retained tape storage
after graph cleanup. That narrow experiment demonstrates alignment and updates;
it does not establish held-out quality, a useful assistant or total process
memory bounds. The paired lesson “Prepare a byte pair without shifting twice”
explains JSONL, token IDs, vectors, masks and the complete learning commands.
