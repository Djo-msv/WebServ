//#include "Response.hpp"
#include "Request.hpp"

int main()
{
	try {
		Request obj;
		obj += "GET /scripts/login?username=param&password=param HTTP/1.1\r\n";
		obj += "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:146.0) Gecko/20100101 Firefox/146.0\r\n";
		obj += "Accept-Language: en-US,en;q=0.5\r\n";
		obj += "Content-Length: 4\r\n";
		obj += "\r\n";
		obj += "here";
		std::cout << std::endl;
		obj.parse();
		obj.read();
		std::cout << "and now see get size : " << obj.getSize() << std::endl;
		/*Response res;
		res.makeResponse(&obj);
		std::cout << std::endl << std::endl << res.getResponse() << std::endl;*/
	}
	catch (std::exception &e)
	{
		std::cerr << "error somewhere :: " << e.what() << std::endl;
	}
}
//Content-Length: 489\r\n
//Transfer-Encoding: chunked\r\n
