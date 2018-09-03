#include "ViFi/FileTree.hpp"
#include "ViFi/FileOpSequence.hpp"
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace {
// Maximum directory level, used as pivot for unchanged paths.
constexpr FileTree::Level MAX_LEVEL =
    std::numeric_limits<FileTree::Level>::max();
// Entry id of the root directory.
constexpr FileTree::Id ROOT_ID = 0;
// Maximum valid entry id.
constexpr FileTree::Id MAX_ID = std::numeric_limits<FileTree::Id>::max() - 2;
// Check for a valid entry id.
constexpr bool isValidId(FileTree::Id id) { return id <= MAX_ID; }
// Invalid entry id that stands for a non-existing (removed) entry.
constexpr FileTree::Id NONE_ID = MAX_ID + 1;
// Invalid entry id that stands for a newly created directory.
constexpr FileTree::Id CREATE_DIR = MAX_ID + 2;
} // namespace

/*!
 * \brief Internal data to store a file tree node.
 */
struct FileTree::Node {
  /*!
   * \brief Complete Node constructor with "unchanged" default pivot.
   */
  Node(const Node *_dir, Id _entry, Id _target, fs::path _name, Level _level,
       Level _pivot = MAX_LEVEL)
      : dir(_dir), entry(_entry), target(_target), name(std::move(_name)),
        level(_level), pivot(_pivot) {}

  /*!
   * \brief Construct temporary Node for search comparison.
   */
  Node(const Node *_dir, fs::path _name = fs::path())
      : dir(_dir), entry(NONE_ID), target(NONE_ID), name(std::move(_name)),
        level(0), pivot(MAX_LEVEL) {}

  const Node *dir; //!< Pointer to parent directory node.
  Id entry;        //!< Id of the directory entry, a file or also a directory.
  Id target;       //!< Target Id of the directory entry.
  fs::path name;   //!< Entry name in the directory.
  Level level;     //!< Directory depth level in the file tree.
  Level pivot;     //!< Level of first path difference for target.

  /*!
   * \brief Data to store the entry change of a file tree node.
   */
  struct Move {
    Id from; //!< Entry id before the change.
    Id to;   //!< Entry id after the change.
  };

  std::vector<Move> moves; //!< Stores one move per pivot, which level moves.

  /*!
   * \brief Get the move data for given pivot.
   * \param pvt Pivot level, corresponding to execution level.
   * \return Move reference for given pivot.
   */
  Move &move(Level pvt) { return moves[moves.size() - pvt]; }

  /*!
   * \brief Get the constant move data for given pivot.
   * \param pvt Pivot level, corresponding to execution level.
   * \return Constant move reference for given pivot.
   */
  [[nodiscard]] const Move &move(Level pvt) const {
    return moves.at(moves.size() - pvt);
  }

  /*!
   * \brief Reassemble the filesystem path of the file tree node.
   * \return Filesystem path of this file tree node.
   */
  [[nodiscard]] fs::path path() const {
    if (dir && dir != this) {
      return dir->path() / name;
    } else {
      return name;
    }
  }

  /*!
   * \brief Equality comparison.
   * \param other File tree node to compare this to.
   * \return True if both nodes are equal.
   */
  bool operator==(const Node &other) const {
    return dir->entry == other.dir->entry && entry == other.entry &&
           target == other.target && name == other.name && level == other.level;
  }
};

namespace {
// Compute the directory level where the path of two entry nodes diverge.
FileTree::Level pivot(const FileTree::Node *nodeA,
                      const FileTree::Node *nodeB) {
  FileTree::Level result = 0;
  if (nodeA && nodeB) {
    const FileTree::Node *nextA = nodeA->dir;
    const FileTree::Node *nextB = nodeB->dir;
    if (nodeA->level > nodeB->level) {
      nextB = nodeB;
      result = nodeB->level;
    } else if (nodeA->level < nodeB->level) {
      nextA = nodeA;
      result = nodeA->level;
    } else {
      result = (nodeA->name != nodeB->name) ? nodeA->level : MAX_LEVEL;
    }
    if (nextA != nodeA || nextB != nodeB) {
      result = std::min(result, pivot(nextA, nextB));
    }
  }
  return result;
}

// Less than comparison for the directory of referenced nodes.
bool lessDir(const FileTree::Node *nodeA, const FileTree::Node *nodeB) {
  return nodeA->dir->entry < nodeB->dir->entry;
}

// Less than comparison for directory and name of referenced nodes.
bool lessDirName(const FileTree::Node *nodeA, const FileTree::Node *nodeB) {
  return nodeA->dir->entry < nodeB->dir->entry ||
         (nodeA->dir->entry == nodeB->dir->entry && nodeA->name < nodeB->name);
}
} // namespace

