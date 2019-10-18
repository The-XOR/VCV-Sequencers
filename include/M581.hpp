#pragma once
#include "common.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "M581Types.hpp"

struct M581;
struct M581Widget : SequencerWidget
{
public:
	enum MENUACTIONS
	{
		RANDOMIZE_PITCH = 0x01,
		RANDOMIZE_COUNTER = 0x02,
		RANDOMIZE_MODE = 0x04,
		RANDOMIZE_ENABLE = 0x08,
		RANDOMIZE_LAQUALUNQUE = 0x10,
		QUANTIZE_PITCH
	};

	struct RandomizeSubItemItem : MenuItem {
		RandomizeSubItemItem(Module *m, const char *title, int action);

		int randomizeDest;
		M581 *md;
		void onAction(const event::Action &e) override;
	};

	struct RandomizeItem : ui::MenuItem
	{
	public:
		RandomizeItem(Module *m)
		{
			md = m;
			text = "Force the hand of chance";
			rightText = RIGHT_ARROW;
		};
		Menu *createChildMenu() override
		{
			Menu *sub_menu = new Menu;
			sub_menu->addChild(new RandomizeSubItemItem(md, "Ov Pitch", RANDOMIZE_PITCH));
			sub_menu->addChild(new RandomizeSubItemItem(md, "Ov Count", RANDOMIZE_COUNTER));
			sub_menu->addChild(new RandomizeSubItemItem(md, "Ov Mode", RANDOMIZE_MODE));
			sub_menu->addChild(new RandomizeSubItemItem(md, "Ov En/Dis", RANDOMIZE_ENABLE));
			sub_menu->addChild(new RandomizeSubItemItem(md, "Ov Power", RANDOMIZE_LAQUALUNQUE));
			return sub_menu;
		}

	private:
		Module *md;
	};

private:
	Menu *addContextMenu(Menu *menu) override;

public:
	M581Widget(M581 *module);
	void onMenu(int action);
};

struct BefacoSlidePotFix : SvgSlider
{
	BefacoSlidePotFix()
	{
		pModule = NULL;
		myID = -1;
		Vec margin = Vec(4.5, 4.5);

		maxHandlePos = Vec(mm2px(-3.09541 / 2.0 + 2.27312 / 2.0), -mm2px(5.09852 / 2.0)).plus(margin);
		minHandlePos = Vec(mm2px(-3.09541 / 2.0 + 2.27312 / 2.0), mm2px(27.51667 - 5.09852 / 2.0)).plus(margin);
		setBackgroundSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/BefacoSlidePot.svg")));
		setHandleSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/BefacoSlidePotHandle.svg")));
		background->box.pos = margin;
		box.size = background->box.size.plus(margin.mult(2));
	}
	void SetID(M581 *pm, int id);

	void onDragStart(const event::DragStart &e) override;

private:
	M581 *pModule;
	int myID;
};

struct CounterSwitch : SvgSlider
{
	CounterSwitch()
	{
		snap = true;
		maxHandlePos = Vec(-mm2px(2.3 - 2.3 / 2.0), 0);
		minHandlePos = Vec(-mm2px(2.3 - 2.3 / 2.0), mm2px(24 - 2.8));
		background->svg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/counterSwitchPot.svg"));
		background->wrap();
		background->box.pos = Vec(0, 0);
		box.size = background->box.size;
		handle->svg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/counterSwitchPotHandle.svg"));
		handle->wrap();
	}

	void randomize() override { paramQuantity->setValue(roundf(random::uniform() * paramQuantity->getMaxValue())); }
};

struct RunModeDisplay : TransparentWidget
{
	float *mode;
	std::shared_ptr<Font> font;

	RunModeDisplay()
	{
		mode = NULL;
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Segment7Standard.ttf"));
	}

