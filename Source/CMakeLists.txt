include(${CMAKE_CURRENT_LIST_DIR}/platform/CMakeLists.txt)

list(APPEND GAME_SOURCE
	${CMAKE_CURRENT_LIST_DIR}/AppDelegate.cpp
	${CMAKE_CURRENT_LIST_DIR}/MainScene.cpp
	${CMAKE_CURRENT_LIST_DIR}/PhysFsFileStream.cpp
	${CMAKE_CURRENT_LIST_DIR}/Utility.cpp
)
	 
list(APPEND GAME_HEADER
	${CMAKE_CURRENT_LIST_DIR}/AppDelegate.h
	${CMAKE_CURRENT_LIST_DIR}/MainScene.h
	${CMAKE_CURRENT_LIST_DIR}/PhysFsFileStream.h
	${CMAKE_CURRENT_LIST_DIR}/Utility.h
)
