find_path( ed25519_INCLUDE ed25519.h HINTS "${PROJECT_SOURCE_DIR}/dependencies/ed25519/src" )

if ( ed25519_INCLUDE )
    set( ED25519_FOUND TRUE )

    message( STATUS "${Green}Found ed25519 include at: ${ed25519_INCLUDE}${Reset}" )

set( ED25519_INCLUDE
    ${ed25519_INCLUDE}/sha512.h
    ${ed25519_INCLUDE}/sc.h
    ${ed25519_INCLUDE}/precomp_data.h
    ${ed25519_INCLUDE}/ge.h
    ${ed25519_INCLUDE}/fixedint.h
    ${ed25519_INCLUDE}/fe.h
    ${ed25519_INCLUDE}/ed25519.h
    ${ed25519_INCLUDE}/verify.c
    ${ed25519_INCLUDE}/sign.c
    ${ed25519_INCLUDE}/sha512.c
    ${ed25519_INCLUDE}/seed.c
    ${ed25519_INCLUDE}/sc.c
    ${ed25519_INCLUDE}/keypair.c
    ${ed25519_INCLUDE}/key_exchange.c
    ${ed25519_INCLUDE}/ge.c
    ${ed25519_INCLUDE}/fe.c
    ${ed25519_INCLUDE}/add_scalar.c
)
else ( )
    message( FATAL_ERROR "${Red}Failed to locate ed25519 dependency.${Reset}" )
endif ( )
