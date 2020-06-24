#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;

struct gate;
struct gateWidget : ModuleWidget
{
	gateWidget(gate *module);
};

#define NUM_GATES 4
struct gate : Module
{
	enum ParamIds
	{
		NUM_PARAMS
	};
	enum InputIds
	{
		IN_A1,
		IN_B1 = IN_A1+NUM_GATES,
		CNTRL_1 = IN_B1+NUM_GATES,
		NUM_INPUTS = CNTRL_1+NUM_GATES
	};
	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS = OUT_1+NUM_GATES
	};
	enum LightIds
	{
		LED_A1,
		LED_B1 = LED_A1+NUM_GATES,
		NUM_LIGHTS=LED_B1+NUM_GATES
	};

	gate() : Module()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}
	void process(const ProcessArgs &args) override;

private:
};
