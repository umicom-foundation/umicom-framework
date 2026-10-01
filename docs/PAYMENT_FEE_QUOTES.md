# Calculate and review a payment fee

A quote answers a calculation question: given these assumptions, what would
the fee and total debit be? It does not send a payment, reserve funds or create
a journal entry. The quote service has no database or payment-provider handle.

## Understand the inputs

An amount has a currency, an integer coefficient and a scale. GBP with scale 2
and coefficient 12345 represents GBP 123.45. The scale is explicit; the service
does not look up how many decimal places a currency normally uses.

One basis point is 0.01 percent. A rate of 25 basis points means 0.25 percent.
The supported rate range is 0 through 10000, inclusive. The same range applies
to the illustrative tax rate. These are supplied assumptions, not current
provider prices or a determination that tax applies to a payment.

The calculation follows these steps:

1. Multiply principal by the variable fee rate and round to a whole minor unit.
2. Add the fixed fee, stopping at the maximum fee. A maximum of zero means
   zero fee; it does not mean unlimited. Maximum must be at least fixed fee.
3. Apply the supplied tax rate to that capped fee, then round separately.
4. Add fee and tax to principal. Reject the quote if either sum is too large
   for a signed 64-bit integer.

The cap applies before tax. Total charges can therefore exceed the fee cap.
Principal must be positive. The low-level fee calculator also accepts zero
principal, which can still incur its fixed fee unless capped to zero.

## Choose rounding deliberately

| Policy | 2.5 minor units | -2.5 minor units |
| --- | ---: | ---: |
| Toward zero | 2 | -2 |
| Nearest, ties away from zero | 3 | -3 |
| Nearest, ties to even | 2 | -2 |
| Away from zero | 3 | -3 |

Half-even sends 3.5 to 4 because 4 is even. The shared rate API supports signed
amounts, including the smallest signed 64-bit integer. Payment quotes accept
positive principal and nonnegative fees and tax only. Arithmetic uses integer
quotients and remainders rather than floating-point prices or compiler-specific
128-bit types. Rates above 100 percent are rejected.

## Capture a quote in C23

1. Include `umicom/finance/payments/payment_quote.h` and use `Umicom::finance`.
2. Fill a `UmiPaymentQuoteRequest` with bounded quote/payment IDs, the principal,
   a rule created with `umi_payments_payment_fee_rule_init`, and both rounding
   choices. All rule amounts use the principal's currency and scale.
3. Call `UmiPaymentQuoteCreate`. Check its status before reading its output.
4. Use `UmiPaymentQuoteRead` for a copied snapshot, `UmiPaymentQuoteDescribe` for
   readable text, or `UmiPaymentQuoteExportCsv` for an owned CSV document.
5. Destroy the CSV document and quote when finished. They own their contents
   independently; changing or discarding the original request cannot change them.

For example, principal 12345 at scale 2, fixed fee 10, variable rate 25, cap
1000, and illustrative tax rate 2000 produce the following with half-even
rounding: variable fee 31, total fee 41, tax 8, total charges 49, and total
debit 12394. These values represent 123.45, 0.41, 0.08, 0.49 and 123.94 in
the chosen currency. Bank's `src/console/payment_quote_example.c` is a complete
example with status handling and ownership cleanup.

CSV includes the IDs, currency, scale, exact integer inputs and results, both
rounding policies and a scenario label. All cells are quoted. Formula-like
text IDs get the shared CSV exporter's leading apostrophe; numeric cells remain
typed numbers. A spreadsheet's import settings still determine whether very
large integers are preserved exactly. Invalid UTF-8/control text can cause CSV
export to fail even when its bounded financial identifier was accepted.

## Handle a rejected calculation

`INVALID_ARGUMENT` means an input failed the contract: for example, an empty
ID, negative fee, cap below fixed fee, unsupported rounding mode or rate above
10000. `CAPACITY_EXCEEDED` means a total would overflow. Correct the inputs and
create a new quote; never treat a failed call as a zero-cost result.

Checked fee and charge functions leave their scalar output unchanged on
failure. Initialisers now also retain the old destination on failure. The
legacy scalar convenience functions remain available and return zero on invalid
input; new callers should use status-returning functions to distinguish an
error from an actual zero fee. The old arithmetic remains in explained disabled
source blocks for review. No public function was removed.

## Use the native calculator

`UmiGtk4PaymentQuoteCreate` returns a floating GTK widget for a host to parent.
It has editable assumptions, a Calculate quote action, and a CSV copy action.
Edits disable Copy until a successful calculation captures the new inputs.
Failed calculation retains the previous displayed result with an error message.
The host calls `UmiGtk4PaymentQuoteDetach` at closure to make retained controls
inert. Object-bound signal closures also protect buttons that outlive the root.

Quotes are in-memory scenarios. They do not establish identity, approval,
available funds, provider fees, exchange rates, tax liability or an executable
payment instruction. Those workflows must use their own authoritative services.
