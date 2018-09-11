
#include "ViFi/FileOpSequence.hpp"
#include "ViFi/FileTree.hpp"
#include "ViFi/ReadText.hpp"
#include "gtest/gtest.h"
#include <strstream>

/*!
 * \brief Test matching of file trees.
 * \see FileTree
 */
class FileTreeMatch : public testing::Test {
protected:
  /*!
   * \brief Check file operation order that results from a file tree.
   * \param tree File tree to generate operation sequence from.
   * \param order Expected order of file operations.
   */
  void checkOperations(const FileTree &tree, const FileOpSequence &order) {
    FileOpSequence sequence;
    tree.generate(sequence);
    sequence.prepare();
    EXPECT_TRUE(sequence == order);
  }

  /*!
   * \brief Check file operation order that results from a file tree.
   * \param current Current file tree in text format, see ReadText::read().
   * \param changed Changed file tree in text format, see ReadText::read().
   * \param order Expected order of file operations.
   */
  void checkOperations(const std::string &current, const std::string &changed,
                       const FileOpSequence &order) {
    FileTree tree;
    std::istringstream inCurrent(current);
    ReadText::read(inCurrent, tree);
    tree.endOriginal();
    std::istringstream inChanged(changed);
    ReadText::read(inChanged, tree);
    tree.endTarget();
    checkOperations(tree, order);
  }
};

TEST_F(FileTreeMatch, EmptyTree) {
  FileTree tree;
  tree.endOriginal();
  tree.endTarget();
  FileOpSequence order;
  checkOperations(tree, order);
}

TEST_F(FileTreeMatch, BasePath) {
  FileTree tree;
  tree.setBasePath("/example/dir");
  tree.endOriginal();
  tree.setBasePath("/example/dir");
  tree.endTarget();
  FileOpSequence order;
  checkOperations(tree, order);
}

TEST_F(FileTreeMatch, SimpleTree) {
  std::ostringstream current;
  current << "# ViFi@/base" << std::endl;
  current << "01" << '\t' << "file1.txt" << std::endl;
  current << "02" << '\t' << "dir2" << std::endl;
  current << "03" << '\t' << "dir2/file2.txt" << std::endl;
  current << "04" << '\t' << "dir2/file3.txt" << std::endl;
  std::ostringstream changed;
  changed << "# ViFi@/base" << std::endl;
  changed << "01" << '\t' << "file1.txt" << std::endl;
  changed << "02" << '\t' << "dir2" << std::endl;
  changed << "03" << '\t' << "file2.txt" << std::endl;
  changed << "04" << '\t' << "dir2/file3.txt" << std::endl;
  FileOpSequence order;
  order.addOutOp(3, "/base/dir2/file2.txt", false, 2, 2, 1);
  order.addInOp(3, "/base/file2.txt", false, 1, 1);
  checkOperations(current.str(), changed.str(), order);
}

TEST_F(FileTreeMatch, SwapDirectory) {
  std::ostringstream current;
  current << "# ViFi@/base" << std::endl;
  current << "01" << '\t' << "dirA" << std::endl;
  current << "02" << '\t' << "dirA/file1.txt" << std::endl;
  current << "03" << '\t' << "dirB" << std::endl;
  current << "04" << '\t' << "dirB/file2.txt" << std::endl;
  std::ostringstream changed;
  changed << "# ViFi@/base" << std::endl;
  changed << "01" << '\t' << "dirB" << std::endl;
  changed << "02" << '\t' << "dirA/file1.txt" << std::endl;
  changed << "03" << '\t' << "dirA" << std::endl;
  changed << "04" << '\t' << "dirB/file2.txt" << std::endl;
  FileOpSequence order;
  order.addOutOp(1, "/base/dirA", false, 1, 1, 1);
  order.addOutOp(3, "/base/dirB", false, 1, 1, 1);
  order.addInOp(1, "/base/dirB", false, 1, 1);
  order.addInOp(3, "/base/dirA", false, 1, 1);
  checkOperations(current.str(), changed.str(), order);
}

TEST_F(FileTreeMatch, DeleteDirectory) {
  std::ostringstream current;
  current << "# ViFi@/base" << std::endl;
  current << "01" << '\t' << "dirA" << std::endl;
  current << "02" << '\t' << "dirA/file1.txt" << std::endl;
  current << "03" << '\t' << "dirB" << std::endl;
  current << "04" << '\t' << "dirB/file2.txt" << std::endl;
  std::ostringstream changed;
  changed << "# ViFi@/base" << std::endl;
  changed << "02" << '\t' << "dirA/file1.txt" << std::endl;
  changed << "03" << '\t' << "dirB" << std::endl;
  changed << "04" << '\t' << "dirB/file2.txt" << std::endl;
  FileOpSequence order;
  order.addOutOp(1, "/base/dirA", false, 1, 1, 0);
  checkOperations(current.str(), changed.str(), order);
}

