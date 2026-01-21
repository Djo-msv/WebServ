#include <fstream>
#include <iostream>
#include <cstring>

int main()
{
	try {
		if (getenv("username") == NULL)
			throw std::out_of_range("5"); //bad request
		std::string name = getenv("username");
		if (getenv("password") == NULL)
			throw std::out_of_range("5"); //bad request
		std::string pwd = getenv("password");
		std::string path = "./logins/" + name;
		std::ifstream user(path.c_str(), std::ios::in);
		if (user.is_open())
		{
			user >> path;
			user.close();
			if (path != pwd)
				std::cout << "retry.html";
			else
				std::cout << "success.html";
			return 0;
		}
		std::ofstream n_user(path.c_str(), std::ios::out);
		if (!n_user.is_open())
			throw std::out_of_range("4"); //server error
		n_user << pwd;
		n_user.close();
		std::cout << "success.html";
	}
	catch (std::exception &e)
	{
		std::cout << e.what();
	}
}
