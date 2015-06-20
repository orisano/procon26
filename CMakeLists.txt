cmake_minimum_required(VERSION 3.1)
project(2015)

set(CMAKE_CXX_COMPILER g++-5)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -O3 -msse -Wall -ltcmalloc")

set(BOARD_TILE
    Board.cpp
    Board.hpp
    Tile.cpp
    Tile.hpp
    Square.hpp)

set(BIT_BOARD_TILE
    ${BOARD_TILE}
    BitTile.cpp
    BitTile.hpp
    BitBoard.cpp
    BitBoard.hpp)

set(CORE_FILES
    Action.hpp
    Answer.cpp
    Answer.hpp
    Home.hpp
    Takumi/Takumi.hpp
    Takumi/ZobristHash.hpp)


set(BEAM_SOURCE_FILES
    ${CORE_FILES}
    ${BOARD_TILE}
    Takumi/Beam.cpp
    Takumi/Beam.hpp
    CacheTile.hpp
    BeamMain.cpp)

set(BIT_BEAM_SOURCE_FILES
    ${CORE_FILES}
    ${BIT_BOARD_TILE}
    Takumi/BitBeam.cpp
    Takumi/BitBeam.hpp
    CacheTile.hpp
    BitBeamMain.cpp)

set(GET_ANS_SOURCE_FILES
    ${CORE_TILES}
    ${BOARD_TILE}
    CacheTile.hpp
    getAnswer.cpp)

set(BIT_BOARD_TEST_FILES
    ${BIT_BOARD_TILE}
    Test/BitBoardTest.cpp)

add_executable(Beam ${BEAM_SOURCE_FILES})
add_executable(getAns ${GET_ANS_SOURCE_FILES})
add_executable(BitBeam ${BIT_BEAM_SOURCE_FILES})
add_executable(BitBoardTest ${BIT_BOARD_TEST_FILES})