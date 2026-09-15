function( dbs_copy_target_dependencies target_name )
    #[[
        For Windows builds especially, copy the runtime DLLs of a given target
        into the executable target's build directory. The original locations of
        the required runtime DLLs are not automatically added to the PATH, so
        they need to be copied into the executable's build directory for easy
        discovery. This allows the programs to actually run.

        PARAMETERS:
            target_name The target whose dependencies are to be copied.
    ]]
    add_custom_command( TARGET ${target_name}
        POST_BUILD
        VERBATIM
        COMMENT "Copying ${target_name} dependencies"
        COMMAND ${CMAKE_COMMAND} -E copy -t ${CMAKE_CURRENT_BINARY_DIR} $<TARGET_RUNTIME_DLLS:${target_name}>
        COMMAND_EXPAND_LISTS
    )
endfunction()