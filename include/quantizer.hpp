#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;
struct Quantizer;
struct midyQuant;
struct ScaleChoice;
struct scaleRootChoice;
struct midiQtzDriverChoice;
struct midiQtzChannelChoice;
struct midiQtzDeviceChoice;
struct quantizeModule;

static std::vector<const char *> QuantizerRootNotes = {"C", "C#", "D", "D#", "E", "F", "F#","G","G#","A","A#","B"};

struct MidiOutput : midi::Output 
{
	bool lastGates[128];

	MidiOutput() 
	{
		reset();
	}

	void reset() 
	{
		for (int note = 0; note < 128; note++) 
		{
			lastGates[note] = false;
		}
	}

	void panic() 
	{
		reset();
		for (int note = 0; note < 128; note++) 
		{
			// Note off
			midi::Message m;
			m.setStatus(0x8);
			m.setNote(note);
			m.setValue(0);
			sendMessage(m);
		}
	}

	void sendNote(bool gate, int note, int vel) 
	{
		if (gate && !lastGates[note]) 
		{
			// Note on
			midi::Message m;
			m.setStatus(0x9);
			m.setNote(note);
			m.setValue(vel);
			sendMessage(m);
		} else if (!gate && lastGates[note]) 
		{
			// Note off
			midi::Message m;
			m.setStatus(0x8);
			m.setNote(note);
			m.setValue(vel);
			sendMessage(m);
		}
		lastGates[note] = gate;
	}
};

struct qtzrDisplay : LedDisplay 
{
	ScaleChoice *scaleChoice;
	LedDisplaySeparator *separator;
	scaleRootChoice *rootChoice;
	qtzrDisplay();

	void CreateInterface(quantizeModule *module);
};

struct qtzrMidiDisplay : LedDisplay 
{
	midiQtzDriverChoice *driverChoice;
	midiQtzDeviceChoice *deviceChoice;
	LedDisplaySeparator *driverSeparator;
	LedDisplaySeparator *deviceSeparator;
	midiQtzChannelChoice *channelChoice;
	qtzrMidiDisplay();

	void CreateInterface(midyQuant *module);
};

struct QScale
{
	const char *name;
	std::vector<int> notes;
	QScale(const char *n, std::vector<int> ns)
	{
		name = n;
		notes = ns;
	}
};

struct QuantizerWidget : ModuleWidget
{
	QuantizerWidget(Quantizer *module);
	
private:
	qtzrDisplay *display;
};

struct quantizeModule
{
public:
	void SetRoot(int n) {root = n; }
	void SetScale(int n) {scale = n; calcScale();}
	inline int getScale() {return scale;}
	inline int getRoot() {return root;}
	inline const std::vector<QScale> &getAvailableScales() {return availableScales;}

protected:
	quantizeModule() 
	{
		initializeScale();
		root = 0;
		SetScale(0);
	}

	json_t *qtzToJson() 
	{
		json_t *rootJ = json_object();
		json_object_set_new(rootJ, "scale", json_integer(scale));
		json_object_set_new(rootJ, "root", json_integer(root));
		return rootJ;
	}

	void qtzFromJson(json_t *rootJ) 
	{
		json_t *bpmJson = json_object_get(rootJ, "scale");
		if(bpmJson)
			scale = json_integer_value(bpmJson);
		bpmJson = json_object_get(rootJ, "root");
		if(bpmJson)
			root = json_integer_value(bpmJson);
	}
	int scale;
	int root;

	void calcScale()
	{
		currentScale.clear();
		for(int k = 0; k < int(availableScales.at(scale).notes.size()); k++)
			currentScale.push_back(SEMITONE * (availableScales.at(scale).notes.at(k)-12));
		for(int k = 0; k < int(availableScales.at(scale).notes.size()); k++)
			currentScale.push_back(SEMITONE * availableScales.at(scale).notes.at(k));
	}

