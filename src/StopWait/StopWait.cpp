// StopWait.cpp : 定义控制台应用程序的入口点。
//


#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "StopWait/StopWaitRdtSender.h"
#include "StopWait/StopWaitRdtReceiver.h"


int main(int argc, char* argv[])
{
	srand(40);  // for debug
	RdtSender *ps = new StopWaitRdtSender();
	RdtReceiver * pr = new StopWaitRdtReceiver();
	pns->setRunMode(0);  //VERBOS模式
//	pns->setRunMode(1);  //安静模式
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("../input_.txt");
	pns->setOutputFile("../output.txt");

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//指向唯一的工具类实例，只在main函数结束前delete
	delete pns;										//指向唯一的模拟网络环境类实例，只在main函数结束前delete
	
	return 0;
}

