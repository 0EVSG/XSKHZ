#include "ViFi/FileOpRunner.hpp"
#include <stdexcept>
#include <string>

namespace {
// Recursively copy a file or directory from source to target path.
void copyRecursive(const fs::path &source, const fs::path &target) {
  if (fs::is_directory(source)) {
    fs::copy_directory(source, target);
    for (const fs::directory_entry &entry :
         fs::recursive_directory_iterator(source)) {
      fs::path relative = fs::relative(entry.path(), source);
      fs::copy(entry.path(), target / relative);
    }
  } else {
    fs::copy(source, target);
  }
}
} // namespace

FileOpRunner::FileOpRunner(const fs::path &tempDir) {
  if (!tempDir.empty() && fs::exists(tempDir.parent_path())) {
    fs::create_directory(tempDir);
    _tempDir = tempDir;
  } else {
    throw std::runtime_error("Unusable directory for temporary space: " +
                             _tempDir.string());
  }
}

void FileOpRunner::finish() {
  if (!_tempDir.empty() && fs::exists(_tempDir) && fs::is_empty(_tempDir)) {
    fs::remove_all(_tempDir);
    _tempDir.clear();
  }
}

fs::path FileOpRunner::temporary(Id entryId) const {
  return _tempDir / FileOpSequence::temporary(entryId);
}

void FileOpRunner::copyOut(Id entryId, const fs::path &source) {
  copyRecursive(source, temporary(entryId));
}

void FileOpRunner::moveOut(Id entryId, const fs::path &source) {
  fs::rename(source, temporary(entryId));
}

void FileOpRunner::remove(const fs::path &source) { fs::remove_all(source); }

void FileOpRunner::copyIn(Id entryId, const fs::path &target) {
  copyRecursive(temporary(entryId), target);
}

void FileOpRunner::moveIn(Id entryId, const fs::path &target) {
  fs::rename(temporary(entryId), target);
}

void FileOpRunner::createDir(const fs::path &target) {
  fs::create_directory(target);
}