	void initializeScale()
	{
		availableScales.push_back(QScale("Chromatic"				,  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 }));
		availableScales.push_back(QScale("Octaves"					,  { 0 }));
		availableScales.push_back(QScale("Fifths"					,  { 0, 7 }));
		availableScales.push_back(QScale("PentAmbig"				,  { 0, 2, 7 }));
		availableScales.push_back(QScale("Fourths"					,  { 0, 5, 10 }));
		availableScales.push_back(QScale("cbm_q0"					,  { 0, 4, 5, 9 }));
		availableScales.push_back(QScale("cbm_q2"					,  { 0, 3, 5, 8 }));
		availableScales.push_back(QScale("Tristan"					,  { 0, 3, 6, 10 }));
		availableScales.push_back(QScale("cbm_p0"					,  { 0, 2, 5, 7, 11 }));
		availableScales.push_back(QScale("PentaMaj"					,  { 0, 2, 4, 7, 9 }));
		availableScales.push_back(QScale("PentaMin"					,  { 0, 2, 3, 7, 8 }));
		availableScales.push_back(QScale("Slendro"					,  { 0, 2, 5, 7, 10 }));
		availableScales.push_back(QScale("Pelog"					,  { 0, 1, 3, 7, 8 }));
		availableScales.push_back(QScale("Miyako-bushi"				,  { 0, 1, 4, 7, 8 }));
		availableScales.push_back(QScale("Blues"					,  { 0, 3, 5, 6, 7, 10 }));
		availableScales.push_back(QScale("SixtoneSym"				,  { 0, 1, 4, 5, 8, 9 }));
		availableScales.push_back(QScale("Petrushka"				,  { 0, 1, 4, 6, 7, 10 }));
		availableScales.push_back(QScale("Prometheus"				,  { 0, 2, 4, 6, 9, 10 }));
		availableScales.push_back(QScale("Japanese"					,  { 0, 1, 5, 7, 8, 10 }));
		availableScales.push_back(QScale("Wholetone"				,  { 0, 2, 4, 6, 8, 10 }));
		availableScales.push_back(QScale("Augmented"				,  { 0, 3, 4, 6, 8, 11 }));
		availableScales.push_back(QScale("AugmentedMaj"				,  { 0, 3, 4, 5, 7, 8, 11 }));
		availableScales.push_back(QScale("Major"					,  { 0, 2, 4, 5, 7, 9, 11 }));
		availableScales.push_back(QScale("Minor"					,  { 0, 2, 3, 5, 7, 9, 11 }));
		availableScales.push_back(QScale("BebopMaj"					,  { 0, 2, 4, 5, 7, 8, 9 }));
		availableScales.push_back(QScale("BebopMin"					,  { 0, 3, 4, 5, 7, 9, 10 }));
		availableScales.push_back(QScale("BebopMelMin"				,  { 0, 2, 3, 5, 7, 8, 9 }));
		availableScales.push_back(QScale("HarmonicMaj"				,  { 0, 2, 4, 5, 7, 8, 11 }));
		availableScales.push_back(QScale("HarmonicMin"				,  { 0, 2, 3, 5, 7, 8, 11 }));
		availableScales.push_back(QScale("NeopolitanMaj"			,  { 0, 1, 3, 5, 7, 9, 11 }));
		availableScales.push_back(QScale("NeopolitanMin"			,  { 0, 1, 3, 5, 7, 8, 11 }));
		availableScales.push_back(QScale("Dorian"					,  { 0, 2, 3, 5, 7, 9, 10 }));
		availableScales.push_back(QScale("Phrygian"					,  { 0, 1, 3, 5, 7, 8, 10 }));
		availableScales.push_back(QScale("Lydian"					,  { 0, 2, 4, 6, 7, 9, 11 }));
		availableScales.push_back(QScale("Aeolian"					,  { 0, 2, 3, 5, 7, 8, 10 }));
		availableScales.push_back(QScale("Mixolydian"				,  { 0, 2, 4, 5, 7, 9, 10 }));
		availableScales.push_back(QScale("Locrian"					,  { 0, 1, 3, 5, 6, 8, 10 }));
		availableScales.push_back(QScale("Algerian"					,  { 0, 2, 3, 5, 6, 7, 8, 11 }));
		availableScales.push_back(QScale("Enigmatic"				,  { 0, 1, 4, 6, 8, 10, 11 }));
		availableScales.push_back(QScale("Arabic"					,  { 0, 1, 4, 5, 7, 8, 11 }));
		availableScales.push_back(QScale("Todi"						,  { 0, 1, 3, 6, 7, 8, 11 }));
		availableScales.push_back(QScale("Purvi"					,  { 0, 1, 4, 6, 7, 8, 11 }));
		availableScales.push_back(QScale("OvertoneDom"				,  { 0, 2, 4, 6, 7, 9, 10 }));
		availableScales.push_back(QScale("Hindu"					,  { 0, 2, 4, 5, 7, 8, 10 }));
		availableScales.push_back(QScale("Altered"					,  { 0, 1, 3, 4, 6, 8, 10 }));
		availableScales.push_back(QScale("PhrygianDom"				,  { 0, 1, 4, 5, 7, 8, 10 }));
		availableScales.push_back(QScale("Octatonic"				,  { 0, 1, 3, 4, 6, 7, 9, 10 }));
		availableScales.push_back(QScale("Lydian_Minor"				,  { 0, 2, 4, 6, 7, 8, 10 }));
		availableScales.push_back(QScale("Rag_Madhukant"			,  { 0, 2, 3, 6, 7, 9, 10 }));
		availableScales.push_back(QScale("Rag_Madhuvanti"			,  { 0, 2, 3, 6, 7, 9, 11 }));
		availableScales.push_back(QScale("Rag_Ahir_Bhairav"			,  { 0, 1, 4, 5, 7, 9, 10 }));
		availableScales.push_back(QScale("Rag_Arnand_Bhairav"		,  { 0, 1, 4, 5, 7, 9, 11 }));
		availableScales.push_back(QScale("BebopDom"					,  { 0, 2, 4, 5, 7, 10, 11 }));
		availableScales.push_back(QScale("Saranga"					,  { 0, 2, 3, 6, 7, 10, 11 }));
		availableScales.push_back(QScale("Sambah"					,  { 0, 2, 3, 4, 7, 8, 10 }));
		availableScales.push_back(QScale("HungarianMaj"				,  { 0, 3, 4, 6, 7, 9, 10 }));
		availableScales.push_back(QScale("InstantJazz"				,  { 0, 1, 3, 6, 7, 9, 11 }));
		availableScales.push_back(QScale("Jeth"						,  { 0, 2, 3, 5, 6, 9, 11 }));
		availableScales.push_back(QScale("MixoBlues"				,  { 0, 3, 4, 5, 6, 7, 10 }));
		availableScales.push_back(QScale("HungarianMin"				,  { 0, 2, 3, 6, 7, 8, 11 }));
		availableScales.push_back(QScale("Diminished"				,  { 0, 2, 3, 5, 6, 8, 9, 11 }));
		availableScales.push_back(QScale("Bhairav_That"				,  { 0, 1, 2, 5, 7, 8, 11 }));
		availableScales.push_back(QScale("SpanishEightTone"			,  { 0, 1, 3, 4, 5, 6, 8, 10 }));
		availableScales.push_back(QScale("Ninetone"					,  { 0, 2, 3, 4, 6, 7, 8, 9, 11 }));
	}

	float quantize(float semitone)
	{
		int ptr = 0;
		for(int k = 1; k < (int)currentScale.size(); k++)
		{
			if(fabs(semitone - currentScale[k]) >= fabs(semitone - currentScale[ptr]))
				break;
			else
				ptr = k;
		}
		return currentScale[ptr]+ SEMITONE * root;
	}

	std::vector<QScale> availableScales;
	std::vector<float> currentScale;
};

