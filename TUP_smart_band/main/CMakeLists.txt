file(GLOB UI_SOURCES "ui/*.c")

idf_component_register(
    SRCS "main_entrance.c" "qmi8658c.c" ${UI_SOURCES}
    INCLUDE_DIRS "."
)
