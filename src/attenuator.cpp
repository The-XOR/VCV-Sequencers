#include "../include/common.hpp"
#include "../include/attenuator.hpp"

void Attenuator::process(const ProcessArgs &args)
{
	bool am = isSwitchOn(this, ATTMODE);

	for(int k = 0; k < NUM_ATTENUATORS; k++)
	{
		if(outputs[OUT_1 + k].isConnected())
			outputs[OUT_1 + k].setVoltage( (inputs[IN_1 + k].getVoltage() * params[ATT_1 + k].value)+params[OFFS_1 + k].value);
	}

	for(int k = 0; k < NUM_VLIMITERS; k++)
	{
		if (outputs[OUT_1 + k + NUM_ATTENUATORS].isConnected())
		{
			float mi = std::min(params[LIM1_MIN + k].value, params[LIM1_MAX + k].value);
			float ma = std::max(params[LIM1_MIN + k].value, params[LIM1_MAX + k].value);
			if(am)
				outputs[OUT_1 + k + NUM_ATTENUATORS].setVoltage( rescale(inputs[IN_1 + k + NUM_ATTENUATORS].getVoltage(), LVL_MIN, LVL_MAX, mi, ma));
			else
				outputs[OUT_1 + k + NUM_ATTENUATORS].setVoltage( clamp(inputs[IN_1 + k + NUM_ATTENUATORS].getVoltage(), mi, ma));
		}
	}

}

AttenuatorWidget::AttenuatorWidget(Attenuator *module) : ModuleWidget()
{
	CREATE_PANEL(module, this, 8, "res/modules/attenuator.svg");

	float y = yncscape(104.213, 5.885);
	float ypot = yncscape(104.098, 6.0);
	float delta_y = mm2px(104.213-95.041);
	
	for(int k = 0; k < NUM_ATTENUATORS; k++)
	{
		addInput(createInput<portGRSmall>(Vec(mm2px(1.432), y), module, Attenuator::IN_1 + k));
		addParam(createParam<daviesVerySmallWhite>(Vec(mm2px(11.558), ypot), module, Attenuator::OFFS_1 + k));
		addParam(createParam<daviesVerySmallWhite>(Vec(mm2px(21.612), ypot), module, Attenuator::ATT_1 + k));
		addOutput(createOutput<portGSmall>(Vec(mm2px(30.953), y), module, Attenuator::OUT_1 + k));
		y += delta_y;
		ypot += delta_y;
	}

	y = yncscape(41.771, 5.885);
	ypot = yncscape(41.656, 6.0);

	for(int k = 0; k < NUM_VLIMITERS; k++)
	{
		addInput(createInput<portGRSmall>(Vec(mm2px(1.432), y), module, Attenuator::IN_1 + k + NUM_ATTENUATORS));
		addParam(createParam<daviesVerySmallRed>(Vec(mm2px(11.558), ypot), module, Attenuator::LIM1_MIN + k));
		addParam(createParam<daviesVerySmallRed>(Vec(mm2px(21.612), ypot), module, Attenuator::LIM1_MAX + k));
		addOutput(createOutput<portGSmall>(Vec(mm2px(30.953), y), module, Attenuator::OUT_1 + k + NUM_ATTENUATORS));
		y += delta_y;
		ypot += delta_y;
	}

	addParam(createParam<TL1105HBSw>(Vec(mm2px(17.017), yncscape(57.138, 4.477)), module, Attenuator::ATTMODE));
}

