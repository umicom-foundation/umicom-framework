/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/service.c
 *
 * PURPOSE:
 *   Provide one strategy-research catalogue and derive safe research readiness
 *   from the canonical trading workspace without creating execution authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/service.h"

#include <stdio.h>
#include <string.h>

#include "umicom/strategy_research/backtest/average_loss.h"
#include "umicom/strategy_research/backtest/average_win.h"
#include "umicom/strategy_research/backtest/expectancy.h"
#include "umicom/strategy_research/backtest/exposure.h"
#include "umicom/strategy_research/backtest/gross_loss.h"
#include "umicom/strategy_research/backtest/gross_profit.h"
#include "umicom/strategy_research/backtest/loss_streak.h"
#include "umicom/strategy_research/backtest/max_drawdown.h"
#include "umicom/strategy_research/backtest/net_profit.h"
#include "umicom/strategy_research/backtest/profit_factor.h"
#include "umicom/strategy_research/backtest/recovery_factor.h"
#include "umicom/strategy_research/backtest/trade_count.h"
#include "umicom/strategy_research/backtest/turnover.h"
#include "umicom/strategy_research/backtest/win_rate.h"
#include "umicom/strategy_research/backtest/win_streak.h"
#include "umicom/strategy_research/optimisation/budget.h"
#include "umicom/strategy_research/optimisation/candidate_rank.h"
#include "umicom/strategy_research/optimisation/early_stop.h"
#include "umicom/strategy_research/optimisation/objective.h"
#include "umicom/strategy_research/optimisation/overfit_gap.h"
#include "umicom/strategy_research/optimisation/pareto.h"
#include "umicom/strategy_research/optimisation/robustness.h"
#include "umicom/strategy_research/optimisation/sensitivity.h"
#include "umicom/strategy_research/optimisation/stability.h"
#include "umicom/strategy_research/optimisation/walk_forward.h"
#include "umicom/strategy_research/performance/annualised_return.h"
#include "umicom/strategy_research/performance/annualised_volatility.h"
#include "umicom/strategy_research/performance/calmar.h"
#include "umicom/strategy_research/performance/downside_deviation.h"
#include "umicom/strategy_research/performance/payoff_ratio.h"
#include "umicom/strategy_research/performance/sharpe.h"
#include "umicom/strategy_research/performance/sortino.h"
#include "umicom/strategy_research/performance/stability.h"
#include "umicom/strategy_research/performance/trade_efficiency.h"
#include "umicom/strategy_research/performance/ulcer_index.h"
#include "umicom/strategy_research/replay/checkpoint.h"
#include "umicom/strategy_research/replay/clock_drift.h"
#include "umicom/strategy_research/replay/data_quality.h"
#include "umicom/strategy_research/replay/event_rate.h"
#include "umicom/strategy_research/replay/gap_count.h"
#include "umicom/strategy_research/replay/gap_ratio.h"
#include "umicom/strategy_research/replay/progress.h"
#include "umicom/strategy_research/replay/readiness.h"
#include "umicom/strategy_research/replay/seek_distance.h"
#include "umicom/strategy_research/replay/speed.h"
#include "umicom/strategy_research/simulation/cash_change.h"
#include "umicom/strategy_research/simulation/commission.h"
#include "umicom/strategy_research/simulation/evidence_quality.h"
#include "umicom/strategy_research/simulation/fill_ratio.h"
#include "umicom/strategy_research/simulation/latency.h"
#include "umicom/strategy_research/simulation/notional.h"
#include "umicom/strategy_research/simulation/position_change.h"
#include "umicom/strategy_research/simulation/provenance.h"
#include "umicom/strategy_research/simulation/slippage.h"
#include "umicom/strategy_research/simulation/trade_pnl.h"
#include "umicom/strategy_research/strategy/entry_gate.h"
#include "umicom/strategy_research/strategy/exit_gate.h"
#include "umicom/strategy_research/strategy/position_size.h"
#include "umicom/strategy_research/strategy/risk_reward.h"
#include "umicom/strategy_research/strategy/signal_confidence.h"
#include "umicom/strategy_research/strategy/signal_expiry.h"
#include "umicom/strategy_research/strategy/signal_score.h"
#include "umicom/strategy_research/strategy/stop_distance.h"
#include "umicom/strategy_research/strategy/target_distance.h"
#include "umicom/strategy_research/strategy/version_readiness.h"
#include "umicom/strategy_research/study/atr.h"
#include "umicom/strategy_research/study/bollinger_position.h"
#include "umicom/strategy_research/study/crossover.h"
#include "umicom/strategy_research/study/ema.h"
#include "umicom/strategy_research/study/macd.h"
#include "umicom/strategy_research/study/range.h"
#include "umicom/strategy_research/study/rate_of_change.h"
#include "umicom/strategy_research/study/resistance_distance.h"
#include "umicom/strategy_research/study/rsi.h"
#include "umicom/strategy_research/study/sma.h"
#include "umicom/strategy_research/study/support_distance.h"
#include "umicom/strategy_research/study/trend_strength.h"
#include "umicom/strategy_research/study/volatility.h"
#include "umicom/strategy_research/study/volume_ratio.h"
#include "umicom/strategy_research/study/vwap.h"

