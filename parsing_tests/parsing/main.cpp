#include "Request.hpp"

int main()
{
	try {
		Request obj("GET html/index.html?key=param&key2=param2 HTTP/1.1\r\nUser-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:146.0) Gecko/20100101 Firefox/146.0\r\nAccept-Language: en-US,en;q=0.5\r\n\r\n");
		obj.check_request();
		obj.read();
		std::cout << "and now see get size : " << obj.getSize() << std::endl;
	}
	catch (std::exception &e)
	{
		std::cerr << "error somewhere :: " << e.what();
	}
}
//Content-Length: 489\r\n
//Transfer-Encoding: chunked\r\n
