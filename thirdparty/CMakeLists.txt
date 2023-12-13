include(FetchContent)
FetchContent_Declare(
  physfs
  GIT_REPOSITORY https://github.com/icculus/physfs.git
  GIT_TAG        7931b9b3f4ca3b0685fe35f105a3e5c2a1e0d1ed
  SOURCE_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/physfs/_d
)
set(PHYSFS_BUILD_SHARED OFF CACHE INTERNAL "")
set(PHYSFS_BUILD_TEST OFF CACHE INTERNAL "")
set(PHYSFS_DISABLE_INSTALL ON CACHE INTERNAL "")
FetchContent_MakeAvailable(physfs)