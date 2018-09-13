#ifndef FILETREE_HPP
#define FILETREE_HPP

#include <boost/filesystem.hpp>
#include <vector>

namespace fs = boost::filesystem;

namespace FVM {
class IteratorFVM;
class Assembler;
} // namespace FVM

class FileOpSequence;

/*!
 * \class FileTree FileTree.hpp "ViFi/FileTree.hpp"
 * \brief Defines a file tree composed of directory entry nodes.
 *
 * The nodes of the file tree each represent a unique path, with both original
 * and changed paths combined in a single tree. When transitioning from the
 * original to the changed file tree, the actual file or directory object
 * located at a path may change in time, as it is moved away or replaced by a
 * different object.
 * Thus each path node records the file or directory object located there for
 * every step of the transition algorithm. Objects are identified by the unique
 * id that is assigned in the original file tree, presented as hex id in the
 * text file.
 *
 * Typical usage goes as follows:
 * 1. Add the path nodes of the original file tree through addEntry().
 * 2. Finish the original file tree with endOriginal().
 * 3. Add the path nodes of the changed file tree through addEntry().
 * 4. Finish the changed file tree with endTarget().
 * 5. Let generate() create the file operation sequence from the changes.
 *
 * For efficiency, FileTree keeps indexes of the path nodes sorted by id and
 * by parent directory / entry name.
 */
class FileTree {
public:
  typedef std::size_t Id;    //!< Identifier for directory and file entries.
  typedef std::size_t Level; //!< Type used for directory levels.

  struct Node; //!< Stores a directory entry as a node in the file tree.

  /*!
   * \brief Get the name of given entry.
   * \param entry Entry node of the file tree.
   * \return Name of the entry in its parent directory.
   */
  static fs::path name(const Node *entry);

  FileTree();  //!< Initialize empty file tree.
  ~FileTree(); //!< Clean up data and caches.

  /*!
   * \brief Ends loading the original tree, prepares for target.
   */
  void endOriginal();

  /*!
   * \brief Ends loading the target tree, prepares for generate().
   */
  void endTarget();

  /*!
   * \brief Get the base directory.
   * \return Path of the base directory.
   */
  fs::path basePath() const;

  /*!
   * \brief Set the base directory.
   * \param path Path to the base directory.
   * \return Handler for the base directory.
   */
  const Node *setBasePath(const fs::path &path);

  /*!
   * \brief Get the entry node of the base directory.
   */
  const Node *baseNode() const;

  /*!
   * \brief Get the path of a file tree node.
   * \param node File tree node.
   * \return Complete path of the node, empty if node is invalid.
   */
  static fs::path nodePath(const Node *node);

  /*!
   * \brief Get the id of a file tree node.
   * \param node File tree node.
   * \return Id of the node, -1 if node is invalid.
   */
  static Id nodeId(const Node *node);

  /*!
   * \brief Get the entry name of a file tree node.
   * \param node File tree node.
   * \return Entry name of the node, empty if node is invalid.
   */
  static std::string nodeName(const Node *node);

  /*!
   * \brief Get the maximum entry id.
   * \return Maximum id used by any entry in the file tree.
   */
  Id maxEntryId() const;

  //! Range of nodes in a vector.
  struct Range {
    std::vector<Node *>::const_iterator begin; //!< Begin of the range.
    std::vector<Node *>::const_iterator end;   //!< End of the range.
  };

  /*!
   * \brief Get entries of a directory node.
   * \return Range of directory entries sorted by name.
   */
  Range entries(const Node *dir) const;

  /*!
   * \brief Generate file operation sequence from file tree.
   * \param sequence Holds the resulting file operation sequence.
   */
  void generate(FileOpSequence &sequence) const;

  /*!
   * \brief Clear all data.
   */
  void clear() noexcept;

  //! Equality comparator.
  bool operator==(const FileTree &other) const;

  /*!
   * \brief Add an entry node to given directory.
   * \param dir Parent directory handler.
   * \param name Name of the entry in the parent directory.
   * \return Handler for the added entry, null on failure.
   */
  const Node *addEntry(const Node *dir, const fs::path &name);

  /*!
   * \brief Add an entry node to given directory.
   * \param dir Parent directory handler.
   * \param entryId Id of the entry to be added.
   * \param name Name of the entry in the parent directory.
   * \return Handler for the added entry, null on failure.
   */
  Node *addEntry(const Node *dir, Id entryId, const fs::path &name);

private:
  // Compute pivot levels.
  void computePivots();
  // Compute moves per pivot.
  void computeMoves();
  // Get and update node index sorted by directory and name.
  const std::vector<Node *> *index() const;

  Node *_root;    // Root directory of the file tree.
  bool _original; // Set when loading an original file tree.

  std::vector<Node *> _byId;   // Access to original nodes by entry id.
  std::vector<Node *> _nodes;  // All nodes except root.
  std::vector<Node *> *_index; // Nodes sorted by directory and name.
};

#endif // FILETREE_HPP
