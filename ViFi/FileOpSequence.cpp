#include "ViFi/FileOpSequence.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

// Compute the number of hex digits needed to express an entry id.
constexpr int hexWidth(FileOpSequence::Id maxId) {
  if (maxId > 0) {
    return hexWidth(maxId >> 8) + 2;
  }
  return 0;
}

} // namespace

/*!
 * \brief Store parameters of a file operation.
 */
struct FileOpSequence::Operation {
  Type type;     //!< File operation type.
  Id entryId;    //!< Entry id of the file or directory.
  fs::path path; //!< Source or target path, depending on type.
  Level level;   //!< Source or target directory level.
  Level pivot;   //!< Pivot level for sorting.
  int copies;    //!< Number of copies to be made.

  //! Indicate equality of all struct members.
  bool operator==(const Operation &other) const {
    return type == other.type && entryId == other.entryId &&
           path == other.path && level == other.level && pivot == other.pivot &&
           copies == other.copies;
  }
};

FileOpSequence::FileOpSequence() : _maxEntry(0) {}

FileOpSequence::~FileOpSequence() {
  for (Operation *op : _operations) {
    delete op;
  }
  _operations.clear();
}

bool FileOpSequence::empty() const { return _operations.empty(); }

void FileOpSequence::setMaxEntryId(Id id) {
  _maxEntry = std::max(_maxEntry, id);
}

void FileOpSequence::addOutOp(Id entryId, const fs::path &path, bool keep,
                              Level level, Level pivot, int copies) {
  Type type = keep ? CopyOut : MoveOut;
  Operation *op = new Operation({type, entryId, path, level, pivot, copies});
  _operations.push_back(op);
  setMaxEntryId(entryId);
}

void FileOpSequence::addInOp(Id entryId, const fs::path &path, bool create,
                             Level level, Level pivot) {
  int copies = create ? 0 : 1;
  Operation *op = new Operation({CopyIn, entryId, path, level, pivot, copies});
  _operations.push_back(op);
  setMaxEntryId(entryId);
}

void FileOpSequence::prepare() {
  // Sort operations in order of pivot, level, type and entry id.
  std::sort(
      _operations.begin(), _operations.end(),
      [](const Operation *opA, const Operation *opB) {
        return opA->pivot > opB->pivot ||
               (opA->pivot == opB->pivot &&
                (opA->level < opB->level ||
                 (opA->level == opB->level &&
                  (opA->type < opB->type ||
                   (opA->type == opB->type && opA->entryId < opB->entryId)))));
      });
  // Check number of copies.
  std::vector<int> copies(_maxEntry + 1, 0);
  for (const Operation *op : _operations) {
    switch (op->type) {
    case CopyOut:
    case MoveOut:
      copies[op->entryId] += op->copies;
      break;
    case CopyIn:
      copies[op->entryId] -= op->copies;
      break;
    default:
      throw std::runtime_error("FileOpSequence: Unknown operation type.");
    }
    if (copies[op->entryId] < 0) {
      throw std::runtime_error(
          "FileOpSequence: In copies outnumber out copies with " +
          op->path.string());
    }
  }
}

void FileOpSequence::print(const fs::path & /*unused*/) {
  std::vector<int> copies(_maxEntry + 1, 0);
  for (const Operation *op : _operations) {
    switch (op->type) {
    case CopyOut:
      copies[op->entryId] += op->copies;
      std::cout << temporary(op->entryId) << " <=== " << op->path << std::endl;
      break;
    case MoveOut:
      copies[op->entryId] += op->copies;
      if (op->copies > 0) {
        std::cout << temporary(op->entryId) << " <--- " << op->path
                  << std::endl;
      } else {
        std::cout << "[x] <--- " << op->path << std::endl;
      }
      break;
    case CopyIn:
      copies[op->entryId] -= op->copies;
      if (op->copies == 0) {
        std::cout << "[*] ---> " << op->path << std::endl;
      } else if (copies[op->entryId] > 0) {
        std::cout << temporary(op->entryId) << " ===> " << op->path
                  << std::endl;
      } else {
        std::cout << temporary(op->entryId) << " ---> " << op->path
                  << std::endl;
      }
      break;
    default:
      throw std::runtime_error("FileOpSequence: Unknown operation type.");
    }
  }
}

void FileOpSequence::run() {
  std::vector<int> copies(_maxEntry + 1, 0);
  for (const Operation *op : _operations) {
    switch (op->type) {
    case CopyOut:
      copies[op->entryId] += op->copies;
      copyOut(op->entryId, op->path);
      break;
    case MoveOut:
      copies[op->entryId] += op->copies;
      if (op->copies > 0) {
        moveOut(op->entryId, op->path);
      } else {
        remove(op->path);
      }
      break;
    case CopyIn:
      copies[op->entryId] -= op->copies;
      if (op->copies == 0) {
        createDir(op->path);
      } else if (copies[op->entryId] > 0) {
        copyIn(op->entryId, op->path);
      } else {
        moveIn(op->entryId, op->path);
      }
      break;
    default:
      throw std::runtime_error("FileOpSequence: Unknown operation type.");
    }
  }
}

fs::path FileOpSequence::temporary(Id entryId) const {
  std::ostringstream hex;
  hex << std::right << std::setfill('0') << std::setw(hexWidth(_maxEntry))
      << std::hex << entryId;
  return hex.str();
}

void FileOpSequence::copyOut(Id /*unused*/, const fs::path & /*unused*/) {}

void FileOpSequence::moveOut(Id /*unused*/, const fs::path & /*unused*/) {}

void FileOpSequence::remove(const fs::path & /*unused*/) {}

void FileOpSequence::copyIn(Id /*unused*/, const fs::path & /*unused*/) {}

void FileOpSequence::moveIn(Id /*unused*/, const fs::path & /*unused*/) {}

void FileOpSequence::createDir(const fs::path & /*unused*/) {}

bool FileOpSequence::operator==(const FileOpSequence &other) const {
  bool same = _operations.size() == other._operations.size();
  for (std::size_t i = 0; i < _operations.size() && same; ++i) {
    same = *(_operations.at(i)) == *(other._operations.at(i));
  }
  return same;
}
