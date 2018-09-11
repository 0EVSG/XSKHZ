#ifndef FILEOPRUNNER_HPP
#define FILEOPRUNNER_HPP

#include "ViFi/FileOpSequence.hpp"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

/*!
 * \class FileOpRunner FileOpRunner.hpp "ViFi/FileOpRunner.hpp"
 * \brief Runs file operations stored in the sequence base class.
 */
class FileOpRunner : public FileOpSequence {
public:
  /*!
   * \brief File operation runner that uses given temporary directory.
   * \param tempDir Directory to be used as temporary space.
   * \exception std::runtime_error On unusable temporary directory.
   * \exception fs::filesystem_error If creation of temporary directory fails.
   */
  FileOpRunner(const fs::path &tempDir);
  virtual ~FileOpRunner() override = default; //!< Delete internal operations.

  /*!
   * \brief Finish operation execution, clear temporary space.
   * \exception fs::filesystem_error If removal of temporary directory fails.
   */
  void finish();

protected:
  /*!
   * \brief Get a path to store a file or directory temporarily.
   * \param entryId Id of the file or directory entry.
   * \return Path to store the file or directory temporarily.
   */
  virtual fs::path temporary(int entryId) const override;

  /*!
   * \brief Called to copy a file or directory out to temporary space.
   * \param entryId Unique id of the file or directory.
   * \param source Path to copy the file or directory from.
   * \exception fs::filesystem_error On file operation failure.
   */
  virtual void copyOut(int entryId, const fs::path &source) override;

  /*!
   * \brief Called to move a file or directory out to temporary space.
   * \param entryId Unique id of the file or directory.
   * \param source Path to move the file or directory from.
   * \exception fs::filesystem_error On file operation failure.
   */
  virtual void moveOut(int entryId, const fs::path &source) override;

  /*!
   * \brief Called to permanently remove a file or directory.
   * \param source Path of the file or directory to be removed.
   * \exception fs::filesystem_error On file operation failure.
   */
  virtual void remove(const fs::path &source) override;

  /*!
   * \brief Called to copy a file or directory in from temporary space.
   * \param entryId Unique id of the file or directory.
   * \param target Path to copy the file or directory to.
   * \exception fs::filesystem_error On file operation failure.
   */
  virtual void copyIn(int entryId, const fs::path &target) override;

  /*!
   * \brief Called to move a file or directory in from temporary space.
   * \param entryId Unique id of the file or directory.
   * \param target Path to move the file or directory to.
   * \exception fs::filesystem_error On file operation failure.
   */
  virtual void moveIn(int entryId, const fs::path &target) override;

  /*!
   * \brief Called to create a new directory.
   * \param target Path of the directory to be created.
   * \exception fs::filesystem_error On file operation failure.
   */
  virtual void createDir(const fs::path &target) override;

private:
  fs::path _tempDir; // Directory used as temporary space.
};

#endif // FILEOPRUNNER_HPP
