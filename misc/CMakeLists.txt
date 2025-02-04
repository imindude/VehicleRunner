##
## @author  imindude@gmail.com
##

set(TARGET misc)

set(
    SOURCES
        Checksum.cpp
)

add_library(
    ${TARGET}
        OBJECT
            ${SOURCES}
)

target_include_directories(
    ${TARGET}
        PUBLIC
            inc
            msg
        PRIVATE
            impl
)

## EOF
