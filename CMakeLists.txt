cmake_minimum_required (VERSION 2.9)
project (lorminator_dash)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

enable_testing()


include_directories(
    include/
    test/unit-tests/Catch2/single_include/catch2
    test/unit-tests/trompeloeil/include
	${SDL2_INCLUDE_DIR}
	${SDL2_IMAGE_INCLUDE_DIRS}
)

add_executable(lorminator_dash
	src/gui/io.cc
	src/gui/resource-store.cc
	src/animator.cc
	src/behavior.cc
	src/entity.cc
	src/entity-properties.cc
	src/game.cc
	src/level.cc
	src/level-animator.cc
	src/lightning.cc
	src/main.cc
	src/observer.cc
	src/utils.cc
)
set_target_properties(lorminator_dash PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF)
target_link_libraries(lorminator_dash
	${SDL2_LIBRARY}
	${SDL2_IMAGE_LIBRARIES}
)


add_executable(ut
	src/animator.cc
	src/behavior.cc
	src/entity.cc
	src/entity-properties.cc
	src/game.cc
	src/level.cc
	src/level-animator.cc
	src/lightning.cc
	src/observer.cc
	src/utils.cc
	test/unit-tests/mock-input.cc
	test/unit-tests/mock-io.cc
	test/unit-tests/mock-resource-store.cc
	test/unit-tests/tests-main.cc
	test/unit-tests/tests-behavior.cc
	test/unit-tests/tests-entity.cc
	test/unit-tests/tests-game.cc
	test/unit-tests/tests-level.cc
	test/unit-tests/tests-lightning.cc
	test/unit-tests/tests-observer.cc
	test/unit-tests/tests-player.cc
)
set_target_properties(ut PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF)

add_test(NAME unittest
	COMMAND kcov --include-pattern=lorminator_dash --exclude-pattern=Catch2,trompeloeil --configure=lldb-use-raw-breakpoint-writes=1 kcov-output  ./ut
)
