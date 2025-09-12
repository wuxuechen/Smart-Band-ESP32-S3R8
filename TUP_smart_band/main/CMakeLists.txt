file(GLOB UI_SOURCES "ui/*.c")

idf_component_register(
    SRCS "main_entrance.c" ${UI_SOURCES}
    INCLUDE_DIRS "."
)
