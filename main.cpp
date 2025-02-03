#include <iostream>
#include <string>

#include "node.h"

int main(int argc, char **argv)
{
	if (argc == 2)
	{
		unsigned short port = strtol(argv[1], NULL, 10);

		gnet::Node node(port);
		node.run();
	} else
	{
		std::cout << "Usage: " << argv[0] << " <host port>\n";
		return 0;
	}

	return 0;
}

