#include "node.h"

Node::Node(Socket &socket)
	:	connection(socket)
{

}

int Node::getFd()
{
	return connection.fileHandle;
}
