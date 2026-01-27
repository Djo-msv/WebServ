#prama once

class MymlParser
{
	public :
		MymlParser(char *files); /* Calling the constructor, verifying the file, and executing the process */
		~MymlParser(); /* Close _confFilefd */

		std::vector<serverConfiguration>	getServerConfiguration(); // return vector of serverConfiguration */
	private :
		int									_confFileFd;
		std::vector<serverConfiguration>	_servConf;

		int			openFile();
		void		addServerConfiguration();
		std::string	readLine();
		void		error();
};
