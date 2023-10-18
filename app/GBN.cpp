#include "Global.h"
#include "Config.h"
#include "Helpers.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "GBN/GBNRdtSender.h"
#include "GBN/GBNRdtReceiver.h"

#include <cassert>
#include <string>
#include <unistd.h>
#include <getopt.h>


int main(int argc, char* argv[]) {
#ifdef DEBUG_NO_RANDOM
    srand(347);
#endif
	// option related vars
	std::string input_file = "./input.txt";
	std::string output_file = "./output.txt";
	int run_mode = 0;

	// read options
	int opt;
    int digit_optind = 0;
    int option_index = 0;
    std::string optlist = "a:b:c:d";
    static struct option long_options[] =
    {  
        {"input", required_argument, nullptr, 'i'},
        {"output", required_argument, nullptr, 'o'},
        {"verbose", required_argument, nullptr, 'v'},
        {nullptr,  0,                 nullptr, 0},
    }; 
    while((opt = getopt_long(argc, argv, optlist.c_str(), long_options, &option_index))!= -1)
    {  
        switch (opt) {
			case 'i': 
				input_file = optarg;
				break;
			case 'o':
				output_file = optarg;
				break;
			case 'v':
				run_mode = atoi(optarg);
				break;
			default:
				throw "an error occur when parsing args.";
		}
    }  

	logger->common_info("input file: " + input_file);
	logger->common_info("output file: " + output_file);
	logger->common_info("run mode: " + std::to_string(run_mode));
	// assert(0);

	Helpers::init(WINDOW_SIZE, MAX_SEQNUM);

    RdtSender *ps = new GBNRdtSender();
	RdtReceiver * pr = new GBNRdtReceiver();
	pns->setRunMode(run_mode);  //VERBOS模式
//	pns->setRunMode(1);  //安静模式
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	// pns->setInputFile("../input_.txt");
	pns->setInputFile(input_file.c_str());
	pns->setOutputFile(output_file.c_str());

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//指向唯一的工具类实例，只在main函数结束前delete
	delete pns;										//指向唯一的模拟网络环境类实例，只在main函数结束前delete
	
	return 0;
}