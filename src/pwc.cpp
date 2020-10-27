#include "../include/pwc.hpp"

void pwc::process(const ProcessArgs &args)
{
	float vOut = 0;
	if(trigger.process(inputs[IN].getVoltage()))
	{
		float v = (0.001f+params[PW].getValue()) * 0.01f;		// valore in secondi
		pulsed.trigger(v);		
		vOut = LVL_MAX;
	} else
	{
		float deltaTime = 1.0 / args.sampleRate;

		int pulseStatus = pulsed.process(deltaTime);
		if (pulseStatus == -1) 
		{
			vOut = 0;
		} else
		{
			vOut = pulsed.inProgress ? LVL_MAX : 0;
		}
		
	}
	
	outputs[OUT].setVoltage(vOut);
}

pwcWidget::pwcWidget(pwc *module)
{
	CREATE_PANEL(module, this, 3, "res/modules/pwc.svg");
	addInput(createInput<portSmall>(Vec(mm2px(4.678), yncscape(97.491, 5.885)), module, pwc::IN));
	addParam(createParam<Davies1900hFixWhiteKnobSmall>(Vec(mm2px(3.620), yncscape(55.82, 8)), module, pwc::PW));
	addOutput(createOutput<portGSmall>(Vec(mm2px(4.678), yncscape(9.385, 5.885)), module, pwc::OUT));
}

