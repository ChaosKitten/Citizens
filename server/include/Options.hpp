#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <string>
#include <iostream>

namespace Citizens
{
	class Options
	{
	public:
		std::string config;
		bool batch_mode;
		std::string batch_file;
		bool print_help;
		Options(int argc, char* argv[]);
		static void help() {
			std::cerr << 
			"Usage:\n" <<
			"\t-h: prints this help\n" <<
			"\t-b [file]|--batch[=file]: enables batch mode and (optionally) opens and interprets 'file'\n" <<
			"\t-c file|--config=file: selects configuration file 'file'\n";
		};
	};
};

#endif
