#pragma once
#include "common.hpp"
#include <chrono>
#define BPM_MINVALUE (10.0)
#define BPM_MAXVALUE (300.0)
#define PWM_MINVALUE (0.05)
#define PWM_MAXVALUE (0.95)
#define SWING_MINVALUE (0.0)
#define SWING_MAXVALUE (0.5)

#define OUT_SOCKETS (21)
#ifdef ARCH_MAC 
typedef std::chrono::time_point<std::chrono::steady_clock> fuck_mac_os;
#else
typedef std::chrono::time_point<std::chrono::system_clock> fuck_mac_os;
#endif
struct PwmClock;
struct PwmClockWidget : SequencerWidget
{
	PwmClockWidget(PwmClock *module);
	void SetBpm(float bpmint);
	void onHoverKey(const event::HoverKey &e) override;

};

struct SA_TIMER	//sample accurate version
{
	float Reset()
	{
		prevTime = curTime = APP->engine->getSampleTime();
		return Begin();
	}

	void RestartStopWatch() { stopwatch = 0; }
	float Begin()
	{
		RestartStopWatch();
		return totalPulseTime = 0;
	}
	float Elapsed() { return totalPulseTime; }
	float StopWatch() { return stopwatch; }

	float Step()
	{
		curTime += APP->engine->getSampleTime();
		float deltaTime = curTime - prevTime;
		prevTime = curTime;
		totalPulseTime += deltaTime;
		stopwatch += deltaTime;
		return deltaTime;
	}

private:
	float curTime;
	float prevTime;
	float totalPulseTime;
	float stopwatch;
};

struct MIDICLOCK_TIMER
{
	void reset()
	{
		midiClockAccum = midiClockCounter = 0;
		lastclockpulse = std::chrono::high_resolution_clock::now();
		bpm = BPM_MINVALUE;
		resetStat();	
	}

	void resetAccum() { midiClockAccum = 0; }
	inline uint64_t clockCounter() { return midiClockAccum; }
	float getBpm(float trigger)
	{
		if (midiClock.process(trigger))
		{
			midiClockAccum++;
			if ((midiClockCounter++ % 24) == 0)
			{
				midiClockCounter = 0;
				fuck_mac_os now = std::chrono::high_resolution_clock::now();
				float elapsed_msec = std::chrono::duration_cast<std::chrono::microseconds>(now - lastclockpulse).count();
				if (elapsed_msec > 0.0)
				{
					if (addSample(now, elapsed_msec))
						bpm = clamp(readStat(), (float)BPM_MINVALUE, (float)BPM_MAXVALUE);
					lastclockpulse = now;
				}
			}
		}

		return bpm;
	}

	private:
		dsp::SchmittTrigger midiClock;
		int midiClockCounter;
		fuck_mac_os lastclockpulse;
		float bpm = BPM_MINVALUE;
		int meanCalcSamples;
		float meanCalcTempValue;
		fuck_mac_os meanCalcStart;
		uint64_t midiClockAccum;
		void resetStat()
		{
			meanCalcStart = std::chrono::high_resolution_clock::now();
			meanCalcSamples = 0;
			meanCalcTempValue = 0.f;
		}

		bool addSample(fuck_mac_os now, float v)
		{
			meanCalcSamples++;
			meanCalcTempValue += v;
			return std::chrono::duration_cast<std::chrono::milliseconds>(now - meanCalcStart).count() >= 500; // aggiorna bpm ogni 500msec
		}

		float readStat()
		{
			float mean = meanCalcTempValue / meanCalcSamples;  // elapsed_time medio
			float rv;
			if(mean > 0.000001)
				rv = (1000000.0f / mean / 24.0f) * 60.f;
			resetStat();
			return rv;
		}
};

struct PwmClock : Module
{
	enum ParamIds
	{
		BPM_INC, BPM_DEC,
		PWM, BPM, BPMDEC,
		SWING,
		OFFON,
		PULSE,
		FOLLOWF8,
		NUM_PARAMS
	};
	enum InputIds
	{
		RESET,
		EXT_BPM,
		PWM_IN,
		SWING_IN,
		REMOTE_IN,
		MIDI_STOP,
		MIDI_START,
		MIDI_CLOCK,
		MIDI_CONTINUE,
		PULSE_IN,
		NUM_INPUTS
	};

