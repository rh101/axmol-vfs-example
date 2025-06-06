if(ANDROID)

  list(APPEND GAME_SOURCE
    ${CMAKE_CURRENT_LIST_DIR}/android/VfsFileUtils.cpp
  )
  list(APPEND GAME_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/android/VfsFileUtils.h
  )

elseif(WINDOWS)

  list(APPEND GAME_SOURCE
    ${CMAKE_CURRENT_LIST_DIR}/win32/VfsFileUtils.cpp
  )
  list(APPEND GAME_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/win32/VfsFileUtils.h
  )

endif()