fs::path FileTree::name(const FileTree::Node *entry) {
  if (entry) {
    return entry->name;
  } else {
    return fs::path();
  }
}

FileTree::FileTree()
    : _root(new Node(nullptr, ROOT_ID, ROOT_ID, fs::path(), 0)),
      _original(true), _index(new std::vector<Node *>()) {
  _root->dir = _root;
  clear();
}

FileTree::~FileTree() {
  clear();
  delete _root;
  delete _index;
}

void FileTree::endOriginal() {
  for (Id id = 0; id < _byId.size(); ++id) {
    const Node *node = _byId.at(id);
    if (!node) {
      throw std::runtime_error("Missing entry from original tree.");
    } else if (node->entry != id) {
      throw std::runtime_error("Invalid entry id in original tree.");
    }
  }
  _original = false;
  // Create index of original file tree.
  index();
}

void FileTree::endTarget() {
  computePivots();
  computeMoves();
}

fs::path FileTree::basePath() const { return _root->name; }

const FileTree::Node *FileTree::setBasePath(const fs::path &path) {
  _root->name = path;
  return _root;
}

const FileTree::Node *FileTree::baseNode() const { return _root; }

fs::path FileTree::nodePath(const FileTree::Node *node) {
  if (node) {
    return node->path();
  }
  return fs::path();
}

FileTree::Id FileTree::nodeId(const FileTree::Node *node) {
  if (node) {
    return node->entry;
  }
  return NONE_ID;
}

fs::path FileTree::nodeName(const FileTree::Node *node) {
  if (node) {
    return node->name;
  }
  return fs::path();
}

FileTree::Id FileTree::maxEntryId() const { return _byId.size() - 1; }

FileTree::Range FileTree::entries(const FileTree::Node *dir) const {
  // Binary search the range of all entries in given directory.
  const std::vector<Node *> *idx = index();
  const Node value(dir);
  auto range = std::equal_range(idx->begin(), idx->end(), &value, lessDir);
  return {range.first, range.second};
}

void FileTree::generate(FileOpSequence &sequence) const {
  // Set maximum entry id for hex id length.
  sequence.setMaxEntryId(maxEntryId());
  // Count additional target copies and add corresponding file operations.
  std::vector<int> copies(_byId.size(), 0);
  for (const Node *node : _nodes) {
    for (Level p = node->level; p >= 1; --p) {
      const Node::Move &move = node->move(p);
      if (isValidId(move.to) && move.to != move.from) {
        sequence.addInOp(move.to, node->path(), false, node->level, p);
        copies[move.to] += 1;
      } else if (move.to == CREATE_DIR && move.to != move.from) {
        sequence.addInOp(0, node->path(), true, node->level, p);
      }
    }
  }
  // Add file operations out to temporary space.
  for (const Node *node : _nodes) {
    for (Level p = node->level; p >= 1; --p) {
      const Node::Move &move = node->move(p);
      if (p == node->level && isValidId(move.from) &&
          (move.from != move.to || copies.at(move.from) > 0)) {
        bool keep = (move.from == move.to);
        sequence.addOutOp(move.from, node->path(), keep, node->level, p,
                          copies.at(move.from));
      } else if (move.from != NONE_ID && move.from != move.to) {
        Id id = isValidId(move.from) ? move.from : ROOT_ID;
        sequence.addOutOp(id, node->path(), false, node->level, p, 0);
      }
    }
  }
}

void FileTree::clear() noexcept {
  *_root = Node(_root, ROOT_ID, ROOT_ID, fs::path(), 0);

  _byId.resize(1, _root);
  for (const Node *node : _nodes) {
    delete node;
  }
  _nodes.clear();
  _index->clear();
}

bool FileTree::operator==(const FileTree &other) const {
  return _root->name == other._root->name &&
         std::equal(index()->begin(), index()->end(), other.index()->begin(),
                    other.index()->end(),
                    [](const Node *nodeA, const Node *nodeB) {
                      return *nodeA == *nodeB;
                    });
}

