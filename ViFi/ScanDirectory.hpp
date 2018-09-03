#ifndef SCANDIRECTORY_HPP
#define SCANDIRECTORY_HPP

#include <experimental/filesystem>

class FileTree;

namespace fs = std::experimental::filesystem;

/*!
 * \class ScanDirectory ScanDirectory.hpp "ViFi/ScanDirectory.hpp"
 * \brief Scan the filesystem structure of a directory into a file tree.
 *
 * Creates an internal file tree representation from recursively scanning an
 * existing directory. All implementation is static, this class only exists for
 * documentation and namespace purposes.
 */
class ScanDirectory {
public:
  /*!
   * \brief Scan the given directory and load its filesystem structure.
   *
   * Recursively iterates through the given directory and stores the directory
   * and file entries as nodes in the file tree. It is only useful to load the
   * original part of a file tree.
   *
   * \param directory Path of the directory to be scanned.
   * \param tree File tree to store the directory scan results.
   * \exception std::nested_exception Wrapped-up internal exception.
   */
  static void scan(const fs::path &directory, FileTree &tree);
};

#endif // SCANDIRECTORY_HPP
