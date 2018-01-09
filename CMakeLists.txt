cmake_minimum_required(VERSION 3.9)
project(TriangleMeshToClearSolid)

set(CMAKE_CXX_STANDARD 11)

include_directories(../TriangleMeshToClearSolid)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
find_package(CGAL QUIET )

if ( CGAL_FOUND )
    include( ${CGAL_USE_FILE} )
    include( CGAL_CreateSingleSourceCGALProgram )
endif()

FIND_PACKAGE( Boost 1.40 COMPONENTS filesystem REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_executable(TriangleMeshToClearSolid
        features/Graph.h
        features/Segment.cpp
        features/Segment.h
        features/Space.cpp
        features/Space.h
        features/Surface.cpp
        features/Surface.hpp
        features/SurfaceGraph.cpp
        features/SurfaceGraph.h
        features/Triangle.cpp
        features/Triangle.h
        features/TriangleMesh.cpp
        features/TriangleMesh.h
        features/TriangleMeshGraph.cpp
        features/TriangleMeshGraph.h
        features/vertex.cpp
        features/vertex.h
        fileio/3DSImporter.cpp
        fileio/3DSImporter.h
        fileio/GenerationWriter.cpp
        fileio/GenerationWriter.h
        fileio/Importer.h
        fileio/JSONMaker.cpp
        fileio/JSONMaker.h
        fileio/TVRImporter.cpp
        fileio/TVRImporter.h
        logic/CGALCalculation.cpp
        logic/CGALCalculation.h
        logic/check.cpp
        logic/check.hpp
        logic/CleanPolygonMaker.cpp
        logic/CleanPolygonMaker.h
        logic/SurfacesListCalculation.cpp
        logic/SurfacesListCalculation.h
        manage/ClusteringRoomMaker.cpp
        manage/ClusteringRoomMaker.h
        manage/MergingRoomMaker.cpp
        manage/MergingRoomMaker.h
        manage/RoomMaker.cpp
        manage/RoomMaker.h
        space_maker/OnlyWallSpaceMaker.cpp
        space_maker/OnlyWallSpaceMaker.h
        space_maker/SpaceMaker.h
        predefine.h
        TVRConverter.cpp
        util.cpp
        util.h manage/CombineParameter.h)


TARGET_LINK_LIBRARIES( TriangleMeshToClearSolid LINK_PUBLIC ${Boost_LIBRARIES} )