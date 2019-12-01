#pragma once

#include "../include/z8exp_base.hpp"

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

struct z8expV : z8exp
{
	enum ParamIds
	{
		VOLTAGE_1,
		_BASEPARAM = VOLTAGE_1 + 16,
		RANGE = _BASEPARAM + z8exp::NUM_PARAMS,
		NUM_PARAMS = RANGE + cvStrip::CVSTRIP_PARAMS
	};
	enum InputIds
	{
		_BASENPUT,
		RANGE_IN = _BASENPUT + z8exp::NUM_INPUTS,
		NUM_INPUTS = RANGE_IN + cvStrip::CVSTRIP_INPUTS
	};
	enum OutputIds
	{
		OUT,
		NUM_OUTPUTS
	};
	enum LightIds
	{
		_BASELED,
		NUM_LIGHTS = _BASELED + z8exp::NUM_LIGHTS
	};

	z8expV() : z8exp(_BASEPARAM, _BASENPUT, _BASELED)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParams();
		cvs.configure(this, NUM_PARAMS - cvStrip::CVSTRIP_PARAMS);		
		for(int r = 0; r < 4; r++)
		{
			for(int c = 0; c < 4; c++)
			{
				int n = c + r * 4;
				configParam(VOLTAGE_1 + n, 0.0, 1.0, 0.0, "Voltage", "V");
			}
		}
	}

	void QuantizePitch();
	void process(const ProcessArgs &args) override;
	virtual void onCreateWidget() override 	{cvs.Init(pWidget); }
	cvStrip cvs;
	virtual void onDisconnected() override
	{
		outputs[OUT].setVoltage(LVL_OFF);
	}
private:
};

