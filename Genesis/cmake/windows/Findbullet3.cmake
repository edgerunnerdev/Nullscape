set(BULLET3_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/Genesis/libs/bullet3-2.87/include/bullet" CACHE STRING "Bullet3 - include directory")
set(BULLET3_LIBRARY_DIRS "${CMAKE_SOURCE_DIR}/Genesis/libs/bullet3-2.87/lib" CACHE STRING "Bullet3 - library directory")
set(BULLET3_LIBRARIES 
	Bullet3Common_$<$<CONFIG:Debug>:Debug>$<$<CONFIG:RelWithDebInfo>:RelWithDebugInfo>
	BulletDynamics_$<$<CONFIG:Debug>:Debug>$<$<CONFIG:RelWithDebInfo>:RelWithDebugInfo>
	BulletCollision_$<$<CONFIG:Debug>:Debug>$<$<CONFIG:RelWithDebInfo>:RelWithDebugInfo>
	LinearMath_$<$<CONFIG:Debug>:Debug>$<$<CONFIG:RelWithDebInfo>:RelWithDebugInfo>
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(bullet3 REQUIRED_VARS BULLET3_INCLUDE_DIRS BULLET3_LIBRARY_DIRS)