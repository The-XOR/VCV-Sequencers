#include "../include/common.hpp"
#include "../include/quantizer.hpp"

void midyQuant::process(const ProcessArgs &args)
{
	if(resetTrigger.process(inputs[RESET].getVoltage()))
	{
		midiOutput.reset();
	} else 
	{
		for (int c = 0; c < inputs[CV].getChannels(); c++)
		{

			bool gate = inputs[GATE].getPolyVoltage(c) >= 1.f; // 1=rise, -1=fall
			int vel = clamp((int)std::round(inputs[VEL].getNormalPolyVoltage(10.f * 100 / 127, c) / 10.f * 127), 0, 127);
			int note = clamp((int) std::round(NearestSemitone(inputs[CV].getVoltage(c)) * 12.f + 60.f), 0, 127);
			midiOutput.playNote(gate, note, vel);
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

