#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "sfml-system" for configuration "RelWithDebInfo"
set_property(TARGET sfml-system APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(sfml-system PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libsfml-system.so.2.6.1"
  IMPORTED_SONAME_RELWITHDEBINFO "libsfml-system.so.2.6"
  )

list(APPEND _cmake_import_check_targets sfml-system )
list(APPEND _cmake_import_check_files_for_sfml-system "${_IMPORT_PREFIX}/lib/libsfml-system.so.2.6.1" )

# Import target "sfml-window" for configuration "RelWithDebInfo"
set_property(TARGET sfml-window APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(sfml-window PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libsfml-window.so.2.6.1"
  IMPORTED_SONAME_RELWITHDEBINFO "libsfml-window.so.2.6"
  )

list(APPEND _cmake_import_check_targets sfml-window )
list(APPEND _cmake_import_check_files_for_sfml-window "${_IMPORT_PREFIX}/lib/libsfml-window.so.2.6.1" )

# Import target "sfml-graphics" for configuration "RelWithDebInfo"
set_property(TARGET sfml-graphics APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(sfml-graphics PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libsfml-graphics.so.2.6.1"
  IMPORTED_SONAME_RELWITHDEBINFO "libsfml-graphics.so.2.6"
  )

list(APPEND _cmake_import_check_targets sfml-graphics )
list(APPEND _cmake_import_check_files_for_sfml-graphics "${_IMPORT_PREFIX}/lib/libsfml-graphics.so.2.6.1" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
