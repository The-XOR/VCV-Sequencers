#pragma once
#include "common.hpp"
#include <algorithm>

struct Klee;
struct KleeWidget : SequencerWidget
{
public:
	enum MENUACTIONS
	{
		RANDOMIZE_BUS = 0x01,
		RANDOMIZE_PITCH = 0x02,
		RANDOMIZE_LOAD = 0x04,
		RANDOMIZE_LAQUALUNQUE = 0x08,
		SET_RANGE_1V,
		QUANTIZE_PITCH
	};
	struct RandomizeSubItemItem : MenuItem {
		RandomizeSubItemItem(Module *k, const char *title, int action);

		int randomizeDest;
		Klee *kl;
		void onAction(const event::Action &e) override;
	};

	struct RandomizeItem : ui::MenuItem
	{
	public:
		RandomizeItem(Module *k)
		{
			kl = k;
			text = "Force the hand of chance";
			rightText = RIGHT_ARROW;
		};
		Menu *createChildMenu() override
		{
			Menu *sub_menu = new Menu;
			sub_menu->addChild(new RandomizeSubItemItem(kl, "Ov Pitch", RANDOMIZE_PITCH));
			sub_menu->addChild(new RandomizeSubItemItem(kl, "Ov Bus", RANDOMIZE_BUS));
			sub_menu->addChild(new RandomizeSubItemItem(kl, "Ov Load", RANDOMIZE_LOAD));
			sub_menu->addChild(new RandomizeSubItemItem(kl, "Ov Power", RANDOMIZE_LAQUALUNQUE));
			return sub_menu;
		}

	private:
		Module *kl;
	};
private:
	Menu *addContextMenu(Menu *menu) override;

public:
	KleeWidget(Klee *module);
	void onMenu(int action);
};

struct Klee : Module
{
	enum ParamIds
	{
		PITCH_KNOB,
		GROUPBUS = PITCH_KNOB + 16,
		LOAD_BUS = GROUPBUS + 16,
		LOAD_PARAM = LOAD_BUS + 16,
		STEP_PARAM,
		X28_X16,
		RND_PAT,
		B_INV,
		RND_THRESHOLD,
		BUS1_LOAD,
		BUS_MERGE,
		BUS2_MODE = BUS_MERGE + 3,
		RANGE,
		NUM_PARAMS = RANGE + cvMiniStrip::CVMINISTRIP_PARAMS
	};

	enum InputIds
	{
		LOAD_INPUT,
		EXT_CLOCK_INPUT,
		RND_THRES_IN,
		RANDOMIZONE,
		RANGE_IN,
		NUM_INPUTS = RANGE_IN + cvMiniStrip::CVMINISTRIP_INPUTS
	};

	enum OutputIds
	{
		CV_A,
		CV_B,
		CV_AB,
		CV_A__B,
		GATE_OUT,
		TRIG_OUT = GATE_OUT + 3,
		EXPANDER_OUT = TRIG_OUT + 3,
		NUM_OUTPUTS
	};

	enum LightIds
	{
		LED_PITCH,

		LED_BUS = LED_PITCH + 16,
		temp1 = LED_BUS + 3,
		NUM_LIGHTS
	};

	Klee() : Module()
	{
		pWidget = NULL;
		theRandomizer = 0;

		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		cvs.configure(this, NUM_PARAMS - cvMiniStrip::CVMINISTRIP_PARAMS);

		for(int k = 0; k < 8; k++)
		{
			configParam(Klee::LOAD_BUS + k, 0.0, 1.0, 0.0);
			configParam(Klee::LOAD_BUS + k + 8, 0.0, 1.0, 0.0);
			configParam(Klee::GROUPBUS + k, 0.0, 2.0, 2.0);
			configParam(Klee::GROUPBUS + k + 8, 0.0, 2.0, 2.0);
		}

		for(int k = 0; k < 3; k++)
		{
			configParam(Klee::BUS_MERGE + k, 0.0, 1.0, 0.0);
		}

		configParam(Klee::BUS2_MODE, 0.0, 1.0, 0.0);
		configParam(Klee::LOAD_PARAM, 0.0, 1.0, 0.0);
		configParam(Klee::BUS1_LOAD, 0.0, 1.0, 0.0);
		configParam(Klee::STEP_PARAM, 0.0, 1.0, 0.0);
		configParam(Klee::X28_X16, 0.0, 1.0, 0.0);
		configParam(Klee::RND_PAT, 0.0, 1.0, 0.0);
		configParam(Klee::B_INV, 0.0, 1.0, 0.0);
		configParam(Klee::RND_THRESHOLD, 0.0, 1.0, 0.0);

		for(int k = 0; k < 8; k++)
		{
			configParam(Klee::PITCH_KNOB + k, 0.0, 1.0, 0.125);
			configParam(Klee::PITCH_KNOB + 8 + k, 0.0, 1.0, 0.125);
		}
		#ifdef LAUNCHPAD
		drv = new LaunchpadBindingDriver(this, Scene1, 1);
		#endif
		#ifdef OSCTEST_MODULE
		oscDrv = new OSCDriver(this, 1);
		#endif

		on_loaded();
	}
	void QuantizePitch();

	#ifdef DIGITAL_EXT
	~Klee()
	{
		#if defined(LAUNCHPAD)
		delete drv;
		#endif
		#if defined(OSCTEST_MODULE)
		delete oscDrv;
		#endif
	}
	#endif

	void setWidget(KleeWidget *pwdg) { pWidget = pwdg; }

	void dataFromJson(json_t *root) override
	{
		Module::dataFromJson(root);
		json_t *rndJson = json_object_get(root, "theRandomizer");
		if(rndJson)
			theRandomizer = json_integer_value(rndJson);
		on_loaded();
	}
	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();
		json_t *rndJson = json_integer(theRandomizer);
		json_object_set_new(rootJ, "theRandomizer", rndJson);
		return rootJ;
	}
	void process(const ProcessArgs &args) override;
	void onReset() override { load(); }
	void onRandomize() override { load(); }

	#ifdef DIGITAL_EXT
	float connected;
	#endif
	#ifdef LAUNCHPAD
	LaunchpadBindingDriver *drv = NULL;
	#endif
	#if defined(OSCTEST_MODULE)
	OSCDriver *oscDrv = NULL;
	#endif
	int theRandomizer;
	cvMiniStrip cvs;

private:
	static uint8_t bitfield[8];
	KleeWidget *pWidget;
	void showValues();
	void sr_rotate();
	bool chance();
	void populate_gate(int clk);
	void update_bus();
	void load();
	void on_loaded();
	void randrandrand();
	void randrandrand(int action);
	void populate_outputs();
	void check_triggers(float deltaTime);
	int getValue3(int k);
	dsp::SchmittTrigger loadTrigger;
	SchmittTrigger2 clockTrigger;
	dsp::PulseGenerator triggers[3];
	dsp::SchmittTrigger rndTrigger;

	union
	{
		struct
		{
			bool A[8];
			bool B[8];
		};
		bool P[16];
	} shiftRegister;

	bool bus_active[3];
};
