#include "src/polyfill/file-dialog.hpp"

// QFileDialog fails for WSL paths on Windows, so we need to reimplement it :(

#ifdef _WIN32

#include <windows.h>

#include "src/polyfill/windows/unicode.hpp"
#include <shobjidl.h>
#include <string>
#include <vector>

struct FileFilterW {
  std::wstring name;
  std::wstring patterns;
};

static IFileOpenDialog *
openDialog(const char *dialogTitle, FILEOPENDIALOGOPTIONS options,
           const std::initializer_list<FileDialog::FileFilter> &filters) {
  IFileOpenDialog *dialog;
  CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
  CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
                   IID_IFileOpenDialog, reinterpret_cast<void **>(&dialog));
  dialog->SetTitle(Unicode::toUtf16(dialogTitle).c_str());
  dialog->SetOptions(options);

  std::vector<FileFilterW> filtersWS;
  std::vector<_COMDLG_FILTERSPEC> filtersWC;
  if (filters.size() > 0) {
    filtersWS.reserve(filters.size());
    filtersWC.reserve(filters.size());

    for (const FileDialog::FileFilter &f : filters) {
      filtersWS.push_back(
          {Unicode::toUtf16(f.name), Unicode::toUtf16(f.patterns)});
    }

    for (const FileFilterW &f : filtersWS) {
      filtersWC.push_back({f.name.c_str(), f.patterns.c_str()});
    }

    dialog->SetFileTypes((unsigned int)filters.size(), filtersWC.data());
  }

  dialog->Show(nullptr);
  return dialog;
}

static fs::path
getSingle(const char *dialogTitle, FILEOPENDIALOGOPTIONS options,
          const std::initializer_list<FileDialog::FileFilter> &filters) {
  IFileOpenDialog *dialog = openDialog(dialogTitle, options, filters);
  IShellItem *item;
  fs::path result;
  HRESULT status;

  status = dialog->GetResult(&item);
  if (SUCCEEDED(status)) {
    wchar_t *pathString;
    status = item->GetDisplayName(SIGDN_FILESYSPATH, &pathString);
    if (SUCCEEDED(status)) {
      result = fs::path(pathString);
      CoTaskMemFree(pathString);
    }

    item->Release();
  }

  dialog->Release();
  CoUninitialize();

  return result;
}

static std::vector<fs::path>
getMultiple(const char *dialogTitle, FILEOPENDIALOGOPTIONS options,
            const std::initializer_list<FileDialog::FileFilter> &filters) {
  IFileOpenDialog *dialog =
      openDialog(dialogTitle, options | FOS_ALLOWMULTISELECT, filters);
  IShellItemArray *files;
  std::vector<fs::path> results;
  HRESULT status;

  status = dialog->GetResults(&files);
  if (SUCCEEDED(status)) {
    unsigned long numItems = 0;
    files->GetCount(&numItems);

    results.reserve(numItems);
    for (unsigned long i = 0; i < numItems; i++) {
      IShellItem *file;
      status = files->GetItemAt(i, &file);
      if (!SUCCEEDED(status))
        continue;

      wchar_t *pathString;
      status = file->GetDisplayName(SIGDN_FILESYSPATH, &pathString);
      if (!SUCCEEDED(status))
        continue;

      results.push_back(fs::path(pathString));
      CoTaskMemFree(pathString);
    }

    files->Release();
  }

  dialog->Release();
  CoUninitialize();

  return results;
}

fs::path FileDialog::getDirectory(const char *dialogTitle) {
  return getSingle(dialogTitle, FOS_PICKFOLDERS | FOS_FORCESHOWHIDDEN, {});
}

fs::path FileDialog::getFile(const char *dialogTitle) {
  return getSingle(dialogTitle, FOS_FORCESHOWHIDDEN, {});
}

fs::path FileDialog::getFile(const char *dialogTitle,
                             const std::initializer_list<FileFilter> &filters) {
  return getSingle(dialogTitle, FOS_STRICTFILETYPES | FOS_FORCESHOWHIDDEN,
                   filters);
}

std::vector<fs::path> FileDialog::getFiles(const char *dialogTitle) {
  return getMultiple(dialogTitle, FOS_FORCESHOWHIDDEN, {});
}

std::vector<fs::path>
FileDialog::getFiles(const char *dialogTitle,
                     const std::initializer_list<FileFilter> &filters) {
  return getMultiple(dialogTitle, FOS_STRICTFILETYPES | FOS_FORCESHOWHIDDEN,
                     filters);
}

#else

#include <QFileDialog>

static std::string
getFilterString(const std::initializer_list<FileDialog::FileFilter> &filters) {
  std::string filterString = "";
  for (const FileDialog::FileFilter &f : filters) {
    filterString += f.name;
    filterString += " (";
    std::string patterns = std::string(f.patterns);
    for (char &c : patterns) {
      if (c == ';')
        c = ' ';
    }
    filterString += patterns;
    filterString += ");;";
  }
  return filterString;
}

static inline std::vector<fs::path> toVector(const QStringList &stringList) {
  std::vector<fs::path> results;
  results.reserve(stringList.size());
  for (const QString &str : stringList) {
    results.push_back(fs::path(str.toStdString()));
  }
  return results;
}

fs::path FileDialog::getDirectory(const char *dialogTitle) {
  const QString path = QFileDialog::getExistingDirectory(nullptr, dialogTitle);
  return path.isNull() ? fs::path() : fs::to_path(path.toStdString());
}

fs::path FileDialog::getFile(const char *dialogTitle) {
  const QString path = QFileDialog::getOpenFileName(nullptr, dialogTitle);
  return path.isNull() ? fs::path() : fs::to_path(path.toStdString());
}

fs::path FileDialog::getFile(const char *dialogTitle,
                             const std::initializer_list<FileFilter> &filters) {
  const QString path = QFileDialog::getOpenFileName(
      nullptr, dialogTitle, QString(), getFilterString(filters).c_str());
  return path.isNull() ? fs::path() : fs::to_path(path.toStdString());
}

std::vector<fs::path> FileDialog::getFiles(const char *dialogTitle) {
  const QStringList paths = QFileDialog::getOpenFileNames(nullptr, dialogTitle);
  return toVector(paths);
}

std::vector<fs::path>
FileDialog::getFiles(const char *dialogTitle,
                     const std::initializer_list<FileFilter> &filters) {
  const QStringList paths = QFileDialog::getOpenFileNames(
      nullptr, dialogTitle, QString(), getFilterString(filters).c_str());
  return toVector(paths);
}

#endif
