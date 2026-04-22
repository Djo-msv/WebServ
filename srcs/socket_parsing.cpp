#include "socket_parsing.hpp"



int		getFlags(MymlDictionary *repertory)
{
	int flags = 0;
	MymlObject *methods = repertory->getValue("allow_methods");

	if (methods->isList()) {
		MymlList *flags_list = methods->getAsList();
		for (std::list<MymlObject*>::iterator it = flags_list->begin(); it != flags_list->end(); it++)
		{
			try { flags |= ServerConfig::stringToMethodFlag((*it)->getAsString());} RETHROW(NotAllowed)
		}
	}
	else
		flags |= ServerConfig::stringToMethodFlag(methods->getAsString());
	return (flags);
}

location	*parse_location(MymlDictionary *location_repertory, location root_loc)
{
	std::string index("");
	std::string root;
	int			redirect = 0;
	int			flags = root_loc.allowed_methods;
	bool		should_list = root_loc.should_list;

	if (location_repertory->has("index"))
		index = location_repertory->getValueAsString("index");
	if (location_repertory->has("list_files"))
		should_list = location_repertory->getValueAsString("list_files") == "true" ? true : false;
	if (location_repertory->has("root"))
	{
		if (location_repertory->has("alias") || location_repertory->has("redirect"))
			throw (std::invalid_argument(EXCLUSIVE_RULE_ERROR));
		root = location_repertory->getValueAsString("root") + location_repertory->getKey();
	} else if (location_repertory->has("alias"))
	{
		if (location_repertory->has("redirect"))
			throw (std::invalid_argument(EXCLUSIVE_RULE_ERROR));
		root = location_repertory->getValueAsString("alias");
	}
	else if (location_repertory->has("redirect")){
		MymlDictionary *redir = location_repertory->getValueAsDictionary("redirect");
		root = redir->getValueAsString("url");
		redirect = redir->getValueAsInt("code");
		if (redirect < 301 || redirect > 308 || (redirect >= 304 && redirect <= 306))
			throw (std::invalid_argument("Redirect code must be set at 301, 302, 303, 307 or 308 do to a proper redirection"));
	}
	else
		root = root_loc.root + location_repertory->getKey();
	try {flags = getFlags(location_repertory);} IGNORE(std::invalid_argument);

	location *loc = new location;
	loc->allowed_methods = flags;
	loc->should_list = should_list;
	loc->index = index;
	loc->root = root;
	loc->redirect = redirect;
	return (loc);
}


void	init_error_files(MymlDictionary *serverRepertory, std::map<int, std::string> &error_files) {
	try {
		std::list<MymlObject *> *error_list = serverRepertory->getValueAsList("error_files")->getList();
		for (std::list<MymlObject *>::iterator it = error_list->begin(); it != error_list->end(); ++it)
		{
			MymlPair	*pair = (*it)->getAsPair();

			error_files.insert(std::make_pair(pair->getKeyAsInt(), pair->getValue()->getAsString()));
		}
	} IGNORE(std::invalid_argument)
}


void	initCgiHandlers(MymlDictionary *serverRepertory, std::map<std::string, std::string> &cgiHandlers) {
	try
	{
		std::list<MymlObject *> *cgi_list = serverRepertory->getValueAsList("cgi_handlers")->getList();
		for (std::list<MymlObject *>::iterator it = cgi_list->begin(); it != cgi_list->end(); ++it)
		{
			MymlPair	*pair = (*it)->getAsPair();
			cgiHandlers.insert(std::make_pair(pair->getKey(), pair->getValue()->getAsString()));
		}
	}
	IGNORE (std::invalid_argument)
	RETHROW (MymlObject::BadCast)
}

void	free_locations(std::map<std::string, location *> &server_locations)
{
	for (std::map<std::string, location *>::iterator it = server_locations.begin(); it != server_locations.end(); ++it)
	{
		delete it->value;
	}
}

