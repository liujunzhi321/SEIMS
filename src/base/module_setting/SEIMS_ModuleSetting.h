#pragma once
#include <string>
#include <vector>

using namespace std;

class WetspaModuleSetting
{
public:
	WetspaModuleSetting(string moduleId,string setting);
	~WetspaModuleSetting(void);
	
	//climate data type for TSD and ITP
	float dataType();
	string dataTypeString();

	//vertical interpolation information for ITP
	bool needDoVerticalInterpolation();

	//get channel flow routing method
	string channelFlowRoutingMethod();

	//get channel sediment routing method
	string channelSedimentRoutingMethod();

	//get channel nutrient routing method
	string channelNutrientRoutingMethod();
private:
	string  m_moduleId;
	string  m_settingString;
	vector<string> m_settings;
	
	void	getSettings();
	string  channelRoutingMethod(int);

	static float dataTypeString2Float(string);
	static string dataType2String(float);



};

