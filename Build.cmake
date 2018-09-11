# Default compiler flags.
set(BUILD_SANITIZE_FLAGS "")
set(BUILD_WARNING_FLAGS "-Wall -Wextra -Wpedantic")

# Optional: Extensive compiler warnings for QA.
option(BUILD_EXTENSIVE_WARNINGS
  "Add extensive compiler warnings to the build."
  OFF
)

if (BUILD_EXTENSIVE_WARNINGS)
  set(BUILD_WARNING_FLAGS
    "${BUILD_WARNING_FLAGS} -Weverything \
                            -Wno-c++98-compat \
                            -Wno-c++98-compat-pedantic \
                            -Wno-c++98-c++11-compat"
  )
endif (BUILD_EXTENSIVE_WARNINGS)

# Optional: Clang Tidy for QA, hard-coded to clang 7.0 for now.
option(BUILD_CLANG_TIDY70
  "Add static checks and lint to build using clang-tidy70."
  OFF
)

if (BUILD_CLANG_TIDY70)
  set(CMAKE_CXX_CLANG_TIDY clang-tidy70
    "-checks=bugprone*,\
             clang-analyzer*,\
             misc*,\
             modernize*,\
             performance*,\
             portability*,\
             readability*,\
             -readability-else-after-return,\
             -readability-implicit-bool-conversion"
  )
endif (BUILD_CLANG_TIDY70)

# Combined compiler flags for convenience.
set(BUILD_COMPILER_FLAGS "${BUILD_WARNING_FLAGS} ${BUILD_SANITIZE_FLAGS}")
