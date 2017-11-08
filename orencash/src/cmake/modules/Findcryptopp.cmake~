find_path( cryptopp_INCLUDE sha.h HINTS "${PROJECT_SOURCE_DIR}/dependencies/cryptopp" )

if ( cryptopp_INCLUDE )
    set( CRYPTOPP_FOUND TRUE )

    message( STATUS "${Green}Found cryptopp include at: ${cryptopp_INCLUDE}${Reset}" )
else ( )
    message( FATAL_ERROR "${Red}Failed to locate cryptopp dependency.${Reset}" )
endif ( )
