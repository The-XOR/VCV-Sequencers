#include "../include/common.hpp"
#include "../include/quantizer.hpp"

void midyQuant::process(const ProcessArgs &args)
{
	if(resetTrigger.process(inputs[RESET].getVoltage()))
	{
		midiOutput.reset();
	} else 
	{
		int clk = gate.process(inputs[GATE].getVoltage()); // 1=rise, -1=fall
		if(clk != 0)
		{
			int vel = (int)rescale(inputs[VEL].getNormalVoltage(0.5), 0.0, 1.0, 0, 127);
			if(note_playing >= 0 && clk == -1)
			{
				midiOutput.sendNote(false, note_playing, vel);
				note_playing = -1;
			} else if(clk == 1)
			{
				float v = inputs[CV].getNormalVoltage(0.0);
				float semitone = NearestSemitone(v);
				note_playing = clamp(std::round(semitone * 12.0 + 60.0), 0, 127);
				midiOutput.sendNote(true, note_playing, vel);
			}
		}
	}
}

void midyQuant::on_loaded()
{
	load();
	calcScale();
}

Menu *midyQuantWidget::addContextMenu(Menu *menu)
{
	menu->addChild(new SeqMenuItem<midyQuantWidget>("MIDI Panic", this, MIDIPANIC));
	return menu;
}

void midyQuantWidget::onMenu(int action)
{
	switch(action)
	{
		case MIDIPANIC: ((midyQuant *)module)->midiOutput.reset();
			break;
	}
}

midyQuantWidget::midyQuantWidget(midyQuant *module) : ModuleWidget()
{
	display = NULL;
	midiDisplay = NULL;
	if(module != NULL)
		module->setWidget(this);

	CREATE_PANEL(module, this, 8, "res/modules/midyQuant.svg");

	display = createWidget<qtzrDisplay>(mm2px(Vec(3.41891, 14.0)));
	display->box.size = mm2px(Vec(33.840, 19));
	if(module != NULL)
		display->CreateInterface(module);
	addChild(display);

	midiDisplay = createWidget<qtzrMidiDisplay>(mm2px(Vec(3.41891, 38.00)));
	midiDisplay->box.size = mm2px(Vec(33.840, 28));
	if(module != NULL)
		midiDisplay->CreateInterface(module);
	addChild(midiDisplay);

	addInput(createInput<PJ301GPort>(Vec(mm2px(2.669), yncscape(9.432, 8.255)), module, midyQuant::CV));
	addInput(createInput<PJ301GRPort>(Vec(mm2px(15.928), yncscape(9.432, 8.255)), module, midyQuant::GATE));
	addInput(createInput<PJ301BPort>(Vec(mm2px(29.686), yncscape(9.432, 8.255)), module, midyQuant::VEL));
	addInput(createInput<PJ301YPort>(Vec(mm2px(15.928), yncscape(29.540, 8.255)), module, midyQuant::RESET));
}

