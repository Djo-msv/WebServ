#include <request_utils.hpp>

//checks if file exists and if we have read permission
std::string	seekFile(std::string &pathfile)
{
	struct stat file_stat;
	if (stat(pathfile.c_str(), &file_stat) == -1)
		throw FileNotFound();
	
	if (file_stat.st_mode & S_IRUSR)
		return (pathfile);
	throw Forbidden();

}

//returns the correct cgi executable for the file type (for example, file == "input.py" will return "usr/bin/python3"
std::string	extractCgi(std::string &file, ServerConfig &config)
{
	std::string extension;
	size_t dotPos = file.find_last_of('.');
	if (dotPos != std::string::npos)
		extension = file.substr(dotPos);
	try {
		return config.getCgi(extension);
	}
	catch (const std::exception &e) { throw ; }
}


/**
 * Renvoie le chemin du fichier d'erreur à afficher, en cherchant d'abord dans la config du server, puis dans les fichiers d'erreur par défaut
 * Si aucun fichier d'erreur n'est trouvé, une InternalServerError est renvoyé
*/
std::string	seekErrorFile(HttpError error, ServerConfig &config)
{
    std::string pathfile;
    try {
        pathfile = config.getErrorFile(error.getErrorCode());
        return seekFile(pathfile);
    }
    catch(const HttpError &e) {
        if (error.getErrorCode() == 500)
            throw InternalServerError();
        pathfile = error.getDefaultFile();
        try {  return seekFile(pathfile); }
        catch(const HttpError& e) {
            throw InternalServerError();
        }
    }
}

//a simple check for hexadecimal numbers (relevant to chunk_parse)
static bool check_hex(ustring hex)
{
	if (hex.empty())
		return false;
	for (ustring::iterator it = hex.begin(); it != hex.end(); it++)
	{
		*it = std::toupper((char)*it);
		if (!isdigit(*it) && (*it < 'A' || *it > 'F'))
			return false;
	}
	return true;
}
//parsing of chunked body (ex :: hex+\r\n+chunk+...+0\r\n\r\n)
ustring chunk_parse(ustring &_body, ustring &new_body)
{
	if (_body.empty())
		throw Request::MissingData();
	unsigned int size = 0;
	int count = 0;
	
	if (_body.find((unsigned char *)"\r\n") == 0)
		_body = _body.substr(2);
	while (!_body.empty())
	{
		if (count > 7) { throw Request::ChunkParsing(); }
		if (_body.find((unsigned char *)"\r\n") == 0)
			_body = _body.substr(2);
		std::size_t pos = _body.find((unsigned char *)"\r\n");
		if (pos == ustring::npos)
			throw Request::MissingData();
		ustring hex = _body.substr(0, pos);
		_body = _body.substr(pos + 2);
		if (!check_hex(hex))
			throw BadRequest();
		sscanf((char *)hex.c_str(), "%x", &size);
		if (!size)
			break ;
		if (size > _body.length())
			throw Request::MissingData();
		new_body += _body.substr(0, size);
		_body = _body.substr(size);
		count++;
	}
	if (size)
		throw Request::MissingData();
	if (_body.length() < 2 || _body.substr(0, 2) != (unsigned char *)"\r\n")
		throw Request::MissingData();
	return new_body;
}

//checks a key for alnum (-)
bool check_key(std::string key) //wip, as im actually unsure what the authorized formatting is
{
	for (std::string::iterator it = key.begin(); it != key.end(); it++)
	{
		if (!isalpha(*it) && *it != '-')
			return false;
	}
	return true;
}

//trims the whitespaces
bool check_val(std::string &val)
{
	size_t pos1 = 0;
	for (std::string::iterator it = val.begin(); it != val.end(); it++) {
		if (!isspace(*it))
			break ;
		pos1++;
	}
	if (pos1 == val.length())
		return false;
	val = val.substr(pos1);
	std::string::iterator cut;
	for (std::string::iterator it = val.begin(); it != val.end(); it++) {
		if (!isspace(*it))
			cut = it;
	}
	if (cut != val.end() -1)
		val.erase(cut, val.end());
	return true;
}

std::list<std::string> target_list(std::string loc)
{
	std::list<std::string> full;
	if (loc.empty() || loc.find('/') == std::string::npos) { return full; }
	if (*(loc.rbegin()) == '/' && loc.size() > 1)
		loc.erase(loc.size() - 1);
	if (loc.rfind('/') != std::string::npos && loc.size() > 1) {
		full.push_back(loc.substr(loc.rfind('/')));
		loc = loc.substr(0, loc.rfind('/'));
	}
	while (!loc.empty() && loc.size() > 1 && loc.rfind('/') != std::string::npos) {
		full.push_back(loc.substr(loc.rfind('/')));
		loc = loc.substr(0, loc.rfind('/'));
	}
	for (std::list<std::string>::iterator it = full.begin(); it != full.end(); it++) {
		if ((*it).empty() || (it != full.begin() && *it == "/")) { it = full.erase(it); }
	}
	return full;
}

std::string get_path_info(std::string loc)
{
	std::string path_info;
	std::string last_dir;
	struct stat s;
	size_t pos;
	while ((pos = loc.rfind('/')) != std::string::npos && pos > 1) {
		if (stat(loc.c_str(), &s) == 0) {
			if(last_dir.empty() && s.st_mode & S_IFDIR)
				last_dir = loc;
			else if (s.st_mode & S_IFREG)
				break ;
		}
		else
			path_info = loc.substr(pos) + path_info;
		loc = loc.substr(0, pos);
	}
	return path_info;
}