	void draw(const DrawArgs &args) override
	{
		if(mode != NULL)
		{
			// Background
			NVGcolor backgroundColor = nvgRGB(0x20, 0x20, 0x20);
			NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
			nvgFillColor(args.vg, backgroundColor);
			nvgFill(args.vg);
			nvgStrokeWidth(args.vg, 1.0);
			nvgStrokeColor(args.vg, borderColor);
			nvgStroke(args.vg);
			// text
			nvgFontSize(args.vg, 18);
			nvgFontFaceId(args.vg, font->handle);
			nvgTextLetterSpacing(args.vg, 2.5);

			Vec textPos = Vec(2, 18);
			NVGcolor textColor = nvgRGB(0xdf, 0xd2, 0x2c);
			nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
			nvgText(args.vg, textPos.x, textPos.y, "~~", NULL);

			textColor = nvgRGB(0xda, 0xe9, 0x29);
			nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
			nvgText(args.vg, textPos.x, textPos.y, "\\\\", NULL);

			textColor = nvgRGB(0xf0, 0x00, 0x00);
			nvgFillColor(args.vg, textColor);
			nvgText(args.vg, textPos.x, textPos.y, run_modes[int(std::round(*mode))], NULL);
		}
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
		M_RESET,
		RANGE,
		NUM_PARAMS = RANGE + cvStrip::CVSTRIP_PARAMS
	};

	enum InputIds
	{
		CLOCK,
		RESET,
		RANDOMIZONE,
		RANGE_IN,
		NUM_INPUTS = RANGE_IN + cvStrip::CVSTRIP_INPUTS
	};

	enum OutputIds
	{
		CV,
		GATE,
		EXPANDER_OUT,
		NUM_OUTPUTS
	};

	enum LightIds
	{
		LED_STEP,
		LED_SUBDIV = LED_STEP + 8,
		NUM_LIGHTS = LED_SUBDIV + 8
	};

	M581() : Module()
	{
		pWidget = NULL;
		theRandomizer = 0;
		lastSliderMoved = -1;

		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for(int k = 0; k < 8; k++)
		{
			configParam(M581::STEP_ENABLE + k, 0.0, 2.0, 1.0);
			configParam(M581::GATE_SWITCH + k, 0.0, 3.0, 2.0);
			configParam(M581::STEP_NOTES + k, 0.0, 1.0, 0.5);
			configParam(M581::COUNTER_SWITCH + k, 0.0, 7.0, 0.0);
		}
		cvs.configure(this, NUM_PARAMS - cvStrip::CVSTRIP_PARAMS);

		configParam(M581::GATE_TIME, 0.005, 1.0, 0.25);
		configParam(M581::SLIDE_TIME, 0.005, 2.0, 0.5);
		configParam(M581::STEP_DIV, 0.0, 3.0, 0.0);
		configParam(M581::NUM_STEPS, 1.0, 31.0, 8.0);
		configParam(M581::RUN_MODE, 0.0, 4.0, 0.0);

		#ifdef LAUNCHPAD
		drv = new LaunchpadBindingDriver(this, Scene2, 3);
		drv->SetAutoPageKey(LaunchpadKey::SESSION, 0);
		drv->SetAutoPageKey(LaunchpadKey::NOTE, 1);
		drv->SetAutoPageKey(LaunchpadKey::DEVICE, 2);
		#endif
		#ifdef OSCTEST_MODULE
		oscDrv = new OSCDriver(this, 2);
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

	int lastSliderMoved;
	void process(const ProcessArgs &args) override;
	void onReset() override { load(); }
	void onRandomize() override { load(); }
	void setWidget(M581Widget *pwdg) { pWidget = pwdg; }
	float getLastNoteValue();
	void QuantizePitch();

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
	LaunchpadBindingDriver *drv = NULL;
	#endif
	#if defined(OSCTEST_MODULE)
	OSCDriver *oscDrv = NULL;
	#endif
	int theRandomizer;
	cvStrip cvs;

private:
	CV_LINE cvControl;
	GATE_LINE gateControl;
	TIMER Timer;
	STEP_COUNTER stepCounter;
	ParamGetter getter;
	M581Widget *pWidget;

	void randrandrand();
	void randrandrand(int action);
	void _reset();
	void on_loaded();
	void load();
	void beginNewStep();
	void showCurStep(int cur_step, int sub_div);
	void expander_out(int cur_step, int sub_div);
	bool any();
	dsp::SchmittTrigger clockTrigger;
	dsp::SchmittTrigger resetTrigger;
	dsp::SchmittTrigger rndTrigger;
	dsp::SchmittTrigger masterReset;
};
