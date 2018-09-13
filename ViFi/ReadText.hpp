#ifndef READTEXT_HPP
#define READTEXT_HPP

#include <boost/filesystem.hpp>
#include <iosfwd>

class FileTree;

namespace fs = boost::filesystem;

/*!
 * \class ReadText ReadText.hpp "ViFi/ReadText.hpp"
 * \brief Read a file tree from an text file.
 *
 * Constructs a file tree from the ids and paths stored in a text file, like
 * the one that is presented to the user in an editor. All implementation is
 * static, the class only exists for documentation and namespace purposes.
 */
class ReadText {
public:
  /*!
   * \brief Convert an escaped path string to path.
   * \param str Path string with escaped slashes in filenames.
   * \return Resulting standard path.
   */
  static fs::path stringToPath(const std::string &str);

  /*!
   * \brief Read file tree data from a text file.
   * \param file Path to an existing and readable text file.
   * \param tree File tree to store the data that is read.
   *
   * The format of the text file should match the output that WriteText::write()
   * produces.
   *
   * \throws std::nested_exception Wrapped-up internal exception.
   */
  static void read(const fs::path &file, FileTree &tree);

  /*!
   * \brief Read file tree data from a text file.
   * \param in Open input stream ready to be read.
   * \param tree File tree to store the data that is read.
   *
   * The format of the text file should match the output that WriteText::write()
   * produces.
   *
   * \remark This method is merely intended for testing.
   * \throws std::nested_exception Wrapped-up internal exception.
   */
  static void read(std::istream &in, FileTree &tree);
};

#endif // READTEXT_HPP
