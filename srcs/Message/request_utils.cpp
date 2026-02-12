#include <request_utils.hpp>


std::string	seekFile(std::string &pathfile)
{
	/** 
	* TODO vérifier l'existence et les permissions du fichier, donné, si le fichier donné est un code erreur ou index,
	* * récupérer le fichier stocké dans serverConf sinon chercher dans le Workdir du serverConf en utilisant le path donné
	* * le fichier n'existe pas ou n'est pas trouvé renvoyer fichier d'erreur du serverConf, (HttpError, "404 not found") 
	* * si n'existe pas renvoyer défaut ("403 Forbidden"), si inaccessible renvoyer page HTML hard codé d'erreur de permission ("500 Internal Server Error"),
	* * séparer nom de l'extension, récupérer le cgi depuis serverConf,
	* * si pas de CGi ou d'extension renvoyer page brut avec exception spécial
	
	**/

	struct stat file_stat;
	if (stat(pathfile.c_str(), &file_stat) == -1)
		throw FileNotFound();
	
	if (file_stat.st_mode & S_IRUSR)
		return (pathfile);
	throw Forbidden();

}


std::string	extractCgi(std::string &file, ServerConfig &config)
{
	std::string extension;
	size_t dotPos = file.find_last_of('.');
	if (dotPos != std::string::npos)
		extension = file.substr(dotPos);
	try {
		return config.getCgi(extension);
	}
	catch (const std::invalid_argument &e) { throw InternalServerError(); }
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
static bool check_hex(std::string hex)
{
	if (hex.empty())
		return false;
	for (std::string::iterator it = hex.begin(); it != hex.end(); it++)
	{
		*it = std::toupper(*it);
		if (!isdigit(*it) && (*it < 'A' || *it > 'F'))
			return false;
	}
	return true;
}

//parsing of chunked body (ex :: hex+\r\n+chunk+...+0\r\n)
std::string chunk_parse(std::string _body)
{
	if (_body.empty())
		throw Request::MissingData();
	std::string new_body;
	unsigned int size = 0;
	while (!_body.empty())
	{
		std::size_t pos = _body.find("\r\n");
		if (pos == std::string::npos)
			throw BadRequest();
		std::string hex = _body.substr(0, pos);
		_body = _body.substr(pos + 2);
		if (!check_hex(hex))
			throw BadRequest();
		sscanf(hex.c_str(), "%x", &size);
		if (!size)
			break ;
		if (size > _body.length())
			throw Request::MissingData();
		new_body += _body.substr(0, size);
		_body = _body.substr(size);
	}
	if (size)
		throw Request::MissingData();
	return new_body;
}
