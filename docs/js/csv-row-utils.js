(function (global) {
  'use strict';

  function formatFinite(value, digits, fallback) {
    const number = Number(value);
    return isFinite(number) ? number.toFixed(digits) : (fallback || '');
  }

  function seriesHeader(series) {
    return series.key + '_' + series.unit.replaceAll('/', '_per_');
  }

  function buildLiveSeriesCsvRows(rows, series, rowValueForSeries) {
    return [
      ['time_s', ...series.map(seriesHeader)],
      ...rows.map(row => [
        formatFinite(row.time, 3, '0.000'),
        ...series.map(item => formatFinite(rowValueForSeries(row, item), 5, ''))
      ])
    ];
  }

  function buildHookeTrialCsvRows(trials) {
    return [
      ['trial', 'extension_cm', 'extension_m', 'force_mean_N', 'force_max_N', 'force_stddev_N', 'duration_s', 'k_estimate_N_per_m'],
      ...trials.map((trial, index) => [
        String(index + 1),
        formatFinite(trial.extensionCm, 3),
        formatFinite(trial.extensionM, 5),
        formatFinite(trial.forceMean, 6),
        formatFinite(trial.forceMax, 6),
        formatFinite(trial.stdDev, 6),
        formatFinite(trial.duration, 6),
        formatFinite(trial.kEstimate, 6)
      ])
    ];
  }

  function buildWeightTrialCsvRows(trials) {
    return [
      ['trial', 'mass_g', 'mass_kg', 'extension_cm', 'extension_m', 'measured_weight_N', 'theoretical_weight_N', 'experimental_g_m_per_s2', 'difference_N', 'error_percent'],
      ...trials.map((trial, index) => [
        String(index + 1),
        formatFinite(trial.massG, 3),
        formatFinite(trial.massKg, 6),
        formatFinite(trial.extensionCm, 3),
        formatFinite(trial.extensionM, 5),
        formatFinite(trial.measuredWeight, 6),
        formatFinite(trial.theoreticalWeight, 6),
        formatFinite(trial.experimentalG, 6),
        formatFinite(trial.difference, 6),
        formatFinite(trial.errorPercent, 3)
      ])
    ];
  }

  function buildBuoyancyTrialCsvRows(trials, fallbackResult) {
    return [
      ['trial', 'volume_ml', 'volume_m3', 'weight_air_N', 'weight_liquid_N', 'experimental_buoyancy_N', 'theoretical_buoyancy_water_N', 'difference_N', 'error_percent', 'loss_percent', 'density_kg_per_m3'],
      ...trials.map((trial, index) => {
        const fallback = typeof fallbackResult === 'function'
          ? fallbackResult(trial.weightAir, trial.weightLiquid, trial.volumeMl)
          : null;
        const theoreticalBuoyancy = isFinite(trial.theoreticalBuoyancy)
          ? trial.theoreticalBuoyancy
          : (fallback ? fallback.theoreticalBuoyancy : NaN);
        const difference = isFinite(trial.difference)
          ? trial.difference
          : (fallback ? fallback.difference : NaN);
        const errorPercent = isFinite(trial.errorPercent)
          ? trial.errorPercent
          : (fallback ? fallback.errorPercent : NaN);
        return [
          String(index + 1),
          formatFinite(trial.volumeMl, 3),
          isFinite(trial.volumeM3) ? trial.volumeM3.toExponential(6) : '',
          formatFinite(trial.weightAir, 6),
          formatFinite(trial.weightLiquid, 6),
          formatFinite(trial.buoyantForce, 6),
          formatFinite(theoreticalBuoyancy, 6),
          formatFinite(difference, 6),
          formatFinite(errorPercent, 3),
          formatFinite(trial.lossPercent, 3),
          formatFinite(trial.density, 3)
        ];
      })
    ];
  }

  function buildCollisionTrialCsvRows(trials, massKg) {
    return [
      ['trial', 'v0_m_per_s', 'impulse_Ns', 'abs_impulse_Ns', 'peak_force_N', 'mean_force_N', 'duration_s', 'mass_kg'],
      ...trials.map((trial, index) => [
        String(index + 1),
        formatFinite(trial.velocity, 5),
        formatFinite(trial.impulse, 6),
        formatFinite(Math.abs(trial.impulse), 6),
        formatFinite(trial.peakForce, 6),
        formatFinite(trial.meanForce, 6),
        formatFinite(trial.duration, 6),
        formatFinite(massKg, 5)
      ])
    ];
  }

  function buildPendulumTrialCsvRows(trials, defaultMassG) {
    return [
      ['trial', 'length_m', 'mass_g', 'angle_deg', 'duration_s', 'delta_t_s', 'oscillations', 'measured_period_s', 'theoretical_period_s', 'half_period_s', 'quarter_period_s', 'ten_periods_s'],
      ...trials.map((trial, index) => {
        const period = Number(trial.period);
        return [
          String(index + 1),
          formatFinite(trial.lengthM, 4),
          formatFinite(trial.massG || defaultMassG, 3),
          formatFinite(trial.angleDeg, 2),
          formatFinite(trial.durationS, 2),
          formatFinite(trial.deltaT || 0, 4),
          String(trial.oscillations || 0),
          formatFinite(trial.measuredPeriod || period, 6),
          formatFinite(period, 6),
          formatFinite(period / 2, 6),
          formatFinite(period / 4, 6),
          formatFinite(period * 10, 6)
        ];
      })
    ];
  }

  global.PalladioCsvRowUtils = {
    buildLiveSeriesCsvRows,
    buildHookeTrialCsvRows,
    buildWeightTrialCsvRows,
    buildBuoyancyTrialCsvRows,
    buildCollisionTrialCsvRows,
    buildPendulumTrialCsvRows
  };
})(typeof window !== 'undefined' ? window : globalThis);
