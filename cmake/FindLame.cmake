include(CheckLibraryExists)

find_path(LAME_INCLUDE_DIR lame/lame.h)
# Note, enforcing static linking to libmp3lame on purpose:
find_library(LAME_LIBRARIES NAMES libmp3lame.a)

if(LAME_INCLUDE_DIR AND LAME_LIBRARIES)
    set(LAME_FOUND TRUE)
endif(LAME_INCLUDE_DIR AND LAME_LIBRARIES)

if(LAME_FOUND)
    message(STATUS "Found lame includes: ${LAME_INCLUDE_DIR}/lame/lame.h")
    message(STATUS "Found lame library: ${LAME_LIBRARIES}")

    get_filename_component(LAME_LIBRARY_PATH ${LAME_LIBRARIES} DIRECTORY)

    # lame lacks pkgconfig support, check for existence of
    # lame_get_maximum_number_of_samples() instead:
    CHECK_LIBRARY_EXISTS("mp3lame" "lame_get_maximum_number_of_samples" "${LAME_LIBRARY_PATH}" LAME_GET_MAXIMUM_NUMBER_OF_SAMPLES)

    if(NOT LAME_GET_MAXIMUM_NUMBER_OF_SAMPLES)
        message(FATAL_ERROR "Could not find lame version 3.100")
    endif()
else(LAME_FOUND)
    if(Lame_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find lame development files")
    endif(Lame_FIND_REQUIRED)
endif(LAME_FOUND)
