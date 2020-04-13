#include "readInputFile.h"

inputFile::inputFile() {};

inputFile::inputFile(std::string iFile)
{
	m_iFile = iFile;
	setInput();
}

void inputFile::setInput()
{
	std::ifstream input(m_iFile);
	std::string container;
	while (std::getline(input, container))
	{
		std::istringstream tokenizer(container);

		std::string key;
		std::string val;

		std::getline(tokenizer, key, ':');
		std::getline(tokenizer, val);
		// only change the case of the keys to all caps, so user input is unaffected
		std::transform(key.begin(), key.end(), key.begin(), toupper);
		m_InputArgs[key] = val;
	}
}