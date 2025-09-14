file(GLOB UI_SOURCES "ui/*.c")
file(GLOB SOURCES "./*.c")
idf_component_register(
    SRCS ${SOURCES} ${UI_SOURCES}
    INCLUDE_DIRS "."
)
