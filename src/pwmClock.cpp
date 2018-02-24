#include "pwmClock.hpp"


void PwmClock::on_loaded()
{
	bpm = 0;
	for(int k = 0; k < OUT_SOCKETS; k++)
	{
		sa_timer[k].Reset();
	}
	load();
}

void PwmClock::load()
{
	updateBpm();
}

void PwmClock::process_keys()
{
	if(btnup.process(params[BPM_INC].value))
	{
		if(bpm_integer < 220.0)
			bpm_integer += 1;
		pWidget->SetBpm(bpm_integer);
	}

	if(btndwn.process(params[BPM_DEC].value))
	{
		if(bpm_integer > 0)
			bpm_integer -= 1;
		pWidget->SetBpm(bpm_integer);
	}
}

void PwmClock::step()
{
	process_keys();
	bpm_integer = roundf(params[BPM].value);
	updateBpm();
	
	for(int k = 0; k < OUT_SOCKETS; k++)
	{
		float gate_len = duration[k] * params[PWM].value;
		sa_timer[k].Step();
		float elps = sa_timer[k].Elapsed();
		if(elps >= duration[k])
		{
			elps = sa_timer[k].Reset();
		}
		if(elps <= gate_len)
			outputs[OUT_1 + k].value = LVL_ON;
		else
			outputs[OUT_1 + k].value = LVL_OFF;
	}
}

PwmClockWidget::PwmClockWidget(PwmClock *module) : SequencerWidget(module)
{
	module->setWidget(this);
	box.size = Vec(150, 380);
	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;

		panel->setBackground(SVG::load(assetPlugin(plugin, "res/PwmClock.svg")));
		addChild(panel);
	}

	addChild(Widget::create<ScrewSilver>(Vec(15, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(15, 365)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 365)));

	int pos_y = 35;
	SigDisplayWidget *display = new SigDisplayWidget(4, 1);
	display->box.pos = Vec(35, pos_y);
	display->box.size = Vec(78, 24);

	display->value = &module->bpm;
	addChild(display);

	addParam(ParamWidget::create<LEDButton>(Vec(10, pos_y + 2), module, PwmClock::BPM_DEC, 0.0, 1.0, 0.0));
	addParam(ParamWidget::create<LEDButton>(Vec(120, pos_y + 2), module, PwmClock::BPM_INC, 0.0, 1.0, 0.0));
	addParam(ParamWidget::create<Rogan1PSWhiteSnapped>(Vec(12, pos_y + 36), module, PwmClock::BPM, 20.0, 220.0, 120.0));
	addParam(ParamWidget::create<Rogan1PSWhiteSnappedSmall>(Vec(62, pos_y + 42), module, PwmClock::BPMDEC, 0.0, 9.0, 0.0));
	addParam(ParamWidget::create<Rogan1PSRed>(Vec(97, pos_y + 36), module, PwmClock::PWM, 0.1, 0.9, 0.5));

	int row = 0;
	int col = 0;
	const char *divisor_len[] = {"1/1", "1/2", "1/4", "1/8", "1/16", "1/32", "1/64"};

	for(int k = 0; k < OUT_SOCKETS; k++)
	{
		int x = col * 36 + 38;
		int y = row * 29 + pos_y + 116;
		addOutput(Port::create<PJ301MPort>(Vec(x, y), Port::OUTPUT, module, PwmClock::OUT_1 + k));

		if(col == 0)
		{
			Label *label = new Label();
			label->box.pos = Vec(0, y + 2);
			label->text = divisor_len[row];
			addChild(label);
		}

		if(++col >= 3)
		{
			col = 0;
			row++;
		}
	}
}

void PwmClockWidget::SetBpm(float bpm_integer)
{
	int index = getParamIndex(PwmClock::BPM);
	params[index]->setValue(bpm_integer);
}