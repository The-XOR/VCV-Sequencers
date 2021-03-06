#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;

struct kExp;
struct kExpWidget : ModuleWidget
{
	kExpWidget(kExp *module);
};

struct kExp : Module
{
	enum ParamIds
	{
		KNOB_1,
		SEQ_SELECT = KNOB_1+8,
		NUM_PARAMS
	};
	enum InputIds
	{
		ANDIN_1,
		ANDIN_2=ANDIN_1+4,
		EXP_IN = ANDIN_2+4,
		CLOCK_IN,
		NUM_INPUTS
	};
	enum OutputIds
	{
		OROUT,
		ANDOUT = OROUT+4,
		BUS_OUTA = ANDOUT+4,
		BUS_OUTB = BUS_OUTA+8,
		OUT = BUS_OUTB+8,
		NUM_OUTPUTS
	};
	enum LightIds
	{
		LDA,
		LDB = LDA + 8,
		EXP_LED = LDB + 8,
		NUM_LIGHTS
	};

	kExp() : Module()
	{		
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for(int k=0; k < 8; k++)
			configParam(KNOB_1+k, LVL_MIN, LVL_MAX, 0);
	}
	void process(const ProcessArgs &args) override;

private:
	SchmittTrigger2 clockTrigger;
	uint8_t and_mask;
};
