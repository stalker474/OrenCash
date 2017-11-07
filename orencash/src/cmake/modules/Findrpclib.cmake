find_path( rpclib_INCLUDE rpc/client.h HINTS "${PROJECT_SOURCE_DIR}/dependencies/rpclib/include" )

if ( rpclib_INCLUDE )
    set( RPCLIB_FOUND TRUE )

    message( STATUS "${Green}Found rpclib include at: ${rpclib_INCLUDE}${Reset}" )
else ( )
    message( FATAL_ERROR "${Red}Failed to locate rpclib dependency.${Reset}" )
endif ( )
