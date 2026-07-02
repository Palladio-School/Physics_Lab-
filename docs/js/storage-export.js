(function (global) {
  'use strict';

  function getItem(key) {
    return global.localStorage.getItem(key);
  }

  function setItem(key, value) {
    global.localStorage.setItem(key, String(value));
  }

  function removeItem(key) {
    global.localStorage.removeItem(key);
  }

  function loadJsonArray(key) {
    try {
      const saved = JSON.parse(global.localStorage.getItem(key) || '[]');
      return Array.isArray(saved) ? saved : [];
    } catch (error) {
      return [];
    }
  }

  function saveJson(key, value) {
    global.localStorage.setItem(key, JSON.stringify(value));
  }

  function downloadCsv(filename, rows) {
    const csv = rows.map(row => row.join(',')).join('\n');
    const blob = new Blob([csv], { type: 'text/csv;charset=utf-8' });
    const url = URL.createObjectURL(blob);
    const link = document.createElement('a');
    link.href = url;
    link.download = filename;
    link.click();
    URL.revokeObjectURL(url);
  }

  function openPrintableDocument(html) {
    const report = global.open('', '_blank');
    if (!report) return false;
    report.document.write(html);
    report.document.close();
    return true;
  }

  global.PalladioStorageExport = {
    getItem,
    setItem,
    removeItem,
    loadJsonArray,
    saveJson,
    downloadCsv,
    openPrintableDocument
  };
})(window);
