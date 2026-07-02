(function (global) {
  'use strict';

  function escapeHtml(value) {
    return String(value ?? '')
      .replace(/&/g, '&amp;')
      .replace(/</g, '&lt;')
      .replace(/>/g, '&gt;')
      .replace(/"/g, '&quot;')
      .replace(/'/g, '&#39;');
  }

  function summaryCard(label, value, detail) {
    return '<div class="summary-card"><span>' + escapeHtml(label) + '</span><strong>' +
      escapeHtml(value) + '</strong><small>' + escapeHtml(detail) + '</small></div>';
  }

  function joinCards(cards) {
    return cards.filter(Boolean).join('');
  }

  function emptyMessage(message) {
    return '<div class="experiment-empty">' + escapeHtml(message) + '</div>';
  }

  function emptyTableRow(colspan, message) {
    return '<tr><td colspan="' + Number(colspan || 1) + '" class="experiment-empty">' +
      escapeHtml(message) + '</td></tr>';
  }

  global.PalladioReadoutUtils = {
    summaryCard,
    joinCards,
    emptyMessage,
    emptyTableRow
  };
})(typeof window !== 'undefined' ? window : globalThis);
