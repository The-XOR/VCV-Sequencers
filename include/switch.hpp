#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;

#define NUM_SWITCHES  (5)

struct XSwitch;
struct SwitchWidget : ModuleWidget
{
	SwitchWidget(XSwitch *module);
};

struct XSwitch : Module
{
	enum ParamIds
	{
		SW_1,
		INV_1 = SW_1 + NUM_SWITCHES,
		OUTMODE = INV_1 + NUM_SWITCHES,
		NUM_PARAMS
	};
	enum InputIds
	{
		IN_1,
		MOD_1= IN_1 + NUM_SWITCHES,
		NUM_INPUTS = MOD_1 + NUM_SWITCHES
	};
	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS = OUT_1 + NUM_SWITCHES
	};
	enum LightIds
	{
		LED_1,
		NUM_LIGHTS = LED_1 + NUM_SWITCHES
	};
	XSwitch() : Module()
	{		
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for(int k = 0; k < NUM_SWITCHES; k++)
			configParam(XSwitch::SW_1+k, 0.0, 1.0, 0.0);
	}
	void process(const ProcessArgs &args) override;

private:
	bool getSwitch(int n)
	{
		bool stat = getModulableSwitch(this, SW_1+n, MOD_1+n);
		return isSwitchOn(this, INV_1 + n) ? !stat : stat;
	}
};
