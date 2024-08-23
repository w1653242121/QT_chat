#include <iostream>
#include "chat_server.h"

using namespace std;

int main()
{
	ChatServer s;
	s.listen(IP, PORT);

	return 0;
}