const FileTree::Node *FileTree::addEntry(const Node *dir,
                                         const fs::path &name) {
  if (_original) {
    return addEntry(dir, _byId.size(), name);
  } else {
    return addEntry(dir, NONE_ID, name);
  }
}

FileTree::Node *FileTree::addEntry(const Node *dir, Id entryId,
                                   const fs::path &name) {
  Node *node = nullptr;
  if (dir) {
    if (_original) {
      if (isValidId(entryId)) {
        if (_byId.size() <= entryId) {
          _byId.resize(entryId + 1, nullptr);
        } else if (_byId.at(entryId)) {
          throw std::runtime_error("Entry id of [" + name.string() +
                                   "] already in use.");
        }
        node = new Node(dir, entryId, NONE_ID, name, dir->level + 1);
        _byId[entryId] = node;
        _nodes.push_back(node);
      } else {
        throw std::runtime_error("Invalid entry id [" + name.string() + "].");
      }
    } else {
      // Search for a directory entry of the same name.
      const Node value(dir, name);
      auto range =
          std::equal_range(_index->begin(), _index->end(), &value, lessDirName);
      if (range.first != range.second) {
        // Existing entry found, set target entry id accordingly.
        node = _byId.at((*range.first)->entry);
        node->target = entryId;
      } else {
        // No existing entry, create a new entry node and append it.
        node = new Node(dir, NONE_ID, entryId, name, dir->level + 1);
        _nodes.push_back(node);
      }
    }
  }
  return node;
}

void FileTree::computePivots() {
  for (Node *node : _nodes) {
    // Compute pivot level if the path has changed.
    if (isValidId(node->target) && node->entry != node->target) {
      node->pivot = pivot(node, _byId.at(node->target));
      if (node->pivot < node->level && node->dir->pivot < node->pivot) {
        // Inherit smaller pivot from parent directory.
        node->pivot = node->dir->pivot;
      }
    } else if (!isValidId(node->target)) {
      node->pivot = node->level;
    }
  }
}

void FileTree::computeMoves() {
  for (Node *node : _nodes) {
    // Initialize level number of moves with NONE_ID ids.
    node->moves.resize(node->level, {NONE_ID, NONE_ID});
  }
  for (const Node *node : _nodes) {
    // Request intermediate target directories if missing, to pivot depth.
    for (const Node *parent = node->dir;
         parent && parent->level >= node->pivot && parent->dir != parent;
         parent = parent->dir) {
      auto it = std::find(_nodes.begin(), _nodes.end(), parent);
      if (it != _nodes.end()) {
        (*it)->move(node->pivot).to = CREATE_DIR;
      }
    }
  }
  for (Node *node : _nodes) {
    // Iterate relevant pivots from level to 1.
    Id previous = node->entry;
    for (Level p = node->level; p >= 1; --p) {
      // Consider prior parent directory changes in this pivot.
      if (p <= node->dir->level &&
          node->dir->move(p).from != node->dir->move(p).to) {
        // Most parent directory changes result in a non-existing entry.
        previous = NONE_ID;
        if (isValidId(node->dir->move(p).to)) {
          // Search for a former entry in the original directory with this name.
          const Node value(_byId.at(node->dir->move(p).to), node->name);
          auto range = std::equal_range(index()->begin(), index()->end(),
                                        &value, lessDirName);
          if (range.first != range.second) {
            // Adopt former entry that is copied with its parent directory.
            const Node *former = *range.first;
            previous = former->move(former->level).to;
          }
        }
      }
      // Keep previous entry id except for ...
      Id next = previous;
      if (p == node->pivot) {
        // ... target id when we reach its pivot and ...
        next = node->target;
      } else if (p == node->level && node->entry != node->target) {
        // ... when we move out the original entry at the beginning.
        next = NONE_ID;
      }
      if (!isValidId(next) && node->move(p).to == CREATE_DIR) {
        // Create intermediate directory if necessary, none is set explicitly.
        next = CREATE_DIR;
      }
      // Set move at pivot p from previous to next entry id.
      node->move(p) = {previous, next};
      previous = next;
    }
  }
}

const std::vector<FileTree::Node *> *FileTree::index() const {
  if (_index->size() != _nodes.size()) {
    *_index = _nodes;
    std::sort(_index->begin(), _index->end(), lessDirName);
  }
  return _index;
}
