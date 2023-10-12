#include "Global.h"
#include "Config.h"
#include "Helpers.h"
#include "TCPSender.h"
#include "TCPReceiver.h"

int main(int argc, char* argv[]) {
#ifdef DEBUG_NO_RANDOM
    srand(347);
#endif

    Helpers::init(WINDOW_SIZE, MAX_SEQNUM);

    RdtSender *ps = new TCPSender();
	RdtReceiver * pr = new TCPReceiver();
	pns->setRunMode(0);  //VERBOS模式
//	pns->setRunMode(1);  //安静模式
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	// pns->setInputFile("../input_.txt");
	pns->setInputFile("../input.txt");
	pns->setOutputFile("../output.txt");

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//指向唯一的工具类实例，只在main函数结束前delete
	delete pns;										//指向唯一的模拟网络环境类实例，只在main函数结束前delete
	
	return 0;
}