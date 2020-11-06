#include "../include/common.hpp"
#include "../include/burst.hpp"

void Burst::on_loaded()
{
	load();
}

void Burst::load()
{
	trigger_pending = false;
	all_off();
}

void Burst::all_off()
{
	for(int k = 0; k < NUM_BURST_PORTS; k++)
		port(k, false);
	active = false;
}

void Burst::process(const ProcessArgs &args)
{
	if(resetTrigger.process(inputs[RESET].getVoltage()))
	{
		onReset();
	} else
	{
		if(!active && !trigger_pending)
		{
			float tv = inputs[TRIGGER_THRESH_IN].isConnected() && inputs[TRIGGER_THRESH_IN].getVoltage() > params[TRIG_THRESH].getValue() ? 1.0 : 0.0;
			trigger_pending = trigger.process(params[TRIGGER].value + tv);
		}

		int clk = clock.process(inputs[CLOCK_IN].getVoltage()); // 1=rise, -1=fall

		if(clk == 1)
		{
			if(!active && trigger_pending)
			{
				prepare_step();
			}

			if(active)
				next_step();
		} else if(active && clk == -1)
			end_step();
	}
}

void Burst::prepare_step()
{
	activating_params.first_cycle = true;
	activating_params.cycle_counter = activating_params.out_span = 0;
	activating_params.max_span = (int)roundf(getModulableParam(this, OUT_SPAN, OUT_SPAN_IN, 1, NUM_BURST_PORTS));
	activating_params.mode = (enum Burst::MODE)roundf(params[MODE].value);
	activating_params.invert_mode = isSwitchOn(this, MODE_INVERT);
	activating_params.retrogade = false;	
	activating_params.max_cycle = (int)roundf(getModulableParam(this, EVENT_COUNT, EVENT_COUNT_IN, 0, 23)) + 1;
	trigger_pending = false;
	active = true;
}

void Burst::end_step()
{
	if(activating_params.cycle_counter >= activating_params.max_cycle)
	{
		all_off();	// ciclo terminato
	} else
	{
		switch(activating_params.mode)
		{
			case Burst::RAND:
			{
				port(activating_params.out_span, false);
			}
			break;

			case Burst::PEND:
			case Burst::FWD:
			{				
				if(!activating_params.invert_mode)
				{
					port(activating_params.out_span, false);
				}					
			}
			break;
		}
	}
}

void Burst::next_step()
{
	switch(activating_params.mode)
	{
		case Burst::RAND:
		{
			activating_params.out_span = int(rescale(random::uniform(), 0.0, 1.0, 0.0, activating_params.max_span));
			port(activating_params.out_span, true);
			activating_params.cycle_counter++;
		}
		break;

		case Burst::FWD:
		{
			if(activating_params.first_cycle)
				activating_params.first_cycle = false;
			else
			{
				if(++activating_params.out_span >= activating_params.max_span)
				{
					activating_params.out_span = 0;
					activating_params.cycle_counter++;
				}
			}
			if(activating_params.cycle_counter < activating_params.max_cycle)
			{
				if(activating_params.invert_mode)
					invert_port(activating_params.out_span);
				else
				{
					port(activating_params.out_span, true);
				}
			}
		}
		break;
			
		case Burst::PEND:
		{
			if(activating_params.first_cycle)
				activating_params.first_cycle = false;
			else
			{
				if(activating_params.retrogade)
				{
					if(--activating_params.out_span < 0)
					{
						if(activating_params.max_span > 0)
							activating_params.out_span = activating_params.invert_mode ? 0 : 1;
						else
							activating_params.out_span = 0;
						activating_params.retrogade = false;
						activating_params.cycle_counter++;
					}
				} else
				{
					if(++activating_params.out_span >= activating_params.max_span)
					{
						if(activating_params.max_span > 1)
							activating_params.out_span = activating_params.invert_mode ? activating_params.out_span-1 : activating_params.out_span - 2;
						else
							activating_params.out_span = 0;
						activating_params.retrogade = true;
						activating_params.cycle_counter++;
					}
				}
			}
			if(activating_params.cycle_counter < activating_params.max_cycle)
			{
				if(activating_params.invert_mode)
					invert_port(activating_params.out_span);
				else
					port(activating_params.out_span, true);
			}
		}
		break;
	}
}