typedef struct ResearchCatalogueEntry {
    const char *id;
    UmiStrategyResearchEvaluator evaluator;
} ResearchCatalogueEntry;

static const ResearchCatalogueEntry CATALOGUE[] = {
    {"backtest.average-loss", umi_strategy_research_evaluate_backtest_average_loss},
    {"backtest.average-win", umi_strategy_research_evaluate_backtest_average_win},
    {"backtest.expectancy", umi_strategy_research_evaluate_backtest_expectancy},
    {"backtest.exposure", umi_strategy_research_evaluate_backtest_exposure},
    {"backtest.gross-loss", umi_strategy_research_evaluate_backtest_gross_loss},
    {"backtest.gross-profit", umi_strategy_research_evaluate_backtest_gross_profit},
    {"backtest.loss-streak", umi_strategy_research_evaluate_backtest_loss_streak},
    {"backtest.max-drawdown", umi_strategy_research_evaluate_backtest_max_drawdown},
    {"backtest.net-profit", umi_strategy_research_evaluate_backtest_net_profit},
    {"backtest.profit-factor", umi_strategy_research_evaluate_backtest_profit_factor},
    {"backtest.recovery-factor", umi_strategy_research_evaluate_backtest_recovery_factor},
    {"backtest.trade-count", umi_strategy_research_evaluate_backtest_trade_count},
    {"backtest.turnover", umi_strategy_research_evaluate_backtest_turnover},
    {"backtest.win-rate", umi_strategy_research_evaluate_backtest_win_rate},
    {"backtest.win-streak", umi_strategy_research_evaluate_backtest_win_streak},
    {"optimisation.budget", umi_strategy_research_evaluate_optimisation_budget},
    {"optimisation.candidate-rank", umi_strategy_research_evaluate_optimisation_candidate_rank},
    {"optimisation.early-stop", umi_strategy_research_evaluate_optimisation_early_stop},
    {"optimisation.objective", umi_strategy_research_evaluate_optimisation_objective},
    {"optimisation.overfit-gap", umi_strategy_research_evaluate_optimisation_overfit_gap},
    {"optimisation.pareto", umi_strategy_research_evaluate_optimisation_pareto},
    {"optimisation.robustness", umi_strategy_research_evaluate_optimisation_robustness},
    {"optimisation.sensitivity", umi_strategy_research_evaluate_optimisation_sensitivity},
    {"optimisation.stability", umi_strategy_research_evaluate_optimisation_stability},
    {"optimisation.walk-forward", umi_strategy_research_evaluate_optimisation_walk_forward},
    {"performance.annualised-return", umi_strategy_research_evaluate_performance_annualised_return},
    {"performance.annualised-volatility", umi_strategy_research_evaluate_performance_annualised_volatility},
    {"performance.calmar", umi_strategy_research_evaluate_performance_calmar},
    {"performance.downside-deviation", umi_strategy_research_evaluate_performance_downside_deviation},
    {"performance.payoff-ratio", umi_strategy_research_evaluate_performance_payoff_ratio},
    {"performance.sharpe", umi_strategy_research_evaluate_performance_sharpe},
    {"performance.sortino", umi_strategy_research_evaluate_performance_sortino},
    {"performance.stability", umi_strategy_research_evaluate_performance_stability},
    {"performance.trade-efficiency", umi_strategy_research_evaluate_performance_trade_efficiency},
    {"performance.ulcer-index", umi_strategy_research_evaluate_performance_ulcer_index},
    {"replay.checkpoint", umi_strategy_research_evaluate_replay_checkpoint},
    {"replay.clock-drift", umi_strategy_research_evaluate_replay_clock_drift},
    {"replay.data-quality", umi_strategy_research_evaluate_replay_data_quality},
    {"replay.event-rate", umi_strategy_research_evaluate_replay_event_rate},
    {"replay.gap-count", umi_strategy_research_evaluate_replay_gap_count},
    {"replay.gap-ratio", umi_strategy_research_evaluate_replay_gap_ratio},
    {"replay.progress", umi_strategy_research_evaluate_replay_progress},
    {"replay.readiness", umi_strategy_research_evaluate_replay_readiness},
    {"replay.seek-distance", umi_strategy_research_evaluate_replay_seek_distance},
    {"replay.speed", umi_strategy_research_evaluate_replay_speed},
    {"simulation.cash-change", umi_strategy_research_evaluate_simulation_cash_change},
    {"simulation.commission", umi_strategy_research_evaluate_simulation_commission},
    {"simulation.evidence-quality", umi_strategy_research_evaluate_simulation_evidence_quality},
    {"simulation.fill-ratio", umi_strategy_research_evaluate_simulation_fill_ratio},
    {"simulation.latency", umi_strategy_research_evaluate_simulation_latency},
    {"simulation.notional", umi_strategy_research_evaluate_simulation_notional},
    {"simulation.position-change", umi_strategy_research_evaluate_simulation_position_change},
    {"simulation.provenance", umi_strategy_research_evaluate_simulation_provenance},
    {"simulation.slippage", umi_strategy_research_evaluate_simulation_slippage},
    {"simulation.trade-pnl", umi_strategy_research_evaluate_simulation_trade_pnl},
    {"strategy.entry-gate", umi_strategy_research_evaluate_strategy_entry_gate},
    {"strategy.exit-gate", umi_strategy_research_evaluate_strategy_exit_gate},
    {"strategy.position-size", umi_strategy_research_evaluate_strategy_position_size},
    {"strategy.risk-reward", umi_strategy_research_evaluate_strategy_risk_reward},
    {"strategy.signal-confidence", umi_strategy_research_evaluate_strategy_signal_confidence},
    {"strategy.signal-expiry", umi_strategy_research_evaluate_strategy_signal_expiry},
    {"strategy.signal-score", umi_strategy_research_evaluate_strategy_signal_score},
    {"strategy.stop-distance", umi_strategy_research_evaluate_strategy_stop_distance},
    {"strategy.target-distance", umi_strategy_research_evaluate_strategy_target_distance},
    {"strategy.version-readiness", umi_strategy_research_evaluate_strategy_version_readiness},
    {"study.atr", umi_strategy_research_evaluate_study_atr},
    {"study.bollinger-position", umi_strategy_research_evaluate_study_bollinger_position},
    {"study.crossover", umi_strategy_research_evaluate_study_crossover},
    {"study.ema", umi_strategy_research_evaluate_study_ema},
    {"study.macd", umi_strategy_research_evaluate_study_macd},
    {"study.range", umi_strategy_research_evaluate_study_range},
    {"study.rate-of-change", umi_strategy_research_evaluate_study_rate_of_change},
    {"study.resistance-distance", umi_strategy_research_evaluate_study_resistance_distance},
    {"study.rsi", umi_strategy_research_evaluate_study_rsi},
    {"study.sma", umi_strategy_research_evaluate_study_sma},
    {"study.support-distance", umi_strategy_research_evaluate_study_support_distance},
    {"study.trend-strength", umi_strategy_research_evaluate_study_trend_strength},
    {"study.volatility", umi_strategy_research_evaluate_study_volatility},
    {"study.volume-ratio", umi_strategy_research_evaluate_study_volume_ratio},
    {"study.vwap", umi_strategy_research_evaluate_study_vwap},
};

