#ifndef WRITETEXT_HPP
#define WRITETEXT_HPP

#include <experimental/filesystem>
#include <ostream>

class FileTree;

namespace fs = std::experimental::filesystem;

/*!
 * \class WriteText WriteText.hpp "ViFi/WriteText.hpp"
 * \brief Write a file tree to a text file.
 *
 * Writes an internal file tree representation to a text file with hex ids and
 * paths, like the one that is presented to the user in an editor. All
 * implementation is static, this class only exists for documentation and
 * namespace purposes.
 */
class WriteText {
public:
  /*!
   * \brief Convert path to an escaped path string.
   * \param path Absolute or relative filesystem path.
   * \return Equivalent path string with escaped slashes in filenames.
   */
  static std::string pathToString(const fs::path &path);

  /*!
   * \brief Write file tree data to a text file.
   * \param tree A populated file tree with its base path set.
   * \param file Location for the output file, a valid file name in an existing
   *             and writable directory.
   *
   * Extracts the base path and entries from the file tree and writes
   * them into a text file at the target path. The output format must be
   * compatible with ReadText::readTreeFile().
   *
   * \warning Existing files at the output file location will be overwritten.
   * \throws std::nested_exception Wrapped-up internal exception.
   */
  static void write(const FileTree &tree, const fs::path &file);

  /*!
   * \brief Write file tree data to an I/O-device.
   * \param tree A populated file tree with its base path set.
   * \param out Open output stream ready to be written to.
   *
   * Extracts the base path and entries from the file tree and writes
   * them as a text file to the I/O-device. The output format must be
   * compatible with ReadText::readTreeFile().
   *
   * \remark This method is merely intended for testing.
   * \throws std::runtime_error Error writing the file.
   */
  static void write(const FileTree &tree, std::ostream &out);
};

#endif // WRITETEXT_HPP
