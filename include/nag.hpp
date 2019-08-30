#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;
#define NUM_NAGS (6)
#define NUM_STEPS (360)
#define MIN_VERTICES 1
#define MAX_VERTICES 32
#define MIN_ROTATE 0
#define MAX_ROTATE (NUM_STEPS-1)
#define MIN_SKEW 0
#define MAX_SKEW (NUM_STEPS-1)
#define MIN_DEGXCLOCK 1
#define MAX_DEGXCLOCK 90

struct nag; 

struct nagWidget : SequencerWidget
{
	nagWidget(nag *module);
	enum MENUACTIONS
	{
		RANDOMIZE_OFFON = 0x01,
		RANDOMIZE_VERTICES = 0x02,
		RANDOMIZE_ROTATION = 0x04,
		RANDOMIZE_SKEW = 0x08,
		RANDOMIZE_LAQUALUNQUE = 0x10
	};
	Menu *addContextMenu(Menu *menu) override;
	void onMenu(int action);

	struct RandomizeSubItemItem : MenuItem 
	{
		RandomizeSubItemItem(Module *n, const char *title, int action);

		int randomizeDest;
		nag *ng;
		void onAction(const event::Action &e) override;
	};

	struct RandomizeItem : ui::MenuItem
	{
	public:
		RandomizeItem(Module *sp)
		{
			ng = sp;
			text = "Force the hand of chance";
			rightText = RIGHT_ARROW;
		}
		Menu *createChildMenu() override
		{
			Menu *sub_menu = new Menu;
			sub_menu->addChild(new RandomizeSubItemItem(ng, "Ov Sin (and cos)", RANDOMIZE_OFFON));
			sub_menu->addChild(new RandomizeSubItemItem(ng, "Ov Numbers", RANDOMIZE_VERTICES));
			sub_menu->addChild(new RandomizeSubItemItem(ng, "Ov Rotation", RANDOMIZE_ROTATION));
			sub_menu->addChild(new RandomizeSubItemItem(ng, "Ov Skew", RANDOMIZE_SKEW));
			sub_menu->addChild(new RandomizeSubItemItem(ng, "Ov Power", RANDOMIZE_LAQUALUNQUE));
			return sub_menu;
		}
	private:
		Module *ng;
	};
};

struct NagSeq 
{
private:
	dsp::PulseGenerator banged;

public:
	int numVertici;
	int rotation;
	int skewFactor;
	bool enabled;
	NVGcolor mycolor;
	std::vector<int> sequence;

	void init(NVGcolor color)
	{ 
		mycolor = color;
		banged.reset();
		enabled = false;
		numVertici = 2;
		skewFactor = 0;
		rotate(0); 
	}

	bool bang(int counter, float dt)
	{
		if (enabled)
		{
			for (int k = 0; k < numVertici; k++)
			{
				if (sequence[k] == counter)
				{
					banged.trigger(PULSE_TIME);
					return true;
				}
			}
		}

		return Highlight(dt);
	}

	inline bool Highlight(float dt) { return banged.process(dt); }

	void reset() 
	{ 
		banged.reset();
		rotate(0); 
	}

	void rotate(int n)
	{
		if (n != rotation)
		{
			rotation = n;
			calcSeq();
		}
	}

	void skew(int n)
	{
		if (n != skewFactor)
		{
			skewFactor = n;
			calcSeq();
		}
	}

	void set(int v)
	{
		if (v != numVertici)
		{
			numVertici = v;
			calcSeq();
		}
	}

	void calcSeq()
	{
		sequence.clear();
		int degrees = (int)(skewFactor+(roundf(3600.0/numVertici)/10.0)); // decimi di grado
		int cur = rotation;
		for (int k = 0; k < numVertici; k++)
		{
			sequence.push_back(cur % 360);
			cur += degrees;
		}
	}
};

struct nag : Module
{
	enum ParamIds
	{
		DEGXCLK,
		ENABLE_1,
		VERTEX_1 = ENABLE_1 + NUM_NAGS,
		ROTATE_1 = VERTEX_1 + NUM_NAGS,
		SKEW_1 = ROTATE_1 + NUM_NAGS,
		DEGMODE = SKEW_1 + NUM_NAGS,
		M_RESET,
		NUM_PARAMS
	};
	enum InputIds
	{
		RESET,
		CLOCK,
		DEGXCLK_IN,
		INVERTEX_1,
		INROTATE_1 = INVERTEX_1 + NUM_NAGS,
		INSKEW_1 = INROTATE_1 + NUM_NAGS,
		RANDOMIZONE = INSKEW_1 + NUM_NAGS,
		NUM_INPUTS
	};
	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS = OUT_1 + NUM_NAGS
	};
	enum LightIds
	{
		ON_1,
		NUM_LIGHTS = ON_1 + NUM_NAGS
	};

	nag() : Module()
	{		
		pWidget = NULL;
		counterRemaining = 0;
		theCounter = 0;
		theRandomizer = 0;
		const NVGcolor SCHEME_CYAN2 = nvgRGB(0xC0, 0xC0, 0xC0);
		const NVGcolor SCHEME_BLUE2 = nvgRGB(0x29, 0xd2, 0xff);
		NVGcolor colors[NUM_NAGS] = { nvgRGB(0xFF, 0x00, 0x00), SCHEME_CYAN2, SCHEME_BLUE2,SCHEME_GREEN, SCHEME_PURPLE,SCHEME_YELLOW };
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (int index = 0; index < NUM_NAGS; index++)
		{
			configParam(ENABLE_1 + index, 0.0, 1.0, 0.0);
			configParam(VERTEX_1+index, MIN_VERTICES, MAX_VERTICES, MIN_VERTICES, "Vertices", "#");
			configParam(ROTATE_1+index, MIN_ROTATE, MAX_ROTATE, MIN_ROTATE, "Rotation", "Degrees");
			configParam(SKEW_1 + index, MIN_SKEW, MAX_SKEW, MIN_SKEW, "Skew", "#");
			sequencer[index].init(colors[index]);
		}

		configParam(DEGXCLK, MIN_DEGXCLOCK, MAX_DEGXCLOCK, MIN_DEGXCLOCK, "Degress x clock", "#");
		#ifdef OSCTEST_MODULE
		oscDrv = new OSCDriver(this, 6);
		#endif
	}

#ifdef DIGITAL_EXT
	~nag()
	{
#if defined(LAUNCHPAD)
		//delete drv;
#endif
#if defined(OSCTEST_MODULE)
		delete oscDrv;
#endif
	}
#endif
	int degPerClock();
	void process(const ProcessArgs &args) override;
	inline int getClock() { return theCounter; }
	NagSeq sequencer[NUM_NAGS];
	void setWidget(nagWidget *pwdg) { pWidget = pwdg; }
	void dataFromJson(json_t *root) override
	{
		Module::dataFromJson(root);
		json_t *rndJson = json_object_get(root, "theRandomizer");
		if (rndJson)
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
	int theRandomizer;

private:
	int getInput(int index, int input_id, int knob_id, float mi, float ma);
	void reset();
	void updateNags(float dt);
	void sclocca(bool dm, float dt);
	void randrandrand(int action);
	void randrandrand();
	void on_loaded();
	void load();
#if defined(OSCTEST_MODULE)
	OSCDriver *oscDrv = NULL;
#endif
	nagWidget *pWidget;

	dsp::SchmittTrigger masterReset;
	dsp::SchmittTrigger resetTrig;
	dsp::SchmittTrigger rndTrigger;
	SchmittTrigger2 clockTrig;
	int theCounter;
	int counterRemaining;
};
