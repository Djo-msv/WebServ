#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <exception>

//strictement un replace du premier n1 par n2 dans la string s
std::string switch_replace(std::string s, std::string n1, std::string n2)
{
	std::string tmp = s.substr((s.find(n1) + n1.length()), (s.length() - (s.find(n1) + n1.length())));
	std::string res = s.substr(0, s.find(n1)) + n2 + tmp;
	return res;
}

//prend le nom d'un fichier, le renvoie sous forme de string (ici, pour mes .html à mettre dans le body de la réponse au client)
std::string file_to_string(std::string filename)
{
	std::ifstream file(filename.c_str());
	if (!file.is_open())
		throw std::out_of_range("couldn't open file " + filename);
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	return buffer.str();
}

//prends un body, renvoie une request toute faite (basée sur la réponse de base)
std::string make_response(std::string body)
{
	std::string res = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nContent-Length: ";
	res += std::to_string(body.length());
	res += "\r\n\r\n";
	res += body;
	return res;
}

//dans le POST /login.html du client, je récupère les infos du <form/> sous forme "username=value&password=value"
//extraction de ces infos
std::string value_extract(std::string request, std::string key)
{
	std::stringstream s(request);
	std::string line;
	getline(s, line);
	while (line.find(key + "=") == std::string::npos && !s.eof())
		getline(s, line);
	if (line.find(key + "=") == std::string::npos)
		throw std::out_of_range("couldn't find the value to key " + key);
	size_t pos = line.find(key) + key.length() + 1;
	size_t len = line.find("&");
	if (len == std::string::npos || len <= pos)
		len = line.length();
	std::string res = line.substr(pos, (len - pos));
	return res;
}

//vérifications de la paire donnée contre le fichier de logins
bool check_logins(std::string filename, std::string key, std::string value)
{
	std::string file = file_to_string(filename);
	if (file.find(key) != std::string::npos)
	{
		size_t pos = file.find(key) + key.length() + 1;
		file = file.substr(pos, value.length());
		if (file.compare(value))
			return false;
		return true;
	}
	std::ofstream files(filename.c_str(), std::ios::app);
	files << key << " " << value << std::endl;
	files.close();
	return true;
}
