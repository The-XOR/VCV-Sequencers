#pragma once
#include "common.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "z8kSequencer.hpp"

struct Z8KWidget : SequencerWidget
{
public:
	Z8KWidget(Z8K *module);
	void onMenu(int action);
private:
	enum MENUACTIONS
	{
		RANDOMIZE_PITCH,
		QUANTIZE_PITCH
	};

	Menu *addContextMenu(Menu *menu) override;
};

struct Z8K : Module
{
	enum ParamIds
	{
		VOLTAGE_1,
		M_RESET = VOLTAGE_1 + 16,
		RANGE,
		NUM_PARAMS = RANGE + cvStrip::CVSTRIP_PARAMS
	};

	enum InputIds
	{
		RESET_1,
		RESET_A = RESET_1 + 4,
		RESET_VERT = RESET_A + 4,
		RESET_HORIZ,

		DIR_1,
		DIR_A = DIR_1 + 4,
		DIR_VERT = DIR_A + 4,
		DIR_HORIZ,

		CLOCK_1,
		CLOCK_A = CLOCK_1 + 4,
		CLOCK_VERT = CLOCK_A + 4,
		CLOCK_HORIZ,

		RANDOMIZE,
		MASTERRESET,

		RANGE_IN,
		NUM_INPUTS = RANGE_IN + cvStrip::CVSTRIP_INPUTS
	};

	enum OutputIds
	{
		CV_1,
		CV_A = CV_1 + 4,
		CV_VERT = CV_A + 4,
		CV_HORIZ,
		ACTIVE_STEP,
		NUM_OUTPUTS = ACTIVE_STEP + 16
	};

	enum LightIds
	{
		LED_ROW,
		LED_COL = LED_ROW + 16,
		LED_VERT = LED_COL + 16,
		LED_HORIZ = LED_VERT + 16,
		NUM_LIGHTS = LED_HORIZ + 16
	};

	enum SequencerIds
	{
		SEQ_1,
		SEQ_A = SEQ_1 + 4,
		SEQ_VERT = SEQ_A + 4,
		SEQ_HORIZ,
		NUM_SEQUENCERS
	};

	Z8K() : Module()
	{
		pWidget = NULL;
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for(int r = 0; r < 4; r++)
		{
			for(int c = 0; c < 4; c++)
			{
				int n = c + r * 4;
				configParam(Z8K::VOLTAGE_1 + n, 0.0, 1.0, 0.0, "Voltage", "V");
			}
		}
		cvs.configure(this, NUM_PARAMS - cvStrip::CVSTRIP_PARAMS);
		/*
		#ifdef LAUNCHPAD
		drv = new LaunchpadBindingDriver(this, Scene4, 1);
		drv->SetAutoPageKey(LaunchpadKey::SESSION, 0);
		#endif*/
		#ifdef OSCTEST_MODULE
		oscDrv = new OSCDriver(this, 4);
		#endif
		on_loaded();
	}

	#ifdef DIGITAL_EXT
	~Z8K()
	{
		/*#if defined(LAUNCHPAD)
		delete drv;
		#endif*/
		#if defined(OSCTEST_MODULE)
		delete oscDrv;
		#endif
	}
	#endif

	void process(const ProcessArgs &args) override;
	void onReset() override { load(); }
	void setWidget(Z8KWidget *pwdg) { pWidget = pwdg; }
	void QuantizePitch();

	void dataFromJson(json_t *root) override { Module::dataFromJson(root); on_loaded(); }
	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();
		return rootJ;
	}
	cvStrip cvs;

	#ifdef DIGITAL_EXT
	float connected;
	#endif
	#if defined(OSCTEST_MODULE)
	OSCDriver *oscDrv = NULL;
	#endif

private:
	void on_loaded();
	void load();
	void reset();
	z8kSequencer seq[10];
	dsp::SchmittTrigger randomizeTrigger;
	Z8KWidget *pWidget;
	dsp::SchmittTrigger masterReset;
	dsp::SchmittTrigger masterResetIn;
};
