(function (global) {
  'use strict';

  function linearFit(points, minDenominator) {
    if (!Array.isArray(points) || points.length < 2) return null;
    const meanX = points.reduce((sum, point) => sum + point.x, 0) / points.length;
    const meanY = points.reduce((sum, point) => sum + point.y, 0) / points.length;
    let numerator = 0;
    let denominator = 0;
    points.forEach(point => {
      const dx = point.x - meanX;
      const dy = point.y - meanY;
      numerator += dx * dy;
      denominator += dx * dx;
    });
    if (denominator < (minDenominator || 1e-12)) return null;
    const slope = numerator / denominator;
    const intercept = meanY - slope * meanX;
    const ssTot = points.reduce((sum, point) => sum + (point.y - meanY) * (point.y - meanY), 0);
    const ssRes = points.reduce((sum, point) => {
      const predicted = intercept + slope * point.x;
      return sum + (point.y - predicted) * (point.y - predicted);
    }, 0);
    const r2 = ssTot < 1e-12 ? 1 : 1 - ssRes / ssTot;
    return { slope, intercept, r2 };
  }

  function hookeFit(trials) {
    if (!Array.isArray(trials) || trials.length < 2) return null;
    const points = trials
      .map(trial => ({ x: trial.extensionM, y: trial.forceMean }))
      .filter(point => point.x > 0);
    return linearFit(points, 1e-12);
  }

  function collisionFit(trials) {
    if (!Array.isArray(trials) || trials.length < 2) return null;
    const points = trials.map(trial => ({ x: trial.velocity, y: Math.abs(trial.impulse) }));
    const fit = linearFit(points, 1e-9);
    return fit ? { slope: fit.slope, intercept: fit.intercept } : null;
  }

  function solveLinearSystem(matrix, vector) {
    const size = vector.length;
    const augmented = matrix.map((row, index) => [...row, vector[index]]);
    for (let column = 0; column < size; column++) {
      let pivot = column;
      for (let row = column + 1; row < size; row++) {
        if (Math.abs(augmented[row][column]) > Math.abs(augmented[pivot][column])) pivot = row;
      }
      if (Math.abs(augmented[pivot][column]) < 1e-12) return null;
      [augmented[column], augmented[pivot]] = [augmented[pivot], augmented[column]];
      const divisor = augmented[column][column];
      for (let item = column; item <= size; item++) augmented[column][item] /= divisor;
      for (let row = 0; row < size; row++) {
        if (row === column) continue;
        const factor = augmented[row][column];
        for (let item = column; item <= size; item++) {
          augmented[row][item] -= factor * augmented[column][item];
        }
      }
    }
    return augmented.map(row => row[size]);
  }

  function polynomialFit(points, degree) {
    if (!Array.isArray(points) || points.length < degree + 1) return null;
    const origin = points.reduce((sum, point) => sum + point.x, 0) / points.length;
    const shifted = points.map(point => ({ x: point.x - origin, y: point.y }));
    const size = degree + 1;
    const matrix = Array.from({ length: size }, () => Array(size).fill(0));
    const vector = Array(size).fill(0);

    shifted.forEach(point => {
      for (let row = 0; row < size; row++) {
        vector[row] += point.y * Math.pow(point.x, row);
        for (let column = 0; column < size; column++) {
          matrix[row][column] += Math.pow(point.x, row + column);
        }
      }
    });

    const coefficients = solveLinearSystem(matrix, vector);
    if (!coefficients) return null;
    const predict = time => {
      const shiftedTime = time - origin;
      return coefficients.reduce((sum, coefficient, power) => {
        return sum + coefficient * Math.pow(shiftedTime, power);
      }, 0);
    };
    const mean = points.reduce((sum, point) => sum + point.y, 0) / points.length;
    const total = points.reduce((sum, point) => sum + Math.pow(point.y - mean, 2), 0);
    const residual = points.reduce((sum, point) => sum + Math.pow(point.y - predict(point.x), 2), 0);
    const r2 = total < 1e-12 ? (residual < 1e-12 ? 1 : 0) : 1 - residual / total;
    const predictors = degree;
    const adjustedR2 = points.length > predictors + 1
      ? 1 - (1 - r2) * (points.length - 1) / (points.length - predictors - 1)
      : -Infinity;
    return {
      degree,
      coefficients,
      origin,
      predict,
      r2,
      adjustedR2,
      rmse: Math.sqrt(residual / points.length)
    };
  }

  function chooseFit(points, fitType) {
    if (fitType === 'none') return null;
    const linear = polynomialFit(points, 1);
    if (fitType === 'linear') return linear;
    const quadratic = polynomialFit(points, 2);
    if (fitType === 'quadratic') return quadratic;
    if (!quadratic) return linear;
    if (!linear) return quadratic;
    return quadratic.adjustedR2 > linear.adjustedR2 + 0.002 ? quadratic : linear;
  }

  global.PalladioFitUtils = {
    linearFit,
    hookeFit,
    collisionFit,
    solveLinearSystem,
    polynomialFit,
    chooseFit
  };
})(window);
