#include "socket_parsing.hpp"

int		getFlags(MymlDictionary *repertory)
{
	int flags = 0;
	MymlObject *methods = repertory->getValue("allow_methods");

	if (methods->isList()) {
		MymlList *flags_list = methods->getAsList();
		for (std::list<MymlObject*>::iterator it = flags_list->begin(); it != flags_list->end(); it++)
			flags |= ServerConfig::stringToMethodFlag((*it)->getAsString());
	}
	else
		flags |= ServerConfig::stringToMethodFlag(methods->getAsString());
	return (flags);
}

location	*parse_location(MymlDictionary *location_repertory, location root_loc)
{
	std::string index("");
	std::string root;
	int			flags;
	bool		should_list = root_loc.should_list;

	try
	{
		if (location_repertory->has("index"))
			index = location_repertory->getValueAsString("index");
		if (location_repertory->has("list_files"))
			should_list = location_repertory->getValueAsString("list_files") == "true" ? true : false;
		if (location_repertory->has("root"))
		{
			if (location_repertory->has("alias"))
				throw (std::invalid_argument("logic error : you cannot define both alias and root at the same time"));
			root = location_repertory->getValueAsString("root") + location_repertory->getKey();
		} else if (location_repertory->has("alias"))
			root = location_repertory->getValueAsString("alias");
		else
			root = root_loc.root + location_repertory->getKey();
		flags = getFlags(location_repertory);
	}
	catch(const std::exception &e) { throw e; }

	location *loc = new location;
	loc->allowed_methods = flags;
	loc->should_list = should_list;
	loc->index = index;
	loc->root = root;
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

void	initOptionnalConfig(MymlDictionary *serverRepertory, std::string &exec_folder, time_t &timeout,
	std::map<std::string, std::string> &cgi_handlers, location &root_loc,
	std::map<std::string, location *> &server_locations, std::map<int, std::string> &error_files)
{
	try
	{
		try { root_loc.allowed_methods = getFlags(serverRepertory); } IGNORE(std::invalid_argument)
		try { root_loc.index = serverRepertory->getValueAsString("index"); } IGNORE(std::invalid_argument)
		root_loc.should_list = false;
		try { root_loc.should_list = serverRepertory->getValueAsString("list_files") == "true" ? true : false;} IGNORE(std::invalid_argument)
		try { exec_folder = serverRepertory->getValueAsString("execution_folder"); } IGNORE(std::invalid_argument)
		try { timeout = serverRepertory->getValueAsInt("timeout"); } IGNORE(std::invalid_argument)
		try { initCgiHandlers(serverRepertory, cgi_handlers); } IGNORE(std::invalid_argument)
		try {
			MymlList *locations = serverRepertory->getValueAsList("locations");
			for (std::list<MymlObject *>::iterator it = locations->begin(); it != locations->end(); ++it)
			{
				MymlDictionary *dict = (*it)->getAsDictionnary();
				server_locations[dict->getKey()] = parse_location(dict, root_loc);
			}
		} IGNORE(std::invalid_argument)
		try { init_error_files(serverRepertory, error_files); } IGNORE(std::invalid_argument)
	} RETHROW(MymlObject::BadCast)
}

ServerConfig *initServerConfig(MymlDictionary *serverRepertory)
{
	std::map<std::string, std::string>	cgi_handlers;
	std::map<int, std::string>			errorFiles;
	std::map<std::string, location *> 	locations;
	location							root_loc = (location){0, "", ""};
	std::string 						exec_folder;
	std::string							upload_folder;
	ssize_t 							max_body = -1;
	time_t	timeout = 15;
	int		port;
	
	try
	{
		port = serverRepertory->getValueAsInt("port");
		root_loc.root = serverRepertory->getValueAsString("root");
	}
	catch (std::invalid_argument &e) { throw std::invalid_argument(std::string("missing mandatory argument : ") + e.what()); }
	RETHROW (MymlObject::BadCast)
	RETHROW (std::bad_alloc)
	try { max_body =  serverRepertory->getValueAsLong("max_body"); } IGNORE (std::invalid_argument)
	try { upload_folder =  serverRepertory->getValueAsString("upload_folder"); } IGNORE (std::invalid_argument)
	try	{ initOptionnalConfig(serverRepertory, exec_folder, timeout, cgi_handlers, root_loc, locations, errorFiles); }
	catch (MymlObject::BadCast &e) {
		throw (MymlObject::BadCast("config optional argument error at server " +  serverRepertory->getKey() + ' ' + e.what()));
	}
	RETHROW (std::invalid_argument)
	RETHROW (std::bad_alloc)
	return (new ServerConfig(port, root_loc, locations, cgi_handlers, exec_folder, upload_folder, max_body, errorFiles, timeout));
}