size_t umi_strategy_research_service_count(void)
{
    return sizeof(CATALOGUE) / sizeof(CATALOGUE[0]);
}

UmiStatus umi_strategy_research_service_id_at(
    size_t index,
    const char **outId)
{
    if (outId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outId = NULL;
    if (index >= umi_strategy_research_service_count()) {
        return UMI_STATUS_NOT_FOUND;
    }
    *outId = CATALOGUE[index].id;
    return UMI_STATUS_OK;
}

UmiStatus umi_strategy_research_service_evaluate(
    const char *id,
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    size_t index;

    if (id == NULL || id[0] == '\0' || input == NULL ||
        outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < umi_strategy_research_service_count(); ++index) {
        if (strcmp(CATALOGUE[index].id, id) == 0) {
            return CATALOGUE[index].evaluator(input, outSnapshot);
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_strategy_research_workspace_snapshot(
    UmiTradingWorkspace *workspace,
    UmiStrategyResearchWorkspaceSnapshot *outSnapshot)
{
    UmiTradingWorkspaceSnapshot trading;
    UmiStatus status;

    if (workspace == NULL || outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&trading, 0, sizeof(trading));
    (void)memset(outSnapshot, 0, sizeof(*outSnapshot));
    status = umi_trading_workspace_snapshot(workspace, &trading);
    if (status != UMI_STATUS_OK) return status;

    outSnapshot->revision = trading.revision;
    (void)snprintf(
        outSnapshot->selectedInstrumentId,
        sizeof(outSnapshot->selectedInstrumentId),
        "%s",
        trading.selected_instrument_id);
    outSnapshot->selectedBarCount = trading.selected_bar_count;
    outSnapshot->retainedTradeCount = trading.trade_tape.retained_count;
    outSnapshot->visibleTradeCount = trading.trade_tape.visible_count;
    outSnapshot->marketDataReady = trading.market_data_ready;
    outSnapshot->riskReady = trading.risk_ready;
    outSnapshot->healthReady = trading.health_ready;

    outSnapshot->strategyReady =
        trading.market_data_ready &&
        trading.risk_ready &&
        trading.health_ready &&
        trading.has_selected_instrument &&
        trading.has_quote &&
        trading.has_bar;

    outSnapshot->replayReady =
        outSnapshot->strategyReady &&
        trading.selected_bar_count > 0U &&
        trading.trade_tape.retained_count > 0U;

    outSnapshot->studiesReady =
        trading.has_selected_instrument &&
        trading.selected_bar_count >= 2U;

    outSnapshot->simulationReady =
        trading.market_data_ready &&
        trading.risk_ready &&
        trading.health_ready;

    outSnapshot->optimisationReady =
        outSnapshot->strategyReady &&
        outSnapshot->studiesReady;

    return UMI_STATUS_OK;
}
