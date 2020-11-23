#include "common.hpp"

////////////////////
// module widgets
////////////////////
using namespace rack;
extern Plugin *pluginInstance;

#define NUM_ATTENUATORS  (4)
#define NUM_VLIMITERS  (4)

struct Attenuator;
struct AttenuatorWidget : ModuleWidget
{
	AttenuatorWidget(Attenuator * module);
};

struct Attenuator : Module
{
	enum ParamIds
	{
		ATT_1,
		OFFS_1 = ATT_1 + NUM_ATTENUATORS,
		LIM1_MIN = OFFS_1 + NUM_ATTENUATORS,
		LIM1_MAX = LIM1_MIN + NUM_VLIMITERS,
		ATTMODE = LIM1_MAX + NUM_VLIMITERS,
		NUM_PARAMS
	};
	enum InputIds
	{
		IN_1,
		NUM_INPUTS = IN_1 + NUM_ATTENUATORS + NUM_VLIMITERS
	};
	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS = OUT_1 + NUM_ATTENUATORS + NUM_VLIMITERS
	};
	enum LightIds
	{
		NUM_LIGHTS
	};
	Attenuator() : Module()
	{	
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		for(int k = 0; k < NUM_ATTENUATORS; k++)
		{
			configParam(Attenuator::ATT_1+k, -5.0, 5.0, 1.0, "Amplitude", "%", 0, 100);
			configParam(Attenuator::OFFS_1+k, LVL_MIN, LVL_MAX, 0.0, "Offset", "V");
		}

		for(int k = 0; k < NUM_VLIMITERS; k++)
		{
			configParam(Attenuator::LIM1_MIN+k, LVL_MIN, LVL_MAX, LVL_MIN, "Min Voltage", "V");
			configParam(Attenuator::LIM1_MAX+k, LVL_MIN, LVL_MAX, LVL_MAX, "Max Voltage", "V");
		}
	}
	void process(const ProcessArgs &args) override;
};
