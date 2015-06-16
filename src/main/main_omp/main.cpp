// Defines the entry point for SEIMS application.
//

#include <iostream>
#include "invoke.h"
#include "util.h"
#include "gdal_priv.h"
#include "omp.h"
#define MAIN
//#define TEST
int main(int argc, const char* argv[])
{
	GDALAllRegister();
	string	modelPath = "";
	int		scenarioID = 0;
	int i = 0;
	int numThread = 1;
	LayeringMethod layeringMethod = UP_DOWN;
	char mongodbIP[16];
	strcpy(mongodbIP,"192.168.6.55");
	int port = 27017;
	if (argc < 2)
	{
		cout<<"Error: To run the program, use either the Simple Usage option or Usage option as below."<<endl;
		goto errexit;
	}
	else if(argc > 2)
		i = 1;
	else
		i = 2;
	while(argc > i)
	{
		if(isPathExists(argv[i])){
			modelPath = argv[i];
			i++;
		}
		else
			goto errexit;
		if (argc > i ){
			if(atoi(argv[i]) > 0)
			{
				numThread = atoi(argv[i]);
				i++;
			}
			else
				goto errexit;
		}
		if (argc > i){
			if(atoi(argv[i]) == 0 || atoi(argv[i]) == 1)
			{
				if(atoi(argv[i])==0)
					layeringMethod = UP_DOWN;
				else
					layeringMethod = DOWN_UP;
				i++;
			}
			else
				goto errexit;
		}
		if(argc > i){
			if(isIPAddress(argv[i]))
			{
				strcpy(mongodbIP,argv[i]);
				i++;
				if(argc > i && atoi(argv[i]) > 0)
				{
					port = atoi(argv[i]);
					i++;
				}
				else
					goto errexit;
			}
			else
				goto errexit;
		}
	}
	if (argc == 2)
	{
		modelPath = argv[1];
	}
	//cout<<modelPath<<endl;
	//cout<<numThread<<endl;
	//cout<<layeringMethod<<endl;
	//cout<<mongodbIP<<":"<<port<<endl;
	//omp_set_num_threads(2);

#ifdef MAIN
	while(modelPath.length() == 0) 
	{
		cout << "Please input the model path:" << endl;
		cin >> modelPath;
	}
#ifndef USE_MONGODB
	testMainSQLite(modelPath,scenarioID, numThread, layeringMethod);
#else
	testMain(modelPath,mongodbIP,port,scenarioID, numThread, layeringMethod);
#endif
	
#endif			

#ifdef TEST
	// run the model
	//Run(strTmp);
	//testSettingInput();
	testBMP();
	//testSettingOutput();
	
	//testModule();
	//testRaster();
#endif

	//system("pause");
	return 0;
errexit:
	cout<<"Simple Usage:\n "<<argv[0]<<" <ModelPath>"<<endl;
	cout<<"\tBy default: "<<endl;
	cout<<"\t\tThe mongodb's IP is 192.168.6.55, the port is 27017."<<endl;
	cout<<"\t\tThe threads or processor number is 1."<<endl;
	cout<<"\t\tThe Layering Method is UP_DOWN."<<endl<<endl;
	cout<<"Usage: "<<argv[0]<<"<ModelPath> <threadsNum> [<layeringMethod> <IP> <port>]"<<endl;
	cout<<"  <ModelPath> is the path of the configuration of the Model."<<endl;
	cout<<"  <threadsNum> must be greater or equal than 1."<<endl;
	cout<<"  <layeringMethod> can be 0 and 1, which means UP_DOWN and DOWN_UP respectively."<<endl;
	cout<<"  <IP> is the address of mongodb database, and <port> is its port number."<<endl;
	exit(0);
}