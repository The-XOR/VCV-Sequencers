#include "../include/common.hpp"
#include "../include/flop.hpp"

void flop::process(const ProcessArgs &args)
{
	bool hiz = isSwitchOn(this, HIZ);
	bool compare = isSwitchOn(this, COMPAREMODE);

	for(int k = 0; k < NUM_FLOPS; k++)
	{
		process(k, hiz, compare);
	}
}

void flop::setOutput(int index, bool on)
{
	lights[LED_OUT + index].value = on ? LED_ON : LED_OFF;
	outputs[OUT_1 + index].value = on ? LVL_ON : LVL_OFF;
	lights[LED_OUTNEG + index].value = on ? LED_OFF : LED_ON;
	outputs[OUTNEG_1 + index].value = on ? LVL_OFF : LVL_ON;
}

float flop::getVoltage(int index, bool hiz)
{
	if(hiz && !inputs[index].isConnected())
	{
		if((hizCounter++ == 4000))
		{
			hizvalue = random::normal() * 2.3;
		} else if(hizCounter == 6000)
		{
			hizCounter = 0;
			hizvalue = 0;
		}
		return hizvalue;
	}

	return inputs[index].getNormalVoltage(0.0);
}

bool flop::logicLevel(float v1, float v2, bool compare)
{	
	return compare ? fabs(v1 - v2) < std::numeric_limits<float>::epsilon() : v1 > v2;
}

void flop::process(int num_op, bool hiz, bool compare)
{
	bool a_in = logicLevel(getVoltage(IN_A + num_op, hiz), params[THRESH_A + num_op].value, compare);
	lights[LED_A + num_op].value = a_in ? LED_ON : LED_OFF;

	bool b_in = logicLevel(getVoltage(IN_B + num_op, hiz), params[THRESH_B + num_op].value, compare);
	lights[LED_B + num_op].value = b_in ? LED_ON : LED_OFF;

	int trig_b = b_Trig[num_op].process(b_in);
	if(a_Trig[num_op].process(a_in) != 0 || trig_b != 0)
	{
		bool stat = false;
		switch(num_op)
		{
			case 0: stat = flipflip_SR(a_in, b_in);	break;
			case 1: stat = flipflip_JK(a_in, b_in);	break;
			case 2: stat = flipflip_T(a_in, b_in, trig_b);	break;
			case 3: stat = flipflip_D(a_in, b_in, trig_b);  break;
		}
		setOutput(num_op, stat);
	}
}

bool flop::flipflip_SR(bool s, bool r)
{
	/*
	s=0 e r=1 il flip-flop si resetta, cioè porta a 0 il valore della variabile d'uscita q 
	s=1 e r=0 il flip-flop si setta cioè porta a 1 il valore della variabile d'uscita q 
	s=0 e r=0 il flip-flop conserva, cioè mantiene inalterato 
	s=1 e r=1 non viene utilizzata in quanto instabile 
	*/
	if(s)
		return r ? (random::uniform() > 0.5) /* s=1 e r=1: instabile! */ : (sr_state = true) /* s=1 e r=0: set*/;
	else
		return r ? (sr_state = false) /* s=0 e r=1: reset! */ : sr_state /* s=0 e r=0: latch*/;
}

bool flop::flipflip_JK(bool j, bool k)
{
	/*
	j=0 e k=0 : latch
	j=0 e k=1 : reset
	j=1 e k=0 : set
	j=1 e k=1 : toggle
	*/
	if(j)
		return jk_state = (k ? !jk_state : true);
	else
		return k ? (jk_state = false) : jk_state;
}

bool flop::flipflip_T(bool t, bool clk_in, int trig_b)
{
	/*
	t = 0 : latch
	t = 1 : i negato (fronte di salita)
	*/
	if(t)
	{
		if(trig_b == 1) // 1=rise, -1=fall
			t_state = !t_state;
	}
	return t_state;
}

bool flop::flipflip_D(bool d, bool clk_in, int trig_b)
{
	if(trig_b == 1) // 1=rise, -1=fall
		d_state = d;

	return d_state;
}

flopWidget::flopWidget(flop *module) : ModuleWidget()
{
	CREATE_PANEL(module, this, 14, "res/modules/flop.svg");
	
	for(int k = 0; k < NUM_FLOPS; k++)
	{
		// A + Q
		addInput(createInput<PJ301GRPort>(Vec(mm2px(7.536f), yncscape(106.746-k*27.103f, 8.255)), module, flop::IN_A + k));
		addParam(createParam<Davies1900hFixWhiteKnobSmall>(Vec(mm2px(22.927f), yncscape(106.874f - k * 27.103f, 8.0)), module, flop::THRESH_A + k));
		addChild(createLight<SmallLight<WhiteLight>>(Vec(mm2px(18.144f), yncscape(109.786f - k * 27.103f, 2.176)), module, flop::LED_A +k));
		addOutput(createOutput<PJ301WPort>(Vec(mm2px(56.753f), yncscape(106.746 - k * 27.103f, 8.255)), module, flop::OUT_1 + k));
		addChild(createLight<SmallLight<RedLight>>(Vec(mm2px(53.069f), yncscape(109.786f - k * 27.103f, 2.176)), module, flop::LED_OUT + k));

		// B + Qneg
		addInput(createInput<PJ301GRPort>(Vec(mm2px(7.536f), yncscape(94.826 - k * 27.103f, 8.255)), module, flop::IN_B + k));
		addParam(createParam<Davies1900hFixWhiteKnobSmall>(Vec(mm2px(22.927f), yncscape(94.954f - k * 27.103f, 8.0)), module, flop::THRESH_B + k));
		addChild(createLight<SmallLight<WhiteLight>>(Vec(mm2px(18.144f), yncscape(97.866f - k * 27.103f, 2.176)), module, flop::LED_B + k));
		addOutput(createOutput<PJ301WPort>(Vec(mm2px(56.753f), yncscape(94.826 - k * 27.103f, 8.255)), module, flop::OUTNEG_1 + k));
		addChild(createLight<SmallLight<RedLight>>(Vec(mm2px(53.069f), yncscape(97.866f - k * 27.103f, 2.176)), module, flop::LED_OUTNEG + k));
	}

	addParam(createParam<TL1105HSw>(Vec(mm2px(10.228), yncscape(120.086, 4.477)), module, flop::COMPAREMODE));
	addParam(createParam<TL1105HSw>(Vec(mm2px(21.197), yncscape(2.319, 4.477)), module, flop::HIZ));
}

