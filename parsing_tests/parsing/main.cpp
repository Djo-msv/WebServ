#include "Request.hpp"

int main()
{
	try {
		Request obj("GET /index.html?key=param&key2=param2 HTTP/1.1\r\nhere some irrelevant words that shouldnt appear in test\n\n");
		//obj.read();
	}
	catch (std::exception &e)
	{
		std::cerr << "error somewhere :: " << e.what();
	}
}
