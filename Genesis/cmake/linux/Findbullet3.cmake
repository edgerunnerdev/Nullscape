find_path(BULLET3_INCLUDE_DIRS btBulletCollisionCommon.h HINTS "/usr/include" "/usr/include/bullet")
find_library(BULLET3_LIBRARY_DIRS NAMES Bullet3Common HINTS "/usr/lib" )

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(bullet3 REQUIRED_VARS BULLET3_INCLUDE_DIRS BULLET3_LIBRARY_DIRS)