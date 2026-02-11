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
		throw Request::FileNotFound();
	
	if (file_stat.st_mode & S_IRUSR)
		return (pathfile);
	throw (HttpError("Forbidden ?")); // ? 403 forbidden or 500 Internal Server error ?

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
	catch (std::runtime_error &e) { throw ; }
}