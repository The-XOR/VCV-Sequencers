#include "../include/dice.hpp"

void dice::process(const ProcessArgs &args)
{
	if(trigger.process(inputs[IN].getVoltage()))
	{
		float mi = getModulableParam(this, MIN, MIN_IN, LVL_MIN, LVL_MAX);
		float ma = getModulableParam(this, MAX, MAX_IN, LVL_MIN, LVL_MAX);
		bool uniform = isSwitchOn(this, MODE);
		float n;
		float range = fabs(ma - mi);
		if (uniform)
		{
			n = range * random::uniform() + mi;
		} else
		{
			n = 0.5 * range * random::normal();
		}

		outputs[OUT].setVoltage(clamp(n, LVL_MIN,LVL_MAX));
	}
} 


diceWidget::diceWidget(dice *module)
{
	CREATE_PANEL(module, this, 3, "res/modules/dice.svg");
	addInput(createInput<portSmall>(Vec(mm2px(4.678), yncscape(97.491, 5.885)), module, dice::IN));
	addInput(createInput<portSmall>(Vec(mm2px(4.678), yncscape(81.812, 5.885)), module, dice::MIN_IN));
	addInput(createInput<portSmall>(Vec(mm2px(4.678), yncscape(60.116, 5.885)), module, dice::MAX_IN));
	addParam(createParam<Davies1900hFixWhiteKnobSmall>(Vec(mm2px(3.620), yncscape(72.225, 8)), module, dice::MIN));
	addParam(createParam<Davies1900hFixWhiteKnobSmall>(Vec(mm2px(3.620), yncscape(50.529, 8)), module, dice::MAX));
	addParam(createParam<NKK1>(Vec(mm2px(3.386), yncscape(28.193, 9.488)), module, dice::MODE));
	addOutput(createOutput<portGSmall>(Vec(mm2px(4.678), yncscape(9.385, 5.885)), module, dice::OUT));
}

