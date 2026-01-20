#include "Request.hpp"

const char **make_env(std::string params)
{
	std::vector<std::string> tab1;
	std::stringstream s(params);
	std::string param;
	while (!s.eof())
	{
		getline(s, param, '&');
		tab1.push_back(param);
	}
	const char **tab = new const char*[tab1.size() +1];
	size_t i = 0;
	for (std::vector<std::string>::iterator it = tab1.begin(); it != tab1.end(); it++)
	{
		tab[i] = it->c_str();
		i++;
	}
	tab[i] = NULL;
	return tab;
}
