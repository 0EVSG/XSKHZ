#include "ViFi/FileOpRunner.hpp"
#include <stdexcept>
#include <string>

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
  fs::copy(source, temporary(entryId), fs::copy_options::recursive);
}

void FileOpRunner::moveOut(Id entryId, const fs::path &source) {
  fs::rename(source, temporary(entryId));
}

void FileOpRunner::remove(const fs::path &source) { fs::remove_all(source); }

void FileOpRunner::copyIn(Id entryId, const fs::path &target) {
  fs::copy(temporary(entryId), target, fs::copy_options::recursive);
}

void FileOpRunner::moveIn(Id entryId, const fs::path &target) {
  fs::rename(temporary(entryId), target);
}

void FileOpRunner::createDir(const fs::path &target) {
  fs::create_directory(target);
}
