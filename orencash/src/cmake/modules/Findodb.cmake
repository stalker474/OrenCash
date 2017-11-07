find_path( odb_INCLUDE sqlite3.h HINTS "${PROJECT_SOURCE_DIR}/dependencies/odb/odb" )

if ( ODB_INCLUDE )
    set( ODB_FOUND TRUE )

    message( STATUS "${Green}Found odb include at: ${odb_INCLUDE}${Reset}" )
else ( )
    message( FATAL_ERROR "${Red}Failed to locate odb dependency.${Reset}" )
endif ( )
