#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;

#define QUATTRO_NUM_STEPS  (8)
#define NUM_STRIPS  (4)

struct quattro;
struct quattroWidget : SequencerWidget
{
	enum MENUACTIONS
	{
		RANDOMIZE_PITCH = 0x01,
		RANDOMIZE_MODE = 0x02,
		RANDOMIZE_DIRECTION = 0x04,
		RANDOMIZE_ABCD = 0x08,
		RANDOMIZE_LAQUALUNQUE = 0x10,
		QUANTIZE_PITCH
	};

	struct RandomizeSubItemItem : MenuItem
	{
		RandomizeSubItemItem(Module *m, const char *title, int action);

		int randomizeDest;
		quattro *md;
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
			sub_menu->addChild(new RandomizeSubItemItem(md, "Ov Mode", RANDOMIZE_MODE));
			sub_menu->addChild(new RandomizeSubItemItem(md, "Ov Direction", RANDOMIZE_DIRECTION));
			sub_menu->addChild(new RandomizeSubItemItem(md, "Ov ABCD", RANDOMIZE_ABCD));
			sub_menu->addChild(new RandomizeSubItemItem(md, "Ov Power", RANDOMIZE_LAQUALUNQUE));
			return sub_menu;
		}
	private:
		Module *md;
	};

	quattroWidget(quattro *module);
	void onMenu(int action);
private:
	void create_strip(quattro *module, int n);
	Menu *addContextMenu(Menu *menu) override;
};

struct quattroStrip
{
public:
	void Init(quattro *pmodule, int n);
	void process(int forceStep, float deltaTime, float trnsp);
	void reset(float deltaTime);

private:
	enum STEPMODE
	{
		RESET,
		NORMAL,
		SKIP
	};
	int curStep;
	int stripID;
	quattro *pModule = NULL;
	dsp::SchmittTrigger resetTrig;
	SchmittTrigger2 clockTrigger;
	void beginPulse(bool silent = true, float trnsp = MIDDLE_C);
	void endPulse();
	STEPMODE getStepMode();
	void move_next();
	PulseGenerator2 resetPulseGuard;
	bool resetting;
	const float pulseTime = 0.001;
	int prenotazioneDiChiamata;
	bool moving_bwd;
	int getDirection();
	void reset_curstep(int movement);
};

struct quattro : Module
{
	enum ParamIds
	{
		VOLTAGE_1,
		MODE = VOLTAGE_1 + QUATTRO_NUM_STEPS,
		STRIPSEL_1 = MODE + QUATTRO_NUM_STEPS,
		DIRECTION1 = STRIPSEL_1 + QUATTRO_NUM_STEPS,
		M_RESET = DIRECTION1 + NUM_STRIPS,
		RANGE,
		NUM_PARAMS = RANGE + outputRange::NUMSLOTS
	};
	enum InputIds
	{
		SETSTEP1,
		RESET1 = SETSTEP1 + QUATTRO_NUM_STEPS,
		DIRECTION_IN1 = RESET1 + NUM_STRIPS,
		CLOCK1 = DIRECTION_IN1 + NUM_STRIPS,
		MRESET_IN = CLOCK1 + NUM_STRIPS,
		RANDOMIZONE,
		RANGE_IN,
		TRANSPOSE_IN = RANGE_IN + outputRange::NUMSLOTS,
		NUM_INPUTS 
	};
	enum OutputIds
	{
		CURSTEP1,
		CV1 = CURSTEP1 + QUATTRO_NUM_STEPS,
		GATE1 = CV1 + NUM_STRIPS,
		NUM_OUTPUTS = GATE1 + NUM_STRIPS
	};
	enum LightIds
	{
		LEDSTRIP1,
		LEDSTRIP2 = LEDSTRIP1 + QUATTRO_NUM_STEPS,
		LEDSTRIP3 = LEDSTRIP2 + QUATTRO_NUM_STEPS,
		LEDSTRIP4 = LEDSTRIP3 + QUATTRO_NUM_STEPS,
		NUM_LIGHTS = LEDSTRIP4 + QUATTRO_NUM_STEPS
	};

	quattro() : Module()
	{
		pWidget = NULL;
		theRandomizer = 0;
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		for(int k = 0; k < QUATTRO_NUM_STEPS; k++)
		{
			configParam(MODE + k, 0.0, 2.0, 1.0);
			configParam(VOLTAGE_1 + k, 0.0, 1.0, 0.5);
			configParam(STRIPSEL_1 + k, 0.0, 3.0, 0.0);
		}

		orng.configure(this, RANGE);
		for(int k = 0; k < NUM_STRIPS; k++)
		{
			configParam(DIRECTION1 + k, 0.0, 2.0, 0.0);
			strip[k].Init(this, k);
		}
		on_loaded();
	}

	void process(const ProcessArgs &args) override;
	void setWidget(quattroWidget *pwdg) { pWidget = pwdg; }
	static int ledStrips[4];
	int theRandomizer;
	outputRange orng;
	void onReset() override { load(); }
	void onRandomize() override { load(); }
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
	void QuantizePitch();

private:
	quattroWidget *pWidget;
	void randrandrand();
	void randrandrand(int action);
	void on_loaded();
	void load();
	void reset(float deltaTime);
	quattroStrip strip[NUM_STRIPS];
	dsp::SchmittTrigger masterReset;
	dsp::SchmittTrigger setStepTrig[QUATTRO_NUM_STEPS];
	dsp::SchmittTrigger rndTrigger;
};