	enum OutputIds
	{
		OUT_1,
		ONSTOP = OUT_1 + OUT_SOCKETS,
		NUM_OUTPUTS
	};

	enum LightIds
	{
		ACTIVE,
		NUM_LIGHTS
	};

	PwmClock() : Module()
	{
		pWidget = NULL;
		pendingKey = 0;
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PwmClock::BPM_INC, 0.0, 1.0, 0.0);
		configParam(PwmClock::BPM_DEC, 0.0, 1.0, 0.0);
		configParam(PwmClock::BPMDEC, 0.0, 9.0, 0.0);
		configParam(PwmClock::BPM, BPM_MINVALUE, BPM_MAXVALUE, 120.0, "BPM","Beats per minute");
		configParam(PwmClock::OFFON, 0.0, 1.0, 0.0);
		configParam(PwmClock::SWING, SWING_MINVALUE, SWING_MAXVALUE, SWING_MINVALUE, "Swing", "%", 0, 100);
		configParam(PwmClock::PWM, PWM_MINVALUE, PWM_MAXVALUE, 0.5, "Duty cycle", "%", 0, 100);

		on_loaded();
	}
	void process(const ProcessArgs &args) override;

	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();
		json_t *bpmJson = json_integer((int)bpm_integer);
		json_object_set_new(rootJ, "bpm_integer", bpmJson);
		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override
	{
		json_t *bpmJson = json_object_get(rootJ, "bpm_integer");
		if(bpmJson)
			bpm_integer = json_integer_value(bpmJson);
		on_loaded();
	}

	void onReset() override
	{
		bpm_integer = 120;

		load();
	}
	void onRandomize() override {}
	void setWidget(PwmClockWidget *pwdg) { pWidget = pwdg; }
	float bpm;
	float swing;
	int pendingKey;

private:
	dsp::SchmittTrigger btnup;
	dsp::SchmittTrigger btndwn;
	PwmClockWidget *pWidget;
	uint32_t tick = UINT32_MAX;
	int bpm_integer = 120;
	bool current_status;
	SchmittTrigger2 resetTrigger;
	SchmittTrigger2 pulseTrigger;
	SchmittTrigger2 onTrigger;
	SchmittTrigger2 offTrigger;
	dsp::SchmittTrigger manualTrigger;
	dsp::PulseGenerator onStopPulse;
	dsp::PulseGenerator onManualStep;
	bool optimize_manualStep;

	void process_keys();
	void updateBpm(bool externalMidiClock, bool followf8);
	void process_active(const ProcessArgs &args, bool externalMidiClock, bool followf8);
	void process_inactive(const ProcessArgs &args);
	void process_extMidiClock(const ProcessArgs &args);

	inline float getDuration(int n) 	{return odd_beat[n] ? swingAmt[n] : duration[n]; }
	float duration[OUT_SOCKETS];
	float swingAmt[OUT_SOCKETS];
	static int ticks_24ppqn[OUT_SOCKETS];
	bool odd_beat[OUT_SOCKETS];
	dsp::PulseGenerator midiClockTrig[OUT_SOCKETS];
	void on_loaded();
	void load();
	void _reset();
	inline float getPwm() { return clamp(rescale(inputs[PWM_IN].getNormalVoltage(0.0), LVL_OFF, LVL_ON, PWM_MINVALUE, PWM_MAXVALUE) + params[PWM].value, PWM_MINVALUE, PWM_MAXVALUE); }

	inline float getSwing() { return clamp(rescale(inputs[SWING_IN].getNormalVoltage(0.0), LVL_OFF, LVL_ON, SWING_MINVALUE, SWING_MAXVALUE) + params[SWING].value, SWING_MINVALUE, SWING_MAXVALUE); }
	bool isGeneratorActive();
	SA_TIMER sa_timer[OUT_SOCKETS];
	MIDICLOCK_TIMER midiClock;
};
