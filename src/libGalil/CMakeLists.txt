find_package( libgclib QUIET)
find_package( ZLIB )

if ( ${LIBGCLIB_FOUND} )
  # Add the headers
  include_directories( ./ )

  # Add all *.c* files as source code
  file(GLOB SrcFiles *.c*)
  add_library(Galil SHARED ${SrcFiles})
  if(APPLE)
    target_link_libraries(Galil /Applications/gclib/dylib/gclib.0.dylib /Applications/gclib/dylib/gclibo.0.dylib)
  else()
    target_link_libraries(Galil gclib gclibo ${ZLIB_LIBRARIES} )
  endif()

  # Tell rest of labRemote that the library exists
  set(libGalil_FOUND TRUE PARENT_SCOPE)
else()
  message("Disabling libGalil due to missing dependencies")
  message(" libgclib = ${LIBGCLIB_FOUND}")

  set(libGalil_FOUND FALSE PARENT_SCOPE)
endif()
