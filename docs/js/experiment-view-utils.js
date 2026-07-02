(function (global) {
  'use strict';

  function isCollisionExperimentMode(state) {
    return state.activeMode === 0 && state.accelDisplayMode === 'force';
  }

  function isHookeExperimentMode(state) {
    return state.activeMode === 2 && state.activeForceExperiment === 'hooke';
  }

  function isWeightExperimentMode(state) {
    return state.activeMode === 2 && state.activeForceExperiment === 'weight';
  }

  function isBuoyancyExperimentMode(state) {
    return state.activeMode === 2 && state.activeForceExperiment === 'buoyancy';
  }

  function isForceExperimentMode(state) {
    return state.activeMode === 2;
  }

  function isMotionExperimentMode(state) {
    return state.activeMode === 4 && ['a-motion', 'a-speed', 'b-uniform-motion'].includes(state.activeExperimentId);
  }

  function isSonarExperimentMode(state) {
    return state.activeMode === 4 && state.activeExperimentId === 'a-sonar';
  }

  function isHeatExperimentMode(state) {
    return state.activeMode === 5 && state.activeExperimentId === 'a-heat';
  }

  function isPendulumExperimentMode(state) {
    return state.activeMode === 6 && state.activeExperimentId === 'a-pendulum';
  }

  function isRightRailMode(state) {
    return isCollisionExperimentMode(state) ||
      isMotionExperimentMode(state) ||
      isForceExperimentMode(state) ||
      isHeatExperimentMode(state) ||
      isPendulumExperimentMode(state);
  }

  function rightRailExperimentTitle(state) {
    if (isHookeExperimentMode(state)) return 'Νόμος του Hooke';
    if (isWeightExperimentMode(state)) return 'Μάζα/Βάρος - επιμήκυνση';
    if (isBuoyancyExperimentMode(state)) return 'Άνωση';
    if (isForceExperimentMode(state)) return 'Πείραμα δύναμης';
    if (isHeatExperimentMode(state)) return 'Θερμική ισορροπία';
    if (isPendulumExperimentMode(state)) return 'Απλό εκκρεμές';
    if (isSonarExperimentMode(state)) return 'SONAR: Μέτρηση απόστασης με ήχο';
    if (isMotionExperimentMode(state)) return 'Ευθύγραμμη ομαλή κίνηση';
    if (isCollisionExperimentMode(state)) return '3ος Νόμος Newton: Κρούση και ώθηση';
    return 'Πείραμα';
  }

  global.PalladioExperimentViewUtils = {
    isCollisionExperimentMode,
    isHookeExperimentMode,
    isWeightExperimentMode,
    isBuoyancyExperimentMode,
    isForceExperimentMode,
    isMotionExperimentMode,
    isSonarExperimentMode,
    isHeatExperimentMode,
    isPendulumExperimentMode,
    isRightRailMode,
    rightRailExperimentTitle
  };
})(window);
