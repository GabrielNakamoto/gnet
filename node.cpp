#include "node.h"

Node::Node()
{
}

int Node::getFd()
{
	return connection.fileHandle;
}
