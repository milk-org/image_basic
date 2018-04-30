project(lib_image_basic_project)

include_directories ("${PROJECT_SOURCE_DIR}/src")
include_directories ("${PROJECT_SOURCE_DIR}/..")
add_library(image_basic SHARED image_basic.c)

install(TARGETS image_basic DESTINATION lib)
install(FILES image_basic.h DESTINATION include)

