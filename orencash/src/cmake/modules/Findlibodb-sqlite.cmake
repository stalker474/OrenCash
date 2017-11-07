find_path( libodb-sqlite_INCLUDE sqlite3.h HINTS "${PROJECT_SOURCE_DIR}/dependencies/odb/libodb-sqlite" )

if ( libodb-sqlite_INCLUDE )
    set( LIBODB-SQLITE_FOUND TRUE )

    message( STATUS "${Green}Found libodb-sqlite include at: ${libodb-sqlite_INCLUDE}${Reset}" )
else ( )
    message( FATAL_ERROR "${Red}Failed to locate libodb-sqlite dependency.${Reset}" )
endif ( )
