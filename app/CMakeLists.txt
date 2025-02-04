##
## @author  imindude@gmail.com
##

set(TARGET app)

set(
    SOURCES
        AppMain.cpp
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
)

target_link_libraries(
    ${TARGET}
        PUBLIC
            bundle
            plugin
)

## EOF
