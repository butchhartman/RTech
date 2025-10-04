target_sources(${PROJECT_NAME} 
        PRIVATE 
        rtEMemoryManager.c
        rtEStackAllocator.c
        )

add_subdirectory(procs)
