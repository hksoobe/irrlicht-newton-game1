add_library(Irrlicht STATIC IMPORTED)
find_library(IRRLICHT_LIBRARY_PATH Irrlicht HINTS "${CMAKE_CURRENT_LIST_DIR}/")
set_target_properties(Irrlicht PROPERTIES IMPORTED_LOCATION "${IRRLICHT_LIBRARY_PATH}")