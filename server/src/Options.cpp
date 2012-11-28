#include "Options.hpp"

// forthe getopt_long implementation
#ifdef _GNU_SOURCE
	#include <getopt.h>
#endif

#include <iostream>

using namespace Citizens;

// this is the getopt_long() implementation of the Options ctor
#ifdef _GNU_SOURCE
Options::Options(int argc, char* argv[])
{
	int opt;
	batch_mode = false;
	print_help = false;
	while(1)
	{
		int option_index = 0;
		static struct option valid_options[] = {
			{"config",1,0,'c'},
			{"batch",2,0,'b'}
		};
		opt = getopt_long(argc,argv,"c:b:h",valid_options,&option_index);
		if(opt==-1)
			break;
		switch(opt)
		{
			case 'c':
				config = optarg;
				std::cerr << "Loading configuration file: " << optarg << std::endl;
				break;
			case 'b':
				batch_file = optarg;
				batch_mode = true;
				break;
			case 'h':
				print_help = true;
				break;
			case '?':
				if(optopt == 'b')
				{
					//! \todo Options::Options has become Unix-specific! oh noooooooooo
					batch_file = "/dev/stdin";
					batch_mode = true;
				}
				break;
		}
	}
}
#endif
