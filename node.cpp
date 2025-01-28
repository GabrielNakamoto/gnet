#include "node.h"

Node::Node(Address &address)
	:	connection(address)
{
}

int Node::getFd()
{
	return connection.fileHandle;
}
