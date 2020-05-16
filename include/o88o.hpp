#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;
#define NUM_o88o_RECT   8

struct o88o;
struct o88oWidget : SequencerWidget
{
	o88oWidget(o88o *module);
	void SetPattern(int ptn);
};

struct o88o : Module
{
public:
	enum ParamIds
	{
		M_RESET,
		GATE,
		SWITCH_BACKW,
		SWITCH_VERT,
		SWITCH_LOOP,
		SWITCH_INVERT,
		FIRSTROW,
		LASTCOL,
		FIRSTCOL,
		PATTERN,
		LASTROW, 
		PTN_INC, 
		PTN_DEC,
		RANDOMIZE,
		LED_GATE,
		GENERATE,
		NUM_PARAMS
	};
	enum InputIds
	{
		RESET_IN,
		CLOCK_IN,
		GATE_IN,
		FIRSTROW_IN,
		LASTROW_IN,
		FIRSTCOL_IN,
		LASTCOL_IN, 
		PATTERN_IN,
		SWBACK_IN,
		SWVERT_IN,
		SWLOOP_IN,
		SWINVERT_IN,
		RANDOMIZE_IN,
		GENERATION_IN,
 		NUM_INPUTS
	};
	enum OutputIds
	{
		GATE_OUT,
		CURROW_OUT,
		CURCOL_OUT,
		TRIGGER_EMPTY,
		NUM_OUTPUTS
	};
	enum LightIds
	{
		NUM_LIGHTS
	};

	o88o() : Module()
	{
		pWidget = NULL;
		curPtn = 0;
		loadPattern();
		lastCol = lastRow = NUM_o88o_RECT - 1;
		firstCol = firstRow = 0;
		invert_active = false;
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PATTERN, 0, NUM_PATTERNS-1, 0, "Pattern", "#",  0.f, 1.f, 1.f);
		configParam(FIRSTROW, 0, NUM_o88o_RECT-1, 0, "First Row", "#", 0.f, 1.f, 1.f);
		configParam(LASTROW, 0, NUM_o88o_RECT-1, NUM_o88o_RECT-1, "Last Row", "#", 0.f, 1.f, 1.f);
		configParam(FIRSTCOL, 0, NUM_o88o_RECT-1, 0, "First Column", "#",  0.f, 1.f, 1.f);
		configParam(LASTCOL, 0, NUM_o88o_RECT-1, NUM_o88o_RECT-1, "Last Column", "#",  0.f, 1.f, 1.f);
		reset();
	}

	void dataFromJson(json_t *root) override
	{
			Module::dataFromJson(root);
		for(int r = 0; r < NUM_o88o_RECT;r++)
		{
			for(int c = 0; c < NUM_o88o_RECT;c++)
			{
				char n[30];
				sprintf(n, "R%iC%i",r,c);
				json_t *rndJson = json_object_get(root, "o88o");
				if(rndJson)
					ThePattern[r][c] =  json_integer_value(rndJson) > 0;
				else
					ThePattern[r][c] = false;				
			}	
		}
	}

	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();
		for(int r = 0; r < NUM_o88o_RECT;r++)
		{
			for(int c = 0; c < NUM_o88o_RECT;c++)
			{
				char n[30];
				sprintf(n, "R%iC%i",r,c);
				json_object_set_new(rootJ, n, json_integer(ThePattern[r][c] ? 1 : 0));
			}	
		}
		return rootJ;
	}

	void process(const ProcessArgs &args) override;
	NVGcolor getCellColor(int r, int c);
	int patternNumber() const { return curPtn; }
	void setWidget(o88oWidget *pwdg) { pWidget = pwdg; }

	void toggleCell(int r, int c)
	{
		ThePattern[r][c] = !ThePattern[r][c];
	}

private:
	inline bool isCellEnabled(int r, int c) 	{return r >= firstRow && r <= lastRow && c >= firstCol && c <= lastCol; }
	void close_gate();
	void next_step();
	void reset();
	void getPatternLimits();
	void out_position();
	void loadPattern();
	void open_gate();
	void next_column(bool vert, bool back, bool loop);
	void next_row(bool vert, bool back, bool loop);
	void process_keys();
	void randPattrn();
	inline int getCell(int r, int c) { return invert_active ? ThePattern[r][c] == 0 : ThePattern[r][c]; }
	int ThePattern[NUM_o88o_RECT][NUM_o88o_RECT];

	dsp::SchmittTrigger generationTrigger;
	//playground pg;
	dsp::SchmittTrigger generationBtn;
	dsp::PulseGenerator emptyTrig;

	enum CELLSTATUS
	{
		OFF = 0,
		DISABLED,
		ENABLED,
		CURRENT,
		CURRENT_DISABLED,
		CURRENT_OFF,
		NUM_COLORS
	};
	static const int NUM_PATTERNS = 51;
	static NVGcolor cellColors[NUM_COLORS];
	static int TheMatrix[NUM_PATTERNS][NUM_o88o_RECT][NUM_o88o_RECT];
	int curPtn;
	int curCol, curRow;
	int firstCol, lastCol;
	int firstRow, lastRow;
	bool invert_active;
	SchmittTrigger2 clockTrigger;
	dsp::SchmittTrigger masterReset;
	dsp::SchmittTrigger btninc;
	dsp::SchmittTrigger btndec;
	o88oWidget *pWidget;
	dsp::SchmittTrigger rndTrigger;
	dsp::SchmittTrigger rndBtnTrig;

};
