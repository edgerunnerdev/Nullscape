include(ExternalProject)

function(BuildExternalLibraries)

  ExternalProject_Add(MagicEnum
    PREFIX ${CMAKE_CURRENT_LIST_DIR}/ext/MagicEnum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
	GIT_TAG v0.8.0
    UPDATE_COMMAND "" CONFIGURE_COMMAND "" BUILD_COMMAND "" INSTALL_COMMAND ""
  )

  ExternalProject_Add(xxhash
    PREFIX ${CMAKE_CURRENT_LIST_DIR}/ext/xxhash
    GIT_REPOSITORY https://github.com/stbrumme/xxhash.git
	GIT_TAG xxhash_v2
    UPDATE_COMMAND "" CONFIGURE_COMMAND "" BUILD_COMMAND "" INSTALL_COMMAND ""
  )

endfunction()