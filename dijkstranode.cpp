#include "dijkstranode.h"
#include <string>

DijkstraNode::DijkstraNode()
    : row(0),
      col(0),
      parent(NULL),
      givenCost(0),
      iteration(0),
      dirFromStart(0),
      dirFromParent(0),
      listStatus(ListStatus::OnNone)
{
}
