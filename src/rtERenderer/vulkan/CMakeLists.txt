target_sources(${PROJECT_NAME} PRIVATE
        rtERenderer_VulkanImpl.c)

add_subdirectory(debug)
add_subdirectory(support)
add_subdirectory(creation)
