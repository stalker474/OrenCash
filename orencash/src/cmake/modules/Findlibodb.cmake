find_path( libodb_INCLUDE sqlite3.h HINTS "${PROJECT_SOURCE_DIR}/dependencies/odb/libodb" )

if ( libodb_INCLUDE )
    set( LIBODB_FOUND TRUE )

    message( STATUS "${Green}Found libodb include at: ${libodb_INCLUDE}${Reset}" )
else ( )
    message( FATAL_ERROR "${Red}Failed to locate libodb dependency.${Reset}" )
endif ( )
