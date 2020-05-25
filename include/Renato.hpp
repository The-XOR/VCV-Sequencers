#include "common.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "rntSequencer.hpp"

struct Renato;
struct RenatoWidget : SequencerWidget
{
public:
	void onMenu(int action);
	RenatoWidget(Renato *module);
	void resetAccess();
	enum MENUACTIONS
	{
		RANDOMIZE_PITCH = 0x01,
		RANDOMIZE_GATEX = 0x02,
		RANDOMIZE_GATEY = 0x04,
		RANDOMIZE_ACCESS = 0x08,
		RANDOMIZE_LAQUALUNQUE = 0x10,
		QUANTIZE_PITCH
	};
private:

	struct RandomizeSubItemItem : MenuItem
	{
		RandomizeSubItemItem(Module *ren, const char *title, int action);

		int randomizeDest;
		Renato *renato;
		void onAction(const event::Action &e) override;
	};

	struct RandomizeItem : ui::MenuItem
	{
	public:
		RandomizeItem(Module *ren)
		{
			renato = ren;
			text = "Force the hand of chance";
			rightText = RIGHT_ARROW;
		};
		Menu *createChildMenu() override
		{
			Menu *sub_menu = new Menu;
			sub_menu->addChild(new RandomizeSubItemItem(renato, "Ov Pitch", RANDOMIZE_PITCH));
			sub_menu->addChild(new RandomizeSubItemItem(renato, "Ov Gate Ov X", RANDOMIZE_GATEX));
			sub_menu->addChild(new RandomizeSubItemItem(renato, "Ov Gate Ov Y", RANDOMIZE_GATEY));
			sub_menu->addChild(new RandomizeSubItemItem(renato, "Ov Access", RANDOMIZE_ACCESS));
			sub_menu->addChild(new RandomizeSubItemItem(renato, "Ov Power", RANDOMIZE_LAQUALUNQUE));
			return sub_menu;
		}

	private:
		Module *renato;
	};

	Menu *addContextMenu(Menu *menu) override;
};

struct Renato : Module
{
	enum ParamIds
	{
		COUNTMODE_X, COUNTMODE_Y,
		SEEKSLEEP,
		ACCESS_1,
		GATEX_1 = ACCESS_1 + 16,
		GATEY_1 = GATEX_1 + 16,
		VOLTAGE_1 = GATEY_1 + 16,
		M_RESET = VOLTAGE_1 + 16,
		RANGE,
		NUM_PARAMS = RANGE + cvStrip::CVSTRIP_PARAMS
	};

	enum InputIds
	{
		XCLK,
		YCLK,
		RESET,
		ACCESS_IN1,
		GATEX_IN1 = ACCESS_IN1 + 16,
		GATEY_IN1 = GATEX_IN1 + 16,
		RANDOMIZONE = GATEY_IN1 + 16,
		INIT_IN,
		NOT_ACC,
		NOT_X,
		NOT_Y,
		RANGE_IN,
		NUM_INPUTS = RANGE_IN + cvStrip::CVSTRIP_INPUTS
	};

	enum OutputIds
	{
		CV,
		XGATE, YGATE,
		CV_OUTSTEP1,
		NUM_OUTPUTS = CV_OUTSTEP1 + 16
	};

	enum LightIds
	{
		LED_GATEX, LED_GATEY,
		LED_1,
		NUM_LIGHTS = LED_1 + 16
	};

	void setWidget(RenatoWidget *pwdg) { pWidget = pwdg; }
	void QuantizePitch();

	Renato() : Module()
	{
		theRandomizer = 0;
		pWidget = NULL;
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(Renato::COUNTMODE_Y, 0.0, 2.0, 0.0);
		configParam(Renato::COUNTMODE_X, 0.0, 2.0, 0.0);
		configParam(Renato::SEEKSLEEP, 0.0, 1.0, 0.0);
		for(int r = 0; r < 4; r++)
		{
			for(int c = 0; c < 4; c++)
			{
				int n = c + r * 4;
				configParam(Renato::ACCESS_1 + n, 0.0, 1.0, 1.0);
				configParam(Renato::GATEX_1 + n, 0.0, 1.0, 1.0);
				configParam(Renato::GATEY_1 + n, 0.0, 1.0, 1.0);
				configParam(Renato::VOLTAGE_1 + n, 0.0, 1.0, 0.0, "Voltage", "V");
			}
		}
		cvs.configure(this, NUM_PARAMS - cvStrip::CVSTRIP_PARAMS);

		onReset();
	}

	void process(const ProcessArgs &args) override;
	void onReset() override { load(); }

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
	bool _accessX(int p) { return _access(xy(p, seqY.Position())); }
	bool _accessY(int p) { return _access(xy(seqX.Position(), p)); }

	int theRandomizer;
	cvStrip cvs;

private:	
	void randrandrand();
	void randrandrand(int action);

private:
	RenatoWidget *pWidget;
	dsp::SchmittTrigger resetTrigger;
	dsp::SchmittTrigger resetAccess;
	dsp::SchmittTrigger accessRndTrigger;
	dsp::SchmittTrigger gatexRndTrigger;
	dsp::SchmittTrigger gateyRndTrigger;
	dsp::SchmittTrigger masterReset;
	void on_loaded();
	void load();
	void led(int n);
	void setOut(int n, bool on);
	int xy(int x, int y) { return 4 * y + x; }
	inline bool getLogic(int inputID, bool cond) {return inputs[inputID].getNormalVoltage(0.0) > 0 ? !cond : cond;}
	inline bool _access(int n) {return getLogic(NOT_ACC, getModulableSwitch(this, ACCESS_1 + n, ACCESS_IN1 + n) > 0); }
	inline bool _gateX(int n) {return getLogic(NOT_X, getModulableSwitch(this, GATEX_1 + n, GATEX_IN1 + n) > 0); }
	inline bool _gateY(int n) {return getLogic(NOT_Y, getModulableSwitch(this, GATEY_1 + n, GATEY_IN1 + n) > 0); }
	rntSequencer seqX;
	rntSequencer seqY;
};
