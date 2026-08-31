# Reusable View Presentation

Created by: Sammy Hegab  
Organisation: Umicom Foundation  
Licence: MIT

`UmiUiViewPresentation` is a bounded immutable snapshot created from a registered
`UmiUiViewFactoryRegistry` factory. A temporary `UmiUiViewModel` is constructed,
its public snapshot and property bag are copied, and the temporary model is then
destroyed.

This gives frontend adapters a small, ownership-safe presentation boundary:

`Product service -> View factory -> UmiUiViewModel -> UmiUiViewPresentation -> Adapter`

The GTK4 reference adapter now renders these presentations in side/bottom panes.
The headless adapter renders the same properties as deterministic text.

No GTK type is exposed by `include/umicom/ui/view_presentation.h`.
