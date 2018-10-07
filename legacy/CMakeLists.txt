set (CMAKE_C_FLAGS "-g")

include_directories(
    include
    lib/include
	${SDL2_INCLUDE_DIR}
)

add_executable(legacy
    callbacks.c
    elem.c
    file_handling.c
    highscore.c
    include/boulder.h
    include/dir.h
    include/file_handling.h
    include/levels.h
    include/point.h
    include/specials.h
    include/tiles.h
    include/types.h
    level_handling.c
    levels.c
#    lib/include/menu.h
#    lib/include/utils.h
#    lib/menu/include/menu.h
#    lib/menu/menu.c
    lib/utils/include/utils.h
    lib/utils/utils.c
    main.c
    res.txt
    sound.c
    specials.c
    sprite.c
    status_field.c
)

target_link_libraries(legacy ${SDL2_LIBRARY})
