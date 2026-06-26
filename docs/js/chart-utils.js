(function (global) {
  'use strict';

  function selectedTimeScale(viewSpan, timeScaleMode) {
    if (timeScaleMode === 's') return { unit: 's', multiplier: 1 };
    if (timeScaleMode === 'ms') return { unit: 'ms', multiplier: 1e-3 };
    if (timeScaleMode === 'us') return { unit: 'μs', multiplier: 1e-6 };
    if (viewSpan < 0.001) return { unit: 'μs', multiplier: 1e-6 };
    if (viewSpan < 1) return { unit: 'ms', multiplier: 1e-3 };
    return { unit: 's', multiplier: 1 };
  }

  function selectedValueScale(selectedSeries, scale, valueScaleMode) {
    const units = [...new Set(selectedSeries.map(series => series.unit))];
    const maxAbs = Math.max(Math.abs(scale.min), Math.abs(scale.max), 1e-12);
    const mapping = {
      micro: { prefix: 'μ', multiplier: 1e-6 },
      milli: { prefix: 'm', multiplier: 1e-3 },
      base: { prefix: '', multiplier: 1 },
      kilo: { prefix: 'k', multiplier: 1e3 }
    };
    if (units.length !== 1) return { prefix: '', multiplier: 1 };
    if (valueScaleMode !== 'auto') return mapping[valueScaleMode] || mapping.base;
    if (maxAbs >= 1000) return mapping.kilo;
    if (maxAbs < 0.001) return mapping.micro;
    if (maxAbs < 1) return mapping.milli;
    return mapping.base;
  }

  function displayUnit(unit, scaleInfo) {
    return (scaleInfo.prefix || '') + unit;
  }

  function formatValue(value, span) {
    const magnitude = Math.abs(value);
    if (magnitude >= 100 || span >= 100) return value.toFixed(0);
    if (magnitude >= 10 || span >= 10) return value.toFixed(1);
    return value.toFixed(2);
  }

  global.PalladioChartUtils = {
    selectedTimeScale,
    selectedValueScale,
    displayUnit,
    formatValue
  };
})(window);