BurstWidget::BurstWidget(Burst *module) : SequencerWidget()
{
	CREATE_PANEL(module, this, 16, "res/modules/Burst.svg");

	float lft_x = mm2px(3.428);

	addInput(createInput<PJ301RPort>(Vec(lft_x, yncscape(108.765, 8.255)), module, Burst::CLOCK_IN));
	addParam(createParam<CKSSThreeFix>(Vec(mm2px(31.624), yncscape(105.749, 10.0)), module, Burst::MODE));
	addParam(createParam<TL1105Sw>(Vec(mm2px(71.486), yncscape(107.417, 6.607)), module, Burst::MODE_INVERT));
	
	addInput(createInput<PJ301BPort>(Vec(lft_x, yncscape(74.386, 8.255)), module, Burst::OUT_SPAN_IN));
	ParamWidget *pwdg = createParam<Davies1900hFixWhiteKnob>(Vec(mm2px(22.644), yncscape(73.751, 9.525)), module, Burst::OUT_SPAN);
	((Davies1900hKnob *)pwdg)->snap = true;
	addParam(pwdg);
	pwdg = createParam<Davies1900hFixWhiteKnob>(Vec(mm2px(49.111), yncscape(73.751, 9.525)), module, Burst::EVENT_COUNT);
	((Davies1900hKnob *)pwdg)->snap = true;
	addParam(pwdg);
	addInput(createInput<PJ301BPort>(Vec(mm2px(69.597), yncscape(74.386, 8.255)), module, Burst::EVENT_COUNT_IN));

	addInput(createInput<PJ301BPort>(Vec(lft_x, yncscape(43.036, 8.255)), module, Burst::TRIGGER_THRESH_IN));
	addParam(createParam<Davies1900hFixRedKnob>(Vec(mm2px(16.027), yncscape(42.401, 9.525)), module, Burst::TRIG_THRESH));
	addInput(createInput<PJ301YPort>(Vec(mm2px(56.363), yncscape(43.036, 8.255)), module, Burst::RESET));
	addParam(createParam<BefacoPushBig>(Vec(mm2px(69.224), yncscape(42.664, 8.999)), module, Burst::TRIGGER));

	float ysup_port = yncscape(17.532, 8.255);
	float yinf_port = yncscape(4.832, 8.255);
	float ysup_led = yncscape(19.070, 5.179);
	float yinf_led = yncscape(6.370, 5.179);
	float x_ports[NUM_BURST_PORTS] = {3.428, 16.662, 29.895, 43.129, 56.363, 69.597};
	float x_leds[NUM_BURST_PORTS] = {4.966, 18.200, 31.434, 44.667, 57.901, 71.135};

	for(int k = 0; k < NUM_BURST_PORTS; k++)
	{
		if(k < NUM_BURST_PORTS / 2)
		{
			addOutput(createOutput<PJ301WPort>(Vec(mm2px(x_ports[k]), ysup_port), module, Burst::OUT_1+k));
			addChild(createLight<LargeLight<RedLight>>(Vec(mm2px(x_leds[k]), yinf_led), module, Burst::LEDOUT_1 + k));
		} else
		{
			addOutput(createOutput<PJ301WPort>(Vec(mm2px(x_ports[k]), yinf_port), module, Burst::OUT_1 + k));
			addChild(createLight<LargeLight<RedLight>>(Vec(mm2px(x_leds[k]), ysup_led), module, Burst::LEDOUT_1 + k));
		}
	}
}
