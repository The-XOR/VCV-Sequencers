#include "../include/pwmClock.hpp"
#include <GLFW/glfw3.h>

int PwmClock::ticks_24ppqn[OUT_SOCKETS] = {
96	,144		,64
,48	,72		,32
,24	,36		,16
,12	,18		,8
,6	,9		,4
,3	,4/*4.5*/		,2
,1/*1.5*/	,2/*2.25*/	,1
};


void PwmClock::on_loaded()
{
	current_status = false;
	bpm = 0;
	swing = 0;
	_reset();
	load();
	midiClock.reset();
}

void PwmClock::_reset()
{
	optimize_manualStep = false;
	for(int k = 0; k < OUT_SOCKETS; k++)
	{
		sa_timer[k].Reset();
		odd_beat[k] = false;
		midiClockTrig[k].reset();
	}
}

void PwmClock::load()
{
	pendingKey = 0;
	updateBpm(false, false);
}

void PwmClock::updateBpm(bool externalMidiClock, bool followf8)
{
	if(followf8)
	{
		bpm = std::numeric_limits<float>::epsilon();
		return;
	}

	bool updated = false;
	float new_bpm;
	if(externalMidiClock)
	{
		new_bpm = midiClock.getBpm(inputs[MIDI_CLOCK].value);
	} else
	{
		new_bpm = (roundf(params[BPMDEC].value) + 10 * bpm_integer) / 10.0;
		new_bpm += (inputs[EXT_BPM].value/LVL_MAX) * BPM_MAXVALUE;
		new_bpm = clamp(new_bpm, BPM_MINVALUE, BPM_MAXVALUE);
	}

	if(bpm != new_bpm)
	{
		updated = true;
		bpm = new_bpm;
		duration[0] = 240.0 / bpm;	// 1/1
		duration[1] = duration[0] + duration[0] / 2.0;
		duration[2] = 2.0 * duration[0] / 3.0;

		for(int k = 1; k < 7; k++)
		{
			duration[3 * k] = duration[3 * (k - 1)] / 2.0;
			duration[3 * k + 1] = duration[3 * (k - 1) + 1] / 2.0;
			duration[3 * k + 2] = duration[3 * (k - 1) + 2] / 2.0;
		}
	}
	float new_swing = getSwing();
	if(updated || new_swing != swing)
	{
		swing = new_swing;
		for(int k = 0; k < OUT_SOCKETS; k++)
			swingAmt[k] = duration[k] + duration[k] * swing;
	}
}

void PwmClock::process_keys()
{
	if(btnup.process(params[BPM_INC].value))
	{
		if(bpm_integer < BPM_MAXVALUE)
		{
			bpm_integer += 1;
			pWidget->SetBpm(bpm_integer);
		}
	} else if(btndwn.process(params[BPM_DEC].value))
	{
		if(bpm_integer > BPM_MINVALUE)
		{
			bpm_integer -= 1;
			pWidget->SetBpm(bpm_integer);
		}
	}
}

void PwmClock::process_active(const ProcessArgs &args, bool externalMidiClock, bool followf8)
{
	onStopPulse.reset();
	onManualStep.reset();
	if(resetTrigger.process(inputs[RESET].value))
	{
		_reset();
		midiClock.reset();
	} else 
	{
		if(externalMidiClock && followf8)
			process_extMidiClock(args);
		else
		{
			for(int k = 0; k < OUT_SOCKETS; k++)
			{
				if(outputs[OUT_1 + k].isConnected())
				{
					float gate_len = getDuration(k) * getPwm();
					sa_timer[k].Step();
					float elps = sa_timer[k].Elapsed();
					if(elps >= getDuration(k))
					{
						elps = sa_timer[k].Reset();
						odd_beat[k] = !odd_beat[k];
					}
					outputs[OUT_1 + k].value = elps <= gate_len ? LVL_ON : LVL_OFF;
				}
			}
		}
	}
}

void PwmClock::process_extMidiClock(const ProcessArgs &args)
{
	uint64_t cc = midiClock.clockCounter();
	float deltaTime = 1.0 / args.sampleRate;

	for(int k = 0; k < OUT_SOCKETS; k++)
	{
		if(outputs[OUT_1 + k].value  > 0.5 && !midiClockTrig[k].process(deltaTime))
			outputs[OUT_1 + k].value = LVL_OFF;
		
		if(outputs[OUT_1 + k].isConnected())
		{
			if((cc % ticks_24ppqn[k]) == 0)
			{
				midiClockTrig[k].trigger(PULSE_TIME);
				outputs[OUT_1 + k].value = LVL_ON;
			}
		}
	}
}

