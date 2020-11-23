#include "../include/common.hpp"
#include "../include/demplex.hpp"

void Dmplex::on_loaded()
{
	load();
}

void Dmplex::load()
{
	set_output(0);
}

void Dmplex::set_output(int n)
{
	cur_sel = n;
	if(pWidget != NULL)
	{
		for(int k = 0; k < NUM_DEMULTIPLEX_OUTPUTS; k++)
		{
			pWidget->SetValue( Dmplex::DIRECT_1+k, k == cur_sel ? 1.0 : 0.0);
		}
	}
}

void Dmplex::process_keys()
{
	if(outInc.process(params[OUTPUT_INC].value))
	{
		int n = roundf(num_outputs_f);
		if(n < NUM_DEMULTIPLEX_OUTPUTS)
		{
			n += 1;
			num_outputs_f = n;
		}
	} else if(outDec.process(params[OUTPUT_DEC].value))
	{
		int n = roundf(num_outputs_f);
		if(n > 1)
		{
			n -= 1;
			num_outputs_f = n;
		}
	}
}

void Dmplex::process(const ProcessArgs &args)
{
	process_keys();
	int num_outputs = roundf(num_outputs_f);

	if(reset.process(inputs[RESET].getVoltage()))
	{
		set_output(0);
	} else if(inputs[CV].isConnected())
	{
		cur_sel = clamp((int)(std::numeric_limits<float>::epsilon() + inputs[CV].getNormalVoltage(0.0)), 0, num_outputs - 1);
		set_output(cur_sel);
	} else
	{
		if(random.process(inputs[RANDOM].getVoltage()))
		{
			set_output(getRand(num_outputs_f));
		} else if(upTrigger.process(params[BTDN].value + inputs[INDN].getVoltage()))
		{
			if(++cur_sel >= num_outputs)
				cur_sel = 0;
			set_output(cur_sel);
		} else if(dnTrigger.process(params[BTUP].value + inputs[INUP].getVoltage()))
		{
			if(--cur_sel < 0)
				cur_sel = num_outputs - 1;
			set_output(cur_sel);
		} else
		{
			for(int k = 0; k < NUM_DEMULTIPLEX_OUTPUTS; k++)
			{
				if(direct[k].process(params[DIRECT_1+k].value))
				{
					set_output(k);
					break;
				}
			}
		}
	}
	if (isSwitchOn(this, DIRECT_1 + cur_sel))
		outputs[OUT_1 + cur_sel].setVoltage(inputs[IN_1].getVoltage());
	else
		outputs[OUT_1 + cur_sel].setVoltage(LVL_OFF);
}

DmplexWidget::DmplexWidget(Dmplex *module) : SequencerWidget()
{
	CREATE_PANEL(module, this, 10, "res/modules/dmplex.svg");

	addParam(createParam<BefacoPushBig>(Vec(mm2px(15.267), yncscape(85.436, 8.999)), module, Dmplex::BTUP));
	addParam(createParam<BefacoPushBig>(Vec(mm2px(15.267), yncscape(33.452, 8.999)), module, Dmplex::BTDN));
	addInput(createInput<PJ301BPort>(Vec(mm2px(15.640), yncscape(71.230, 8.255)), module, Dmplex::INUP));
	addInput(createInput<PJ301BPort>(Vec(mm2px(15.640), yncscape(49.014, 8.255)), module, Dmplex::INDN));

	addInput(createInput<PJ301GRPort>(Vec(mm2px(3.558), yncscape(60.122, 8.255)), module, Dmplex::IN_1));

	addInput(createInput<PJ301YPort>(Vec(mm2px(3.558), yncscape(108.243, 8.255)), module, Dmplex::RESET));
	addInput(createInput<PJ301BPort>(Vec(mm2px(3.558), yncscape(96.422, 8.255)), module, Dmplex::RANDOM));
	addInput(createInput<PJ301BPort>(Vec(mm2px(3.558), yncscape(19.939, 8.255)), module, Dmplex::CV));

	addParam(createParam<UPSWITCH>(Vec(mm2px(3.127), yncscape(10.727, 4.115)), module, Dmplex::OUTPUT_INC));
	addParam(createParam<DNSWITCH>(Vec(mm2px(3.127), yncscape(5.926, 4.115)), module, Dmplex::OUTPUT_DEC));

	SigDisplayWidget *display = new SigDisplayWidget(1, 0);
	display->box.size = Vec(15, 22);
	display->box.pos = Vec(mm2px(8.495), yncscape(6.439, px2mm(display->box.size.y)));
	if(module != NULL)
	{
		display->value = &module->num_outputs_f;
		module->setWidget(this);	
	}
	addChild(display);

	float y = 105.068;
	float x = 40.045;
	float y_offs = 107.196;
	float delta_y = 92.529 - 105.068;
	for(int k = 0; k < NUM_DEMULTIPLEX_OUTPUTS; k++)
	{
		addOutput(createOutput<PJ301GPort>(Vec(mm2px(x), yncscape(y, 8.255)), module, Dmplex::OUT_1 + k));
		addParam(createParam<CKD6Bsmall>(Vec(mm2px(35.198), yncscape(y_offs, 4)), module, Dmplex::DIRECT_1+k));
		y += delta_y;
		y_offs += delta_y;
		if(k == 3)
		{
			y -= 2.117;
			y_offs -= 2.117;
		}
	}
}
