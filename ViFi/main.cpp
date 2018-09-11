#include "ViFi/FileOpRunner.hpp"
#include "ViFi/FileTree.hpp"
#include "ViFi/ReadText.hpp"
#include "ViFi/ScanDirectory.hpp"
#include "ViFi/WriteText.hpp"
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "Copyright.hpp"

/*!
 * \brief Recursively print a nested exception to stderr.
 * \param exception Possibly nested exception to be printed.
 * \param level Exception nest level used for indentation.
 */
void printException(const std::exception &exception, unsigned int level = 0) {
  std::cerr << std::string(level * 2, ' ') << exception.what() << std::endl;
  try {
    std::rethrow_if_nested(exception);
  } catch (const std::exception &e) {
    printException(e, level + 1);
  } catch (...) {
  }
}

/*!
 * \brief ViFi main function, argument parsing and process flow.
 */
int main(int argc, char *argv[]) {
  enum Status { Ok = 0, Cancel = 1, InputError = 1, Failure = -1 };

  // Read arguments.
  std::vector<std::string> arguments(argc);
  for (int i = 0; i < argc; ++i) {
    arguments[i] = argv[i];
  }

  if (arguments.size() >= 2) {
    // Scan a directory and write its content to a ViFi text file.
    if (arguments.at(1) == "scan" && arguments.size() == 4) {
      try {
        FileTree tree;
        ScanDirectory::scan(arguments.at(2), tree);
        WriteText::write(tree, arguments.at(3));
      } catch (const std::exception &e) {
        printException(e);
        return Failure;
      }
    }
    // Interprete changes between two ViFi text files as file operations.
    if (arguments.at(1) == "move" && arguments.size() == 4) {
      try {
        // Read original and target tree from text files.
        FileTree tree;
        fs::path current(arguments.at(2));
        ReadText::read(current, tree);
        tree.endOriginal();
        fs::path changed(arguments.at(3));
        ReadText::read(changed, tree);
        // Generate file operations.
        FileOpRunner operations(current.parent_path());
        tree.endTarget();
        tree.generate(operations);
        operations.prepare();
        // Prompt user for executing file operations.
        if (operations.empty()) {
          std::cout << "No changes detected." << std::endl;
          operations.finish();
        } else {
          operations.print(tree.basePath());
          std::cout << "Do you want to execute operations? [y|n]";
          while (true) {
            int c = std::getc(stdin);
            if (c == 'y' || c == 'Y') {
              std::cout << "Executing operations..." << std::endl;
              operations.run();
              operations.finish();
              std::cout << "Done." << std::endl;
              return Ok;
            } else if (c == 'n' || c == 'N') {
              std::cout << "Cancel." << std::endl;
              return Cancel;
            } else {
              std::cout << "Type 'y' for yes (proceed), 'n' for no (cancel)."
                        << std::endl;
            }
          }
        }
      } catch (const fs::filesystem_error &e) {
        printException(e);
        return Failure;
      } catch (const std::exception &e) {
        printException(e);
        return InputError;
      }
    }
    // Print copyright notice.
    if (arguments.at(1) == "copyright") {
      std::cout << copyright;
    }
  }

  return Ok;
}
