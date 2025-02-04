##
## @author  imindude@gmail.com
##

set(TARGET plugin)

set(
    SOURCES
        # -- interface
        SmallSwarm.cpp
        # -- implementation
        impl/SmallSwarm_Impl.cpp
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
        PRIVATE
            impl
)

target_link_libraries(
    ${TARGET}
        PUBLIC
            bundle
            misc
)

## EOF