void	initOptionnalConfig(MymlDictionary *serverRepertory, std::string &exec_folder, time_t &timeout,
	std::map<std::string, std::string> &cgi_handlers, location &root_loc,
	std::map<std::string, location *> &server_locations, std::map<int, std::string> &error_files)
{
	try { root_loc.allowed_methods = getFlags(serverRepertory); } IGNORE(std::invalid_argument)
	try { root_loc.index = serverRepertory->getValueAsString("index"); } IGNORE(std::invalid_argument)
	try { root_loc.should_list = serverRepertory->getValueAsString("list_files") == "true" ? true : false;} IGNORE(std::invalid_argument)
	try { exec_folder = serverRepertory->getValueAsString("execution_folder"); } IGNORE(std::invalid_argument)
	try { timeout = serverRepertory->getValueAsInt("timeout"); } IGNORE(std::invalid_argument)
	try { initCgiHandlers(serverRepertory, cgi_handlers); } IGNORE(std::invalid_argument)
	try {
		MymlList *locations = serverRepertory->getValueAsList("locations");
		for (std::list<MymlObject *>::iterator it = locations->begin(); it != locations->end(); ++it)
		{
			MymlDictionary *dict = (*it)->getAsDictionnary();
			if (server_locations.find(dict->getKey()) != server_locations.end())
				throw std::invalid_argument("The same location was defined two times");
			server_locations[dict->getKey()] = parse_location(dict, root_loc);
		}
	}
	catch (const ServerConfig::ConfigNotImplemented &e) { free_locations(server_locations); throw e; }
	catch (const MymlObject::BadCast &e) { free_locations(server_locations); throw e; }
	catch (const std::invalid_argument &e) { free_locations(server_locations); throw e; }
	try { init_error_files(serverRepertory, error_files); } IGNORE(std::invalid_argument)
}

ServerConfig *initServerConfig(MymlDictionary *serverRepertory)
{
	std::map<std::string, std::string>	cgi_handlers;
	std::map<int, std::string>			errorFiles;
	std::map<std::string, location *> 	locations;
	location							root_loc = (location){0, 0, false, "", ""};
	std::string 						exec_folder;
	std::string							upload_folder;
	ssize_t 							max_body = -1;
	time_t	timeout = 15;
	int		port;
	
	try
	{
		port = serverRepertory->getValueAsInt("port");
		if (serverRepertory->has("alias"))
			throw std::invalid_argument("alias rule is not allowed at server root");
		if (serverRepertory->has("root"))
		{
			if (serverRepertory->has("redirect"))
				throw std::invalid_argument(EXCLUSIVE_RULE_ERROR);
			root_loc.root = serverRepertory->getValueAsString("root");
		}
		else if (serverRepertory->has("redirect"))
		{
			MymlDictionary *redir = serverRepertory->getValueAsDictionary("redirect");
			root_loc.root = redir->getValueAsString("url");
			root_loc.redirect = redir->getValueAsInt("code");
			int redirect = root_loc.redirect;
			if (redirect < 301 || redirect > 308 || (redirect >= 304 && redirect <= 306))
				throw (std::invalid_argument("Redirect code must be set at 301, 302, 303, 307 or 308 do to a proper redirection"));
		}
	}
	catch (std::invalid_argument &e) { throw std::invalid_argument(std::string("mandatory argument error : ") + e.what()); }
	RETHROW (MymlObject::BadCast)
	RETHROW (std::bad_alloc)
	try { max_body =  serverRepertory->getValueAsLong("max_body"); } IGNORE (std::invalid_argument)
	try	{ initOptionnalConfig(serverRepertory, exec_folder, timeout, cgi_handlers, root_loc, locations, errorFiles); }
	catch (MymlObject::BadCast &e) {
		throw (MymlObject::BadCast("config optional argument error at server " +  serverRepertory->getKey() + ' ' + e.what()));
	}
	try {
		upload_folder = serverRepertory->getValueAsString("upload_folder");
		std::map<std::string, location *>::const_iterator it;
		if ((it = locations.find(upload_folder)) != locations.end())
		{
			if (!(it->value->allowed_methods & ServerConfig::POST))
				it->value->allowed_methods |= ServerConfig::POST;
		}
		else
			locations[upload_folder] = new location((location){ServerConfig::POST, 0, false, upload_folder, ""});
	} IGNORE (std::invalid_argument)
	return (new ServerConfig(port, root_loc, locations, cgi_handlers, exec_folder, upload_folder, max_body, errorFiles, timeout));
}