TEST_F(FileTreeMatch, IntermediateDirectories) {
  std::ostringstream current;
  current << "# ViFi@/base" << std::endl;
  current << "01" << '\t' << "file.txt" << std::endl;
  std::ostringstream changed;
  changed << "# ViFi@/base" << std::endl;
  changed << "01" << '\t' << "dirA/dirB/dirC/file.txt" << std::endl;
  FileOpSequence order;
  order.addOutOp(1, "/base/file.txt", false, 1, 1, 1);
  order.addInOp(0, "/base/dirA", true, 1, 1);
  order.addInOp(0, "/base/dirA/dirB", true, 2, 1);
  order.addInOp(0, "/base/dirA/dirB/dirC", true, 3, 1);
  order.addInOp(1, "/base/dirA/dirB/dirC/file.txt", false, 4, 1);
  checkOperations(current.str(), changed.str(), order);
}

TEST_F(FileTreeMatch, IntermediateSubdirectory) {
  std::ostringstream current;
  current << "# ViFi@/base" << std::endl;
  current << "01" << '\t' << "dirA" << std::endl;
  current << "02" << '\t' << "dirA/file1.txt" << std::endl;
  current << "03" << '\t' << "file2.txt" << std::endl;
  std::ostringstream changed;
  changed << "# ViFi@/base" << std::endl;
  changed << "01" << '\t' << "dirA" << std::endl;
  changed << "02" << '\t' << "dirA/intermediate/file1.txt" << std::endl;
  changed << "03" << '\t' << "dirA/intermediate/file2.txt" << std::endl;
  FileOpSequence order;
  order.addOutOp(2, "/base/dirA/file1.txt", false, 2, 2, 1);
  order.addInOp(0, "/base/dirA/intermediate", true, 2, 2);
  order.addInOp(2, "/base/dirA/intermediate/file1.txt", false, 3, 2);
  order.addOutOp(3, "/base/file2.txt", false, 1, 1, 1);
  order.addInOp(3, "/base/dirA/intermediate/file2.txt", false, 3, 1);
  checkOperations(current.str(), changed.str(), order);
}

TEST_F(FileTreeMatch, MovedSubdirectoryMatching) {
  std::ostringstream current;
  current << "# ViFi@/base" << std::endl;
  current << "01" << '\t' << "dirA" << std::endl;
  current << "02" << '\t' << "dirA/subdir" << std::endl;
  current << "03" << '\t' << "dirA/subdir/file1.txt" << std::endl;
  current << "04" << '\t' << "file2.txt" << std::endl;
  std::ostringstream changed;
  changed << "# ViFi@/base" << std::endl;
  changed << "01" << '\t' << "dirB" << std::endl;
  changed << "02" << '\t' << "dirA/subdir" << std::endl;
  changed << "03" << '\t' << "dirA/subdir/file1.txt" << std::endl;
  changed << "04" << '\t' << "dirB/subdir/file2.txt" << std::endl;
  FileOpSequence order;
  order.addOutOp(1, "/base/dirA", false, 1, 1, 1);
  order.addOutOp(4, "/base/file2.txt", false, 1, 1, 1);
  order.addInOp(1, "/base/dirB", false, 1, 1);
  order.addInOp(4, "/base/dirB/subdir/file2.txt", false, 3, 1);
  checkOperations(current.str(), changed.str(), order);
}

TEST_F(FileTreeMatch, IconsExample) {
  std::ostringstream current;
  current << "# ViFi@/Icons" << std::endl;
  current << "01" << '\t' << "FileIcons" << std::endl;
  current << "02" << '\t' << "FileIcons/close-file-08.svg" << std::endl;
  current << "03" << '\t' << "FileIcons/open-file-03.svg" << std::endl;
  current << "04" << '\t' << "Symbols" << std::endl;
  current << "05" << '\t' << "Symbols/letter.svg" << std::endl;
  current << "06" << '\t' << "Symbols/warning.svg" << std::endl;
  std::ostringstream changed;
  changed << "# ViFi@/Icons" << std::endl;
  changed << "01" << '\t' << "icons/actions" << std::endl;
  changed << "01" << '\t' << "icons/menus" << std::endl;
  changed << "02" << '\t' << "FileIcons/close-file.svg" << std::endl;
  changed << "03" << '\t' << "FileIcons/open-file.svg" << std::endl;
  changed << "04" << '\t' << "icons" << std::endl;
  changed << "05" << '\t' << "icons/actions/send-mail.svg" << std::endl;
  changed << "06" << '\t' << "Symbols/warning.svg" << std::endl;
  FileOpSequence order;
  order.addOutOp(2, "/Icons/FileIcons/close-file-08.svg", false, 2, 2, 1);
  order.addOutOp(3, "/Icons/FileIcons/open-file-03.svg", false, 2, 2, 1);
  order.addOutOp(5, "/Icons/Symbols/letter.svg", false, 2, 2, 1);
  order.addInOp(2, "/Icons/FileIcons/close-file.svg", false, 2, 2);
  order.addInOp(3, "/Icons/FileIcons/open-file.svg", false, 2, 2);
  order.addOutOp(1, "/Icons/FileIcons", false, 1, 1, 2);
  order.addOutOp(4, "/Icons/Symbols", false, 1, 1, 1);
  order.addInOp(4, "/Icons/icons", false, 1, 1);
  order.addInOp(1, "/Icons/icons/actions", false, 2, 1);
  order.addInOp(1, "/Icons/icons/menus", false, 2, 1);
  order.addInOp(5, "/Icons/icons/actions/send-mail.svg", false, 3, 1);
  checkOperations(current.str(), changed.str(), order);
}
