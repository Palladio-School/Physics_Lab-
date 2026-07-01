(function (global) {
  'use strict';

  function sonarRoundTripTimeMs(depthM, soundSpeedWater) {
    return (2 * depthM / soundSpeedWater) * 1000;
  }

  function firstFiniteNumber(source, keys) {
    for (const key of keys) {
      const value = Number(source && source[key]);
      if (Number.isFinite(value)) return value;
    }
    return NaN;
  }

  function sonarEchoTimeMsFromSample(sample, soundSpeedAir) {
    const echoMs = firstFiniteNumber(sample, ['sonarEchoMs', 'echoMs', 'echoTimeMs', 'roundTripMs', 'durationMs']);
    if (Number.isFinite(echoMs)) return Math.max(0, echoMs);
    const echoUs = firstFiniteNumber(sample, ['sonarEchoUs', 'echoUs', 'durationUs', 'roundTripUs']);
    if (Number.isFinite(echoUs)) return Math.max(0, echoUs / 1000);
    const distanceM = firstFiniteNumber(sample, ['distance', 'distanceM']);
    if (Number.isFinite(distanceM)) return sonarRoundTripTimeMs(Math.max(0, distanceM), soundSpeedAir);
    const distanceCm = firstFiniteNumber(sample, ['distanceCm']);
    if (Number.isFinite(distanceCm)) return sonarRoundTripTimeMs(Math.max(0, distanceCm / 100), soundSpeedAir);
    return NaN;
  }

  global.PalladioSonarUtils = {
    sonarRoundTripTimeMs,
    firstFiniteNumber,
    sonarEchoTimeMsFromSample
  };
})(window);
