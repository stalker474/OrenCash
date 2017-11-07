find_path( sqlite3_INCLUDE sqlite3.h HINTS "${PROJECT_SOURCE_DIR}/dependencies/sqlite3" )

if ( sqlite3_INCLUDE )
    set( SQLITE3_FOUND TRUE )

    message( STATUS "${Green}Found sqlite3 include at: ${sqlite3_INCLUDE}${Reset}" )
else ( )
    message( FATAL_ERROR "${Red}Failed to locate sqlite3 dependency.${Reset}" )
endif ( )
