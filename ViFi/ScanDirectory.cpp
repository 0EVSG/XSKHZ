#include "ViFi/ScanDirectory.hpp"
#include "ViFi/FileTree.hpp"
#include <exception>

namespace {

void scanDir(const fs::path &path, const FileTree::Node *dir, FileTree &tree) {
  for (fs::directory_entry &entry : fs::directory_iterator(path)) {
    // Only consider regular, non-hidden files and directories.
    if ((fs::is_regular_file(entry) || fs::is_directory(entry)) &&
        entry.path().filename().string().substr(0, 1) != ".") {
      const FileTree::Node *node = tree.addEntry(dir, entry.path().filename());
      if (fs::is_directory(entry)) {
        scanDir(entry.path(), node, tree);
      }
    }
  }
}

} // namespace

void ScanDirectory::scan(const fs::path &directory, FileTree &tree) {
  try {
    if (!fs::exists(directory)) {
      throw std::runtime_error("Directory does not exist.");
    } else if (!fs::is_directory(directory)) {
      throw std::runtime_error("Not a directory");
    }
    fs::path canonical = fs::canonical(directory);
    const FileTree::Node *root = tree.setBasePath(canonical);
    scanDir(canonical, root, tree);
  } catch (...) {
    std::throw_with_nested(
        std::runtime_error("Failed to scan directory " + directory.string()));
  }
}
