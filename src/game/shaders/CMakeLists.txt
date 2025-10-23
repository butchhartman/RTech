
function(compile_shaders path)

        file(GLOB_RECURSE SHADER_FILES
                ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/*.vert
                ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/*.frag
        )

        message("SHADERFILES: ${SHADER_FILES}")

        add_custom_target(shader_output_dir ALL
                COMMAND ${CMAKE_COMMAND} -E make_directory ${path}
                COMMENT "Creating shader binary directory"
                )

       foreach(CURRENT_SHADER_FILE IN ITEMS ${SHADER_FILES})
       message("meow")
               get_filename_component(SHADER_FILE_NAME_RAW ${CURRENT_SHADER_FILE} NAME_WLE)
               get_filename_component(SHADER_FILE_NAME_EXT ${CURRENT_SHADER_FILE} EXT)

               add_custom_command(
                       OUTPUT "${SHADER_FILE_NAME_RAW}${SHADER_FILE_NAME_EXT}.spv"
                       DEPENDS shader_output_dir
                       COMMAND glslangValidator -o "${path}/${SHADER_FILE_NAME_RAW}${SHADER_FILE_NAME_EXT}.spv" -V "${CURRENT_SHADER_FILE}"
                       COMMENT "Compiling shader ${SHADER_FILE_NAME_RAW}${SHADER_FILE_NAME_EXT}"
               )

               add_custom_target(
                       "${SHADER_FILE_NAME_RAW}${SHADER_FILE_NAME_EXT}"
                       DEPENDS "${SHADER_FILE_NAME_RAW}${SHADER_FILE_NAME_EXT}.spv"
                       COMMENT "GRAAHH"
                       )

               add_dependencies(${PROJECT_NAME} "${SHADER_FILE_NAME_RAW}${SHADER_FILE_NAME_EXT}")
                
       endforeach()

endfunction()