void PwmClock::process_inactive(const ProcessArgs &args)
{
	float deltaTime = 1.0 / args.sampleRate;

	if(current_status && !onStopPulse.process(deltaTime))
		onStopPulse.trigger(PULSE_TIME);

	if(!onManualStep.process(deltaTime))
	{
		if(optimize_manualStep)
		{
			optimize_manualStep = false;
			for(int k = 0; k < OUT_SOCKETS; k++)
				outputs[OUT_1 + k].value = LVL_OFF;

			lights[ACTIVE].value = LED_OFF;

		}
		if((manualTrigger.process(params[PULSE].value) || pulseTrigger.process(inputs[PULSE_IN].value)))
		{
			onManualStep.trigger(PULSE_TIME);
			optimize_manualStep = true;
			for(int k = 0; k < OUT_SOCKETS; k++)
				outputs[OUT_1 + k].value = LVL_ON;

			lights[ACTIVE].value = LED_ON;
		}
	}
	outputs[ONSTOP].value = onStopPulse.process(deltaTime) ? LVL_ON : LVL_OFF;
}

bool PwmClock::isGeneratorActive()
{
	bool active = false;
	if(inputs[REMOTE_IN].isConnected()) // priorita; prioritaria
	{
		pendingKey = 0;
		active = inputs[REMOTE_IN].getNormalVoltage(0.0) > SWITCH_ON;
		if(active && !isSwitchOn(this, OFFON))
		{
			pWidget->params[OFFON]->dirtyValue = params[OFFON].value = 1.0;
		} else if(!active && isSwitchOn(this, OFFON))
		{
			pWidget->params[OFFON]->dirtyValue = params[OFFON].value = 0.0;
		}

	} else if(offTrigger.process(inputs[MIDI_STOP].value))
	{
		pendingKey = 0;
		pWidget->params[OFFON]->dirtyValue = params[OFFON].value = 0.0;
		active = false;
	} else if(onTrigger.process(inputs[MIDI_START].value + inputs[MIDI_CONTINUE].value))
	{
		pendingKey = 0;
		pWidget->params[OFFON]->dirtyValue = params[OFFON].value = 1.0;
		active = true;
	} else
	{
		active = isSwitchOn(this, OFFON);
		if(pendingKey != 0)
		{
			switch(pendingKey)
			{
				case 1:  // segnala lo toggle
					active = !active;
					pWidget->params[OFFON]->dirtyValue = params[OFFON].value = active ? 1.0 : 0.0;
					break;

				case 2:  // segnala lo stoppo
					active = false;
					pWidget->params[OFFON]->dirtyValue = params[OFFON].value = 0.0;
					break;
			}
			pendingKey = 0;
		}
	}
	return active;
}

void PwmClock::process(const ProcessArgs &args)
{
	if(pWidget == NULL)
		return;

	bool active = isGeneratorActive();
	bool externalMidiClock = inputs[MIDI_CLOCK].isConnected();
	bool followf8;
	if(!externalMidiClock)
	{
		process_keys();
		bpm_integer = roundf(params[BPM].value);
		followf8 = false;
	} else
		followf8 = isSwitchOn(this, FOLLOWF8);

	updateBpm(externalMidiClock, followf8);

	if(active)
	{
		process_active(args, externalMidiClock, followf8);
	} else
	{
		process_inactive(args);
	}
	if(active != current_status)
	{
		current_status = active;
		_reset();
		if(active)
		{
			midiClock.resetAccum();
			lights[ACTIVE].value = LED_ON;
		} else
		{
			lights[ACTIVE].value = LED_OFF;
			for(int k = 0; k < OUT_SOCKETS; k++)
				outputs[OUT_1 + k].value = LVL_OFF;
		}
	}
}


