#include <request_utils.hpp>

//checks if file exists and if we have read permission (GET) or write permission (POST)
std::string	seekFile(std::string &pathfile, bool post)
{
	struct stat file_stat;
	if (stat(pathfile.c_str(), &file_stat) == -1)
		throw FileNotFound();
	
	if (!post && file_stat.st_mode & S_IRUSR)
		return pathfile;
	if (post && (file_stat.st_mode & S_IWUSR)) {
		if (!(file_stat.st_mode & S_IFREG))
			throw NotImplemented();
		return pathfile;
	}
	throw Forbidden();

}

//returns the correct cgi executable for the file type (for example, "input.py" will return "usr/bin/python3")
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
 * Looks for config error files first, then default error files
 * If no error file is found, send back InternalServerError
*/
std::string	seekErrorFile(HttpError error, ServerConfig &config)
{
    std::string pathfile;
    try {
        pathfile = config.getErrorFile(error.getErrorCode());
        return seekFile(pathfile, false);
    }
    catch(const HttpError &e) {
        if (error.getErrorCode() == 500)
            throw InternalServerError();
        pathfile = error.getDefaultFile();
        try {  return seekFile(pathfile, false); }
        catch(const HttpError& e) {
            throw InternalServerError();
        }
    }
}

//a simple check for hexadecimal numbers (chunk_parse dependent)
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

//parsing of chunked body in 8 chunks increments (ex :: hex+\r\n+chunk+...+0\r\n\r\n)
ustring chunk_parse(ustring &_body, ustring &new_body)
{
	if (_body.empty())
		throw Request::MissingData();
	unsigned int size = 0;
	int count = 0;
	
	if (_body.find((unsigned char *)"\r\n") == 0)
		_body = _body.substr(2);
	if (_body.empty())
		return new_body;
	while (!_body.empty())
	{
		if (count > 8) { throw Request::ChunkParsing(); } //de-chunk will be continued on next parse() call
		if (_body.find((unsigned char *)"\r\n") == 0)
			_body = _body.substr(2);
		std::size_t pos = _body.find((unsigned char *)"\r\n");
		if (pos == ustring::npos)
			throw Request::MissingData();
		ustring hex = _body.substr(0, pos);
		if (!check_hex(hex))
			throw BadRequest();
		sscanf((char *)hex.c_str(), "%x", &size);
		if (!size)
			break ;
		if (size > _body.length() - 2 - pos)
			throw Request::MissingData();
		_body = _body.substr(pos + 2);
		new_body += _body.substr(0, size);
		_body = _body.substr(size);
		count++;
	}
	if (size)
		throw Request::MissingData();
	if (_body.length() < 5 || _body.substr(0, 5) != (unsigned char *)"0\r\n\r\n")
		throw Request::MissingData();
	return new_body;
}

//check for alnum (-)
bool check_key(std::string key)
{
	for (std::string::iterator it = key.begin(); it != key.end(); it++)
	{
		if (!isalpha(*it) && *it != '-')
			return false;
	}
	return true;
}

//trims the whitespaces, checks for empty values
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

//separates the target (ex: /html/errors/error_411.html) into a locations list (ex: [/html, /errors, /error_411.html])
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

//cuts past the cgi script called for the uri path_info
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
