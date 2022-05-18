#pragma once

#include <memory>
#include <list>

namespace Hexterminate
{

class Fleet;

typedef std::shared_ptr< Fleet > FleetSharedPtr;
typedef std::unique_ptr< Fleet > FleetUniquePtr;
typedef std::weak_ptr< Fleet > FleetWeakPtr;
typedef std::list< FleetSharedPtr > FleetList;
typedef std::list< FleetWeakPtr> FleetWeakPtrList;

}