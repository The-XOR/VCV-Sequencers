#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;

struct z8expC ;
struct z8expCWidget : SequencerWidget
{
	z8expCWidget(z8expC *module);
};

struct z8expC : Module
{
	enum ParamIds
	{
		CHANNEL,
		NUM_PARAMS
	};
	enum InputIds
	{
		EXP_IN,
		CLOCK_IN,
		NUM_INPUTS
	};
	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS = OUT_1+16
	};
	enum LightIds
	{
		LED_1,
		EXP_LED = LED_1+16,
		NUM_LIGHTS
	};

	z8expC() : Module()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		pWidget = NULL;
		configParam(CHANNEL, 0.0, 2.0, 0.0);
		prevStep = -2;
		prevVoltage = -1000;
	}
	void process(const ProcessArgs &args) override;
	void setWidget(z8expCWidget *pwdg)
	{
		pWidget = pwdg;
	}
private:
	z8expCWidget *pWidget;
	int prevStep;
	float prevVoltage ;
};

