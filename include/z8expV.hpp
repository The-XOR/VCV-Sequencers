#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;

struct z8expV ;
struct z8expVWidget : SequencerWidget
{
	z8expVWidget(z8expV *module);
	void onMenu(int action);
private:
	enum MENUACTIONS
	{
		RANDOMIZE_PITCH,
		QUANTIZE_PITCH
	};

	Menu *addContextMenu(Menu *menu) override;
};

struct z8expV : Module
{
	enum ParamIds
	{
		VOLTAGE_1,
		CHANNEL=VOLTAGE_1+16,
		RANGE,
		NUM_PARAMS = RANGE + cvStrip::CVSTRIP_PARAMS
	};
	enum InputIds
	{
		EXP_IN,
		RANGE_IN,
		NUM_INPUTS = RANGE_IN + cvStrip::CVSTRIP_INPUTS
	};
	enum OutputIds
	{
		OUT,
		NUM_OUTPUTS
	};
	enum LightIds
	{
		LED_1,
		EXP_LED = LED_1+16,
		NUM_LIGHTS
	};

	z8expV() : Module()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		pWidget = NULL;
		cvs.configure(this, NUM_PARAMS - cvStrip::CVSTRIP_PARAMS);		
		for(int r = 0; r < 4; r++)
		{
			for(int c = 0; c < 4; c++)
			{
				int n = c + r * 4;
				configParam(VOLTAGE_1 + n, 0.0, 1.0, 0.0, "Voltage", "V");
			}
		}
		configParam(CHANNEL, 0.0, 2.0, 0.0);
		prevStep = -2;
	}
	void QuantizePitch();
	void process(const ProcessArgs &args) override;
	void setWidget(z8expVWidget *pwdg) 
	{ 
		pWidget = pwdg; 
		cvs.Init(pWidget);
	}
	cvStrip cvs;
private:
	z8expVWidget *pWidget;
	int prevStep;
};

