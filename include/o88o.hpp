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

/*
 "1. Any live cell with fewer than two live neighbours dies, as if caused by under-population."
 "2. Any live cell with two or three live neighbours lives on to the next generation."
 "3. Any live cell with more than three live neighbours dies, as if by over-population."
 "4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction."
   */
struct playground
{
	void setWorld(bool (*p)[NUM_o88o_RECT])
	{
		world = p;
	}

	bool next_generation()
	{
		bool grid2[NUM_o88o_RECT + 1][NUM_o88o_RECT + 1] = {};
		for(int x = 0; x < NUM_o88o_RECT; x++)
			for(int y = 0; y < NUM_o88o_RECT; y++)
				grid2[x][y] = world[x][y];

		bool deadWorld = true;
		for(int a = 1; a < NUM_o88o_RECT; a++)
		{
			for(int b = 1; b < NUM_o88o_RECT; b++)
			{
				int life = 0;
				for(int c = -1; c < 2; c++)
				{
					for(int d = -1; d < 2; d++)
					{
						if(!(c == 0 && d == 0))
						{
							if(grid2[a + c][b + d])
								++life;
						}
					}
				}
				if(life < 2)
				{
					if(world[a][b])
						deadWorld = false;
					world[a][b] = false;
				} else if(life == 3)
				{
					if(!world[a][b])
						deadWorld = false;
					world[a][b] = true;
				} else if(life > 3)
				{
					if(world[a][b])
						deadWorld = false;
					world[a][b] = false;
				}
			}
		}

		return deadWorld;
	}

	bool (*world)[NUM_o88o_RECT] = NULL;
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
		pg.setWorld(ThePattern);
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
		json_t *rndJson = json_object_get(root, "ptn");
		if(rndJson)
			curPtn = json_integer_value(rndJson);

		for(int r = 0; r < NUM_o88o_RECT;r++)
		{
			for(int c = 0; c < NUM_o88o_RECT;c++)
			{
				char n[30];
				sprintf(n, "R%iC%i",r,c);
				json_t *rndJson = json_object_get(root, n);
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
		json_object_set_new(rootJ, "ptn", json_integer(curPtn));
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
	inline bool getCell(int r, int c) { return invert_active ? !ThePattern[r][c] : ThePattern[r][c]; }
	bool ThePattern[NUM_o88o_RECT][NUM_o88o_RECT];

	dsp::SchmittTrigger generationTrigger;
	dsp::SchmittTrigger generationBtn;
	dsp::PulseGenerator emptyTrig;
	playground pg;

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
