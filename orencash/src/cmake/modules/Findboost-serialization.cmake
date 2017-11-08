find_path( boost-serialization_INCLUDE boost/serialization/serialization.hpp HINTS "/usr/include" )
if ( boost-serialization_INCLUDE )
    set( BOOST-SERIALIZATION_FOUND TRUE )

    message( STATUS "${Green}Found Boost-Serialization include at: ${boost-serialization_INCLUDE}${Reset}" )
else ( )
    message( FATAL_ERROR "${Red}Failed to locate Boost-Serialization dependency. ${Reset}" )
endif ( )
