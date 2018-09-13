#include "ViFi/FileTree.hpp"
#include "ViFi/ReadText.hpp"
#include "ViFi/WriteText.hpp"
#include "gtest/gtest.h"
#include <exception>
#include <sstream>

/*!
 * \brief Test write and read of file trees.
 * \see FileTree
 */
class TextAndBackAgain : public testing::Test {
protected:
  /*!
   * \brief Check path to escaped string conversion and vice versa.
   * \param path Absolute or relative filesystem path.
   * \param str Equivalent path string with escaped slashes.
   */
  void checkPathConversion(const fs::path &path, const std::string &str) {
    EXPECT_EQ(str, WriteText::pathToString(path));
    EXPECT_EQ(path, ReadText::stringToPath(str));
  }

  /*!
   * \brief Read in and write out file tree data to check the result.
   * \param in Text formatted as expected by ReadText::read().
   * \param out Text as expected to be written by WriteText::write().
   */
  void checkReadWrite(const std::string &in, const std::string &out) {
    try {
      // Read in file tree A from in text.
      std::istringstream inStrA(in);
      FileTree treeA;
      ReadText::read(inStrA, treeA);
      // Write out of file tree A must match the out text.
      std::ostringstream outStrA;
      WriteText::write(treeA, outStrA);
      EXPECT_EQ(out, outStrA.str());
      // Read in file tree B from out text, must be equal to file tree A.
      std::istringstream inStrB(in);
      FileTree treeB;
      ReadText::read(inStrB, treeB);
      EXPECT_TRUE(treeA == treeB);
      // Write out of file tree B must match the out text again.
      std::ostringstream outStrB;
      WriteText::write(treeB, outStrB);
      EXPECT_EQ(out, outStrB.str());
    } catch (const std::exception &e) {
      FAIL() << "Exception: " << e.what();
    } catch (...) {
      FAIL() << "Unknown exception.";
    }
  }
};

TEST_F(TextAndBackAgain, PathConversion) {
  // Empty path.
  checkPathConversion("", "");
  // Simple absolute paths.
  checkPathConversion("/", "/");
  checkPathConversion("/x", "/x");
  checkPathConversion("/x/y/z", "/x/y/z");
  // Simple relative paths.
  checkPathConversion("x", "x");
  checkPathConversion("x/y/z", "x/y/z");
  // Paths with spaces.
  checkPathConversion("/ ", "/ ");
  checkPathConversion("/a /path /with spaces ", "/a /path /with spaces ");
  checkPathConversion(" ", " ");
  checkPathConversion(" a /path /with spaces ", " a /path /with spaces ");
  // Paths with slashes.
  checkPathConversion(R"(/\x\/\y\/\z\)", R"(/\x\/\y\/\z\)");
  checkPathConversion(R"(\x\/\y\/\z\)", R"(\x\/\y\/\z\)");
}

TEST_F(TextAndBackAgain, Empty) {
  std::ostringstream text;
  text << "# ViFi@" << std::endl;
  checkReadWrite(text.str(), text.str());
}

TEST_F(TextAndBackAgain, BasePath) {
  std::ostringstream text;
  text << "# ViFi@/example/dir" << std::endl;
  checkReadWrite(text.str(), text.str());
}

TEST_F(TextAndBackAgain, SimpleTree) {
  std::ostringstream in;
  in << "# ViFi@/example/dir" << std::endl;
  in << "02" << '\t' << "dir2" << std::endl;
  in << "01" << '\t' << "file1.txt" << std::endl;
  in << "04" << '\t' << "dir2/file3.txt" << std::endl;
  in << "03" << '\t' << "dir2/file2.txt" << std::endl;
  // Output must be ordered by path.
  std::ostringstream out;
  out << "# ViFi@/example/dir" << std::endl;
  out << "02" << '\t' << "dir2" << std::endl;
  out << "03" << '\t' << "dir2/file2.txt" << std::endl;
  out << "04" << '\t' << "dir2/file3.txt" << std::endl;
  out << "01" << '\t' << "file1.txt" << std::endl;
  checkReadWrite(in.str(), out.str());
}
