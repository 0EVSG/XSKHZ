#include "ViFi/ReadText.hpp"
#include "ViFi/FileTree.hpp"
#include <exception>
#include <map>

fs::path ReadText::stringToPath(const std::string &str) { return str; }

void ReadText::read(const fs::path &file, FileTree &tree) {
  try {
    // Open text file in read mode and read its content.
    std::ifstream in(file.string(), std::ios_base::in);
    if (!in.is_open()) {
      throw std::runtime_error("Unable to open file for reading.");
    }
    read(in, tree);
  } catch (...) {
    std::throw_with_nested(
        std::runtime_error("Failed to read file " + file.string()));
  }
}

void ReadText::read(std::istream &in, FileTree &tree) {
  // Parse and check header line.
  std::string header;
  if (std::getline(in, header) && header.substr(0, 7) == "# ViFi@") {
    // Set base path from header line.
    fs::path base(stringToPath(header.substr(7)));
    tree.setBasePath(base);
  } else {
    throw std::runtime_error("Unknown header line " + header);
  }

  // Parse entry lines.
  std::map<fs::path, FileTree::Id> entries;
  std::string line;
  while (std::getline(in, line)) {
    // Check presence of tab separator.
    std::string::size_type separator = line.find('\t');
    if (separator < std::string::npos) {
      // Parse entry id.
      std::string::size_type parsed = 0;
      FileTree::Id id = std::stoul(line, &parsed, 16);
      if (id > 0 && parsed == separator) {
        // Parse entry path.
        ++parsed;
        fs::path relative(stringToPath(line.substr(parsed)));
        auto insert = entries.insert({relative, id});
        if (!insert.second) {
          throw std::runtime_error("Duplicate path in " + line);
        }
      } else {
        throw std::runtime_error("Invalid entry id in " + line);
      }
    } else {
      throw std::runtime_error("Missing tabulator in " + line);
    }
  }
  if (in.bad()) {
    throw std::runtime_error("Generic error reading file.");
  }

  // Feed entries into file tree.
  fs::path previous;
  std::vector<const FileTree::Node *> parents = {tree.baseNode()};
  // Iterate entries sorted by path.
  for (const auto &entry : entries) {
    fs::path path = entry.first;
    FileTree::Id id = entry.second;
    // Find directory level where last and current paths differ.
    FileTree::Level level = 0;
    auto part = path.begin();
    for (auto prev = previous.begin();
         prev != previous.end() && part != path.end() && *prev == *part;
         ++prev, ++part) {
      ++level;
    }
    // Truncate parent directory stack to matching level.
    parents.resize(level + 1);
    // Add intermediate directories without entry ids.
    fs::path name = *part;
    for (++part; part != path.end(); ++part) {
      parents.push_back(tree.addEntry(parents.at(level), name));
      name = *part;
      ++level;
    }
    // Add leaf entry of current path with given id.
    parents.push_back(tree.addEntry(parents.at(level), id, name));
    // Set previous path for next iteration.
    previous = path;
  }
}
