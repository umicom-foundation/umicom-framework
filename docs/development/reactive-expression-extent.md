![Umicom](../../resources/brand/umicom-logo.svg)

# Distinguish expression storage from executable instructions

The reactive expression-program record contains two fixed arrays and a count. Its storage validator checks that the count fits both arrays. This lets a consumer establish a bounded extent before reading entries.

A record can satisfy this check without being an executable program. This type currently exposes no public opcode vocabulary, stack-effect rules or execution API. Do not infer those capabilities from its name.

## 1. Initialize before filling the record

~~~c
UmiUiReactiveExpressionProgram record;
umi_ui_reactive_expression_program_init(&record);
/* An empty initialized record has a valid storage extent. */
if (!umi_ui_reactive_expression_program_valid(&record))
    return UMI_STATUS_INVALID_STATE;
~~~

Zero count is valid. It describes an empty record. The arrays have space for UMI_UI_REACTIVE_MAX_TOKENS entries. The validator derives the bound from each actual array so a future layout change cannot silently make one field shorter than the accepted extent.

## 2. Reject a declared count before indexing

If a caller supplies a count above either capacity, validation returns false. It does not scan count entries first. It also leaves the complete record unchanged.

A count is metadata supplied by the producer. The array's allocated extent comes from its type. Trusting the metadata without comparing those two facts can lead a later consumer beyond its storage.

Callers still provide a complete readable object and serialize access during validation and consumption. An extent check cannot validate an arbitrary pointer or protect against concurrent modification.

## 3. Require a separate execution contract

An opcode is a number whose meaning is defined by an instruction set. The storage record alone does not define that meaning. Likewise, a number field does not establish a valid operand, a stack depth or a permitted command.

Before adding execution, define the accepted instructions, operands, stack effects, resource limits and failure publication policy in the responsible Framework service. Reuse existing language services when they match the requirement. Keep application clients thin.

The regression fixture deliberately places an arbitrary opcode value in a record with an accepted count. The extent validator accepts the storage without claiming the opcode is meaningful. Do not add a fabricated interpreter to make this fixture look executable.

## 4. Qualify the boundary

Run commands separately and stop on errors:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug --target umicom-ui-reactive-expression-program-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure -R "^framework[.]ui_reactive[.]expression[.]program$"
~~~

The existing target now checks null input and six count boundaries, including full capacity, capacity plus one and SIZE_MAX. It also checks that validation does not alter the record.

These cases are authored and statically reviewed, not executed qualification. A passing extent check would establish this storage contract only. It would not prove expression compilation, execution, native binding or a complete application workflow.

Continue with [reactive value admission](reactive-value-admission.md) to see how payload validation and live owner publication form additional boundaries.
