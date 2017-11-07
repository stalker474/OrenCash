find_path( restbed_INCLUDE restbed HINTS "${PROJECT_SOURCE_DIR}/dependencies/restbed/source" )

if ( restbed_INCLUDE )
    set( RESTBED_FOUND TRUE )

    message( STATUS "${Green}Found restbed include at: ${restbed_INCLUDE}${Reset}" )
else ( )
    message( FATAL_ERROR "${Red}Failed to locate restbed dependency.${Reset}" )
endif ( )
