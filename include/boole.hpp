#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;

#define NUM_BOOL_OP		(5)		//not, and, or, (the) xor, implication
struct Boole;
struct BooleWidget : ModuleWidget
{
	BooleWidget(Boole *module);
};

struct Boole : Module
{
	enum ParamIds
	{
		INVERT_1,
		THRESH_X = INVERT_1 + NUM_BOOL_OP,
		THRESH_Y = THRESH_X + NUM_BOOL_OP,
		HIZ= THRESH_Y + NUM_BOOL_OP-1,
		COMPAREMODE,
		NUM_PARAMS,
	};
	enum InputIds
	{
		IN_X,
		IN_Y = IN_X + NUM_BOOL_OP,
		NUM_INPUTS = IN_Y+ NUM_BOOL_OP-1
	};
	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS = OUT_1 + NUM_BOOL_OP
	};
	enum LightIds
	{
		LED_X,
		LED_Y = LED_X + NUM_BOOL_OP, 
		LED_OUT = LED_Y + NUM_BOOL_OP-1,
		NUM_LIGHTS = LED_OUT + NUM_BOOL_OP
	};

	Boole() : Module()
	{		
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for(int k = 0; k < NUM_BOOL_OP; k++)
		{
			configParam(Boole::INVERT_1 + k, 0.0, 1.0, 0.0);
			configParam(Boole::THRESH_X + k, LVL_MIN, LVL_MAX, LVL_OFF, "Threshold", "V");
			if(k > 0)
				configParam(Boole::THRESH_Y + k-1, LVL_MIN, LVL_MAX, LVL_OFF, "Threshold", "V");
		}
		hizvalue = 0;
		hizCounter = 0;
	}
	void process(const ProcessArgs &args) override;

private:
	bool process(int num_op, bool hiz, bool compare);
	bool logicLevel(float v1, float v2, bool compare);
	float getVoltage(int index, bool hiz);
	unsigned int hizCounter;
	float hizvalue;
};
