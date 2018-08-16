#ifndef FILEOPSEQUENCE_HPP
#define FILEOPSEQUENCE_HPP

#include <boost/filesystem.hpp>
#include <vector>

namespace fs = boost::filesystem;

/*!
 * \class FileOpSequence FileOpSequence.hpp "ViFi/FileOpSequence.hpp"
 * \brief Records file operations and sorts them by dependencies.
 */
class FileOpSequence {
public:
  //! File operation type.
  enum Type {
    CopyOut, //!< Copy file or directory out to temporary space.
    MoveOut, //!< Move file or directory out to temporary space.
    CopyIn,  //!< Copy file or directory in from temporary space.
  };

  FileOpSequence();          //!< Empty constructor.
  virtual ~FileOpSequence(); //!< Delete internal operations.

  bool empty() const; //!< Indicate an empty operation sequence.

  void setMaxEntryId(int id); //!< Set the maximum entry id.

  /*!
   * \brief Add a file operation out to temporary space.
   * \param entryId Entry id of the file or directory.
   * \param path Original path of the file or directory.
   * \param keep Whether to keep the original (copy or move).
   * \param level Relative directory level of the original.
   * \param pivot Pivot value for sorting, if different from level.
   * \param copies Total number of copies to be made.
   */
  void addOutOp(int entryId, fs::path path, bool keep, int level, int pivot,
                int copies);

  /*!
   * \brief Add a file operation in from the temporary space.
   * \param entryId Entry id of the file or directory.
   * \param path Target path of the file or directory.
   * \param create Whether to create a new directory.
   * \param level Relative directory level of the target.
   * \param pivot Level of first path difference from original.
   */
  void addInOp(int entryId, fs::path path, bool create, int level, int pivot);

  /*!
   * \brief Check and sort operations into a feasible order.
   */
  void prepare();

  /*!
   * \brief Compare all operations in current order.
   * \param other Operation sequence to compare to.
   * \return True if both operation sequences are the same.
   */
  bool operator==(const FileOpSequence &other) const;

  /*!
   * \brief Print file operations to stdout for user inspection.
   * \param base Print operation paths relative to this path.
   */
  void print(fs::path base);

  /*!
   * \brief Execute file operations by calling virtual methods.
   */
  void run();

protected:
  /*!
   * \brief Get a path to store a file or directory temporarily.
   * \param entryId Id of the file or directory entry.
   * \return Path to store the file or directory temporarily.
   */
  virtual fs::path temporary(int entryId) const;

  /*!
   * \brief Called to copy a file or directory out to temporary space.
   * \param entryId Unique id of the file or directory.
   * \param source Path to copy the file or directory from.
   */
  virtual void copyOut(int entryId, fs::path source);

  /*!
   * \brief Called to move a file or directory out to temporary space.
   * \param entryId Unique id of the file or directory.
   * \param source Path to move the file or directory from.
   */
  virtual void moveOut(int entryId, fs::path source);

  /*!
   * \brief Called to permanently remove a file or directory.
   * \param source Path of the file or directory to be removed.
   */
  virtual void remove(fs::path source);

  /*!
   * \brief Called to copy a file or directory in from temporary space.
   * \param entryId Unique id of the file or directory.
   * \param target Path to copy the file or directory to.
   */
  virtual void copyIn(int entryId, fs::path target);

  /*!
   * \brief Called to move a file or directory in from temporary space.
   * \param entryId Unique id of the file or directory.
   * \param target Path to move the file or directory to.
   */
  virtual void moveIn(int entryId, fs::path target);

  /*!
   * \brief Called to create a new directory.
   * \param target Path of the directory to be created.
   */
  virtual void createDir(fs::path target);

private:
  struct Operation;                     // Data for one file operation.
  std::vector<Operation *> _operations; // List of all file operations.
  int _maxEntry;                        // Maximum entry id encountered.
};

#endif // FILEOPSEQUENCE_HPP