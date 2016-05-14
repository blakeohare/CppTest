#ifndef RESOLVER_H
#define RESOLVER_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "../parser/nodes.h"

using namespace std;
using namespace Nodes;

namespace Resolver
{
	void resolveCode(vector<Executable*> code);
}

#endif
