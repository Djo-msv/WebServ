#include "Request.hpp"

int main()
{
	try {
		Request obj("GET /index.html?key=param&key2=param2 HTTP/1.1\r\nhere some irrelevant words that shouldnt appear in test\n\n");
		obj.check_request();
		obj.read();
		std::cout << "converting the _env into a char **tab \n";
		const char **tab = obj.getEnv();
		for (size_t i = 0; tab[i] != NULL; i++)
			std::cout << tab[i] << ";";
		std::cout << std::endl;
	}
	catch (std::exception &e)
	{
		std::cerr << "error somewhere :: " << e.what();
	}
}