PwmClockWidget::PwmClockWidget(PwmClock *module) : SequencerWidget()
{
	if(module != NULL)
		module->setWidget(this);

	CREATE_PANEL(module, this, 15, "res/modules/PwmClock.svg");

	addParam(createParam<UPSWITCH>(Vec(mm2px(14.452), yncscape(104.588 + 4.762, 4.115)), module, PwmClock::BPM_INC));
	addParam(createParam<DNSWITCH>(Vec(mm2px(14.452), yncscape(99.788 + 4.762, 4.115)), module, PwmClock::BPM_DEC));

	SigDisplayWidget *display = new SigDisplayWidget(4, 1);
	display->box.pos = Vec(mm2px(22), RACK_GRID_HEIGHT - mm2px(108 + 4.762));
	display->box.size = Vec(30 + 43, 20);
	if(module != NULL)
		display->value = &module->bpm;
	addChild(display);

	addChild(createParam<BefacoPushBig>(Vec(mm2px(2.937), yncscape(109.841, 8.999)), module, PwmClock::PULSE));
	addInput(createInput<PJ301BPort>(Vec(mm2px(3.309), yncscape(99.175, 8.255)), module, PwmClock::PULSE_IN));

	ParamWidget *pw = createParam<Davies1900hFixWhiteKnobSmall>(Vec(mm2px(50.364), yncscape(100.245 + 4.762, 8)), module, PwmClock::BPMDEC);
	((Davies1900hKnob *)pw)->snap = true;
	addParam(pw);
	pw = createParam<Davies1900hFixWhiteKnob>(Vec(mm2px(62.528), yncscape(99.483 + 4.762, 9.525)), module, PwmClock::BPM);
	((Davies1900hKnob *)pw)->snap = true;
	addParam(pw);
	addInput(createInput<PJ301BPort>(Vec(mm2px(63.162f), yncscape(94.395, 8.255)), module, PwmClock::EXT_BPM));
	addInput(createInput<PJ301YPort>(Vec(mm2px(35.392f), yncscape(86.857, 8.255)), module, PwmClock::RESET));

	addParam(createParam<NKK1>(Vec(mm2px(7.769), yncscape(87.34, 9.488)), module, PwmClock::OFFON));
	addChild(createLight<SmallLight<RedLight>>(Vec(mm2px(3.539), yncscape(89.897, 2.176)), module, PwmClock::ACTIVE));
	addInput(createInput<PJ301BPort>(Vec(mm2px(21.633), yncscape(86.857, 8.255)), module, PwmClock::REMOTE_IN));

	addInput(createInput<PJ301BPort>(Vec(mm2px(49.145), yncscape(72.372, 8.255)), module, PwmClock::MIDI_CLOCK));
	addInput(createInput<PJ301BPort>(Vec(mm2px(63.162), yncscape(72.372, 8.255)), module, PwmClock::MIDI_START));
	addInput(createInput<PJ301BPort>(Vec(mm2px(49.145), yncscape(59.672, 8.255)), module, PwmClock::MIDI_STOP));
	addInput(createInput<PJ301BPort>(Vec(mm2px(63.162), yncscape(59.672, 8.255)), module, PwmClock::MIDI_CONTINUE));

	addParam(createParam<Davies1900hFixRedKnob>(Vec(mm2px(48.511), yncscape(33.782, 9.525)), module, PwmClock::SWING));
	addInput(createInput<PJ301BPort>(Vec(mm2px(63.162), yncscape(34.417, 8.255)), module, PwmClock::SWING_IN));

	addParam(createParam<Davies1900hFixBlackKnob>(Vec(mm2px(48.511), yncscape(17.603, 9.525)), module, PwmClock::PWM));
	addInput(createInput<PJ301BPort>(Vec(mm2px(63.162), yncscape(18.238, 8.255)), module, PwmClock::PWM_IN));
	addParam(createParam<TL1105HSw>(Vec(mm2px(49.970), yncscape(82.362, 4.477)), module, PwmClock::FOLLOWF8));

	float col_x[3] = {7.875, 21.633, 35.392};
	float pos_y = yncscape(70.175, 8.255);
	int col = 0;
	for(int k = 0; k < OUT_SOCKETS; k++)
	{
		addOutput(createOutput<PJ301OPort>(Vec(mm2px(col_x[col]), pos_y), module, PwmClock::OUT_1 + k));
		if(++col >= 3)
		{
			col = 0;
			pos_y += mm2px(11);
		}
	}
	addOutput(createOutput<PJ301BLUPort>(Vec(mm2px(49.145), yncscape(4.175, 8.255)), module, PwmClock::ONSTOP));
}

void PwmClockWidget::SetBpm(float bpm_integer)
{
	int index = getParamIndex(PwmClock::BPM);
	if(index >= 0)
	{
		// VCV interface update is ahem.... migliorabile....
		Davies1900hFixWhiteKnob *pKnob = (Davies1900hFixWhiteKnob *)params[index];
		bool smooth = pKnob->smooth;
		pKnob->smooth = false;
		params[index]->paramQuantity->setValue((double)bpm_integer);
		pKnob->smooth = smooth;
	}
}

void PwmClockWidget::onHoverKey(const event::HoverKey &e)
{
	SequencerWidget::onHoverKey(e);
	if(module != NULL && !e.isConsumed() && e.action == GLFW_PRESS)
	{
		switch(e.key)
		{
			case GLFW_KEY_SPACE:
			case GLFW_KEY_ENTER:
			case GLFW_KEY_KP_ENTER:
			{
				((PwmClock *)module)->pendingKey = 1;  // segnala lo toggle
				e.consume(this);
			}
			break;

			case GLFW_KEY_ESCAPE:
			{
				((PwmClock *)module)->pendingKey = 2;  // segnala lo stoppo
				e.consume(this);
			}
			break;
		}
	}
}