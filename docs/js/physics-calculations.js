(function (global) {
  function weightResult(forceValue, massG) {
    if (!isFinite(forceValue) || massG <= 0) return null;
    const measuredWeight = Math.abs(forceValue);
    const massKg = massG / 1000;
    const theoreticalWeight = massKg * 9.81;
    const experimentalG = measuredWeight / massKg;
    const difference = measuredWeight - theoreticalWeight;
    const errorPercent = theoreticalWeight > 0 ? Math.abs(difference / theoreticalWeight) * 100 : 0;
    return { massG, massKg, measuredWeight, theoreticalWeight, experimentalG, difference, errorPercent };
  }

  function buoyancyResult(airForce, liquidForce, volumeMl) {
    if (!isFinite(airForce) || !isFinite(liquidForce) || volumeMl <= 0) return null;
    const weightAir = Math.abs(airForce);
    const weightLiquid = Math.abs(liquidForce);
    const buoyantForce = weightAir - weightLiquid;
    const volumeM3 = volumeMl / 1000000;
    const waterDensity = 1000;
    const theoreticalBuoyancy = waterDensity * 9.81 * volumeM3;
    const difference = buoyantForce - theoreticalBuoyancy;
    const errorPercent = theoreticalBuoyancy > 0 ? Math.abs(difference / theoreticalBuoyancy) * 100 : 0;
    const density = buoyantForce > 0 ? buoyantForce / (9.81 * volumeM3) : 0;
    const lossPercent = weightAir > 0 ? (buoyantForce / weightAir) * 100 : 0;
    return {
      weightAir,
      weightLiquid,
      buoyantForce,
      volumeM3,
      theoreticalBuoyancy,
      difference,
      errorPercent,
      density,
      lossPercent
    };
  }

  global.PalladioPhysicsCalculations = {
    weightResult,
    buoyancyResult
  };
})(window);
