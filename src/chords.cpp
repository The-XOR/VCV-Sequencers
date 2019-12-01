#include "../include/chords.hpp"

void chords::process(const ProcessArgs &args)
{
	float v = inputs[INPUT].getVoltage();
	for(int k = 0; k < NUM_CHORDS; k++)
	{
		if(isSwitchOn(this, QTZ_IN))
			v = NearestSemitone(v);

		outputs[OUT_1 + k].setVoltage(params[ST_1 + k].value * SEMITONE + v);
	}
}


chordsWidget::chordsWidget(chords *module)
{
	CREATE_PANEL(module, this, 8, "res/modules/chords.svg");
	addInput(createInput<PJ301GPort>(Vec(mm2px(7.351), yncscape(104.003, 8.255)), module, chords::INPUT));
	addParam(createParam<TL1105HSw>(Vec(mm2px(27.445), yncscape(105.892, 4.477)), module, chords::QTZ_IN));

	float dy = 59.398 - 81.982;
	for(int k = 0; k < NUM_CHORDS; k++)
	{
		Davies1900hFixRedKnob *p = createParam<Davies1900hFixRedKnob>(Vec(mm2px(5.764), yncscape(81.982 + k * dy, 11.430)), module, chords::ST_1 + k);
		p->snap = true;
		addParam(p);
		addOutput(createOutput<PJ301GPort>(Vec(mm2px(26.621), yncscape(83.569+k*dy, 8.255)), module, chords::OUT_1+k));
	}
}

