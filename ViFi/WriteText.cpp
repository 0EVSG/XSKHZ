#include "ViFi/WriteText.hpp"
#include "ViFi/FileTree.hpp"
#include <exception>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>

namespace {

// Compute the number of hex digits needed to express an entry id.
constexpr int hexWidth(FileTree::Id maxId) {
  if (maxId > 0) {
    return hexWidth(maxId >> 8) + 2;
  }
  return 0;
}

void writeNode(const FileTree &tree, const FileTree::Node *node,
               const fs::path &dir, std::ostream &out) {
  FileTree::Range range = tree.entries(node);
  // Check for directory content.
  if (range.begin != range.end) {
    int width = hexWidth(tree.maxEntryId());
    // Iterate through directory content.
    for (auto it = range.begin; it != range.end; ++it) {
      const FileTree::Node *sub = *it;
      fs::path name = FileTree::nodeName(sub);
      fs::path path = dir.empty() ? name : dir / name;
      // Write entry id.
      out << std::right << std::setfill('0') << std::setw(width) << std::hex
          << FileTree::nodeId(sub);
      // Write entry path, separated by a tab character.
      out << '\t' << WriteText::pathToString(path) << std::endl;
      // Recursively write subdirectories.
      writeNode(tree, sub, path, out);
    }
  }
}

} // namespace

std::string WriteText::pathToString(const fs::path &path) {
  return path.generic_string();
}

void WriteText::write(const FileTree &tree, const fs::path &file) {
  try {
    // Open file in write mode and write tree file to it as an IO device.
    std::ofstream out(file.string(), std::ios_base::out | std::ios_base::trunc);
    out.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    if (!out.is_open()) {
      throw std::runtime_error("Unable to open file for writing.");
    }
    write(tree, out);
  } catch (...) {
    std::throw_with_nested(
        std::runtime_error("Failed to write file " + file.string()));
  }
}

void WriteText::write(const FileTree &tree, std::ostream &out) {
  // Write path of the base directory.
  out << "# ViFi@" << pathToString(tree.basePath()) << std::endl;
  // Write directory tree to text file.
  writeNode(tree, tree.baseNode(), fs::path(), out);
  // Finish writing.
  out.flush();
}
