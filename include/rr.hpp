#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;

struct RR;
struct RRWidget : ModuleWidget
{
	RRWidget(RR *module);
};

struct RR : Module
{
	enum ParamIds
	{
		EDGE,
		MODE,
		NUM_PARAMS
	};
	enum InputIds
	{
		CLOCK_IN,
		NUM_INPUTS
	};
	enum OutputIds
	{
		OUT1,
		NUM_OUTPUTS = OUT1+8
	};
	enum LightIds
	{
		LD1,
		NUM_LIGHTS = LD1+8
	};

	RR() : Module()
	{		
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		rr = n = -1;
		descSignal.reset();
		pulseRunning = false;
	}
	void process(const ProcessArgs &args) override;

private:
	int next(bool mode_sequential);
	int next();
	int next_seq();
	SchmittTrigger2 clockTrigger;
	int rr;
	float lastclockpulse;
	dsp::PulseGenerator descSignal;
	bool pulseRunning;
	int n;

};