struct Quantizer : Module, quantizeModule
{
	enum ParamIds
	{
		NUM_PARAMS
	};
	enum InputIds
	{
		IN_1,
		NUM_INPUTS
	};
	enum OutputIds
	{
		OUT_1,
		OUT_NOTRANSPOSE,
		OUT_OCTAVE,
		NUM_OUTPUTS
	};
	enum LightIds
	{
		NUM_LIGHTS
	};

	Quantizer() : Module(), quantizeModule()
	{		
		pWidget = NULL;
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);		
	}

	void process(const ProcessArgs &args) override;

	json_t *dataToJson() override
	{
		return qtzToJson();
	}

	void dataFromJson(json_t *rootJ) override
	{
		qtzFromJson(rootJ);
		on_loaded();
	}

	void setWidget(QuantizerWidget *p) {pWidget = p;}

private:
	void on_loaded();
	void load() {}
	QuantizerWidget *pWidget;
};

struct midyQuantWidget : ModuleWidget
{
	enum MENUACTIONS
	{
		MIDIPANIC
	};

	midyQuantWidget(midyQuant *module);
	void appendContextMenu(ui::Menu *menu) override
	{
		menu->addChild(createMenuLabel("*** A la carte ***"));
		addContextMenu(menu);
	}
	void onMenu(int action);
private:	
	Menu *addContextMenu(Menu *menu) ;

private:
	qtzrDisplay *display;
	qtzrMidiDisplay *midiDisplay;
};

struct midyQuant : Module, quantizeModule
{
	enum ParamIds
	{
		NUM_PARAMS
	};
	enum InputIds
	{
		CV,
		GATE,
		VEL,
		RESET,
		NUM_INPUTS
	};
	enum OutputIds
	{
		NUM_OUTPUTS
	};
	enum LightIds
	{
		NUM_LIGHTS
	};

	midyQuant() : Module(), quantizeModule()
	{		
		pWidget = NULL;
		note_playing = -1;
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}

	void process(const ProcessArgs &args) override;

	json_t *dataToJson() override
	{
		json_t *rootJ = qtzToJson();
		json_object_set_new(rootJ, "midi", midiOutput.toJson());
		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override
	{
		qtzFromJson(rootJ);
		json_t *bpmJson = json_object_get(rootJ, "midi");
		if (bpmJson)
			midiOutput.fromJson(bpmJson);

		on_loaded();
	}

	void onReset() override 
	{
		note_playing = -1;
		midiOutput.reset();
	}

	void setWidget(midyQuantWidget *p) {pWidget = p;}
	MidiOutput midiOutput;

private:
	void on_loaded();
	void load() {}
	midyQuantWidget *pWidget;
	SchmittTrigger2 gate;
	dsp::SchmittTrigger resetTrigger;
	int note_playing;
};