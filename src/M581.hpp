#pragma once
#include "common.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "M581Types.hpp"
////////////////////
// module widgets
////////////////////

struct M581;
struct M581Widget : SequencerWidget
{
private:
	enum MENUACTIONS
	{
		RANDOMIZE_PITCH,
		RANDOMIZE_COUNTER,
		RANDOMIZE_MODE,
		RANDOMIZE_ENABLE
	};
	Menu *addContextMenu(Menu *menu) override;

public:
	M581Widget(M581 *module);
	void onMenu(int action);
};


struct CounterSwitch : SVGFader
{
	CounterSwitch()
	{
		snap = true;
		maxHandlePos = Vec(-4, 0);
		minHandlePos = Vec(-4, 80);
		background->svg = SVG::load(assetPlugin(plugin, "res/counterSwitchPot.svg"));
		background->wrap();
		background->box.pos = Vec(0, 0);
		box.size = background->box.size;
		handle->svg = SVG::load(assetPlugin(plugin, "res/counterSwitchPotHandle.svg"));
		handle->wrap();
	}

	void randomize() override { setValue(roundf(randomUniform() * maxValue)); }
};

struct RunModeDisplay : TransparentWidget
{
	float *mode;
	std::shared_ptr<Font> font;

	RunModeDisplay()
	{
		font = Font::load(assetPlugin(plugin, "res/Segment7Standard.ttf"));
	}

	void draw(NVGcontext *vg) override
	{
		// Background
		NVGcolor backgroundColor = nvgRGB(0x20, 0x20, 0x20);
		NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
		nvgBeginPath(vg);
		nvgRoundedRect(vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
		nvgFillColor(vg, backgroundColor);
		nvgFill(vg);
		nvgStrokeWidth(vg, 1.0);
		nvgStrokeColor(vg, borderColor);
		nvgStroke(vg);
		// text
		nvgFontSize(vg, 18);
		nvgFontFaceId(vg, font->handle);
		nvgTextLetterSpacing(vg, 2.5);

		Vec textPos = Vec(2, 18);
		NVGcolor textColor = nvgRGB(0xdf, 0xd2, 0x2c);
		nvgFillColor(vg, nvgTransRGBA(textColor, 16));
		nvgText(vg, textPos.x, textPos.y, "~~", NULL);

		textColor = nvgRGB(0xda, 0xe9, 0x29);
		nvgFillColor(vg, nvgTransRGBA(textColor, 16));
		nvgText(vg, textPos.x, textPos.y, "\\\\", NULL);

		textColor = nvgRGB(0xf0, 0x00, 0x00);
		nvgFillColor(vg, textColor);
		nvgText(vg, textPos.x, textPos.y, run_modes[int(std::round(*mode))], NULL);
	}

private:
	const char *run_modes[5] = {
		"FWD",
		"BWD",
		"PNG",
		"BRN",
		"RND"
	};
};

struct M581 : Module
{
	enum ParamIds
	{
		GATE_SWITCH,
		COUNTER_SWITCH = GATE_SWITCH + 8,
		STEP_NOTES = COUNTER_SWITCH + 8,
		STEP_ENABLE = STEP_NOTES + 8,
		GATE_TIME = STEP_ENABLE + 8,
		SLIDE_TIME,
		NUM_STEPS,
		RUN_MODE,
		STEP_DIV,
		MAXVOLTS
		, NUM_PARAMS
	};

	enum InputIds
	{
		CLOCK,
		RESET,
		NUM_INPUTS
	};

	enum OutputIds
	{
		CV,
		GATE,
		NUM_OUTPUTS
	};

	enum LightIds
	{
		LED_STEP,
		LED_SUBDIV = LED_STEP + 8,
		temp = LED_SUBDIV + 8,
		NUM_LIGHTS
	};

	M581() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
	{
		#ifdef LAUNCHPAD
		drv = new LaunchpadBindingDriver(Scene2, 3);
		drv->SetAutoPageKey(LaunchpadKey::SESSION, 0);
		drv->SetAutoPageKey(LaunchpadKey::NOTE, 1);
		drv->SetAutoPageKey(LaunchpadKey::DEVICE, 2);
		#endif
		#ifdef OSCTEST_MODULE
		oscDrv = new OSCDriver(2);
		#endif

		on_loaded();
	}

	#ifdef DIGITAL_EXT
	~M581()
	{
		#if defined(LAUNCHPAD)
		delete drv;
		#endif
		#if defined(OSCTEST_MODULE)
		delete oscDrv;
		#endif
	}
	#endif

	void step() override;
	void reset() override { load(); }
	void randomize() override { load(); }

	void fromJson(json_t *root) override { Module::fromJson(root); on_loaded(); }
	json_t *toJson() override
	{
		json_t *rootJ = json_object();

		return rootJ;
	}

	float *getAddress(int var)
	{
		switch(var)
		{
			case 0: return &params[M581::RUN_MODE].value;
			case 1: return &params[M581::NUM_STEPS].value;
		}
		return NULL;
	}

	#ifdef DIGITAL_EXT
	float connected;
	#endif
	#ifdef LAUNCHPAD
	LaunchpadBindingDriver *drv;
	#endif
	#if defined(OSCTEST_MODULE)
	OSCDriver *oscDrv;
	#endif

private:
	CV_LINE cvControl;
	GATE_LINE gateControl;
	TIMER Timer;
	STEP_COUNTER stepCounter;
	ParamGetter getter;

	void _reset();
	void on_loaded();
	void load();
	void beginNewStep();
	void showCurStep(int cur_step, int sub_div);
	bool any();
	SchmittTrigger clockTrigger;
	SchmittTrigger resetTrigger;
};
