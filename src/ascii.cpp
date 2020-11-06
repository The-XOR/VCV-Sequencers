#include "../include/ascii.hpp"

void ascii::manifesto()
{
	/*
	arte. l'ultimo vero movimento artistico totale: musica, letteratura, pittura, architettura....
	tutto il resto e' noooooooiaaaaaaa
	*/

	static std::vector<std::string> manifesto_futurista =
	{
		"Manifesto dei futurismo.",
		"",
		"1. Noi vogliamo cantare l'amor del pericolo, l'abitudine all'energia e alla temerita'.",
		"2. Il coraggio, l'audacia, la ribellione, saranno elementi essenziali della nostra poesia.",
		"3. La letteratura esalto' fino ad oggi l'immobilita' pensosa, l'estasi e il sonno.Noi vogliamo esaltare il movimento aggressivo, l'insonnia febbrile, il passo di corsa, il salto mortale, lo schiaffo ed il pugno.",
		"4. Noi affermiamo che la magnificenza del mondo si e' arricchita di una bellezza nuova : la bellezza della velocita'.Un automobile da corsa col suo cofano adorno di grossi tubi simili a serpenti dall'alito esplosivo....un automobile ruggente, che sembra correre sulla mitraglia, e' piu' bello della Vittoria di Samotracia.",
		"5. Noi vogliamo inneggiare all'uomo che tiene il volante, la cui asta ideale attraversa la Terra, lanciata a corsa, essa pure, sul circuito della sua orbita.",
		"6. Bisogna che il poeta si prodighi, con ardore, sfarzo e munificenza, per aumentare l'entusiastico fervore degli elementi primordiali.",
		"7. Non v'e' piu' bellezza, se non nella lotta.Nessuna opera che non abbia un carattere aggressivo puo' essere un capolavoro.La poesia deve essere concepita come un violento assalto contro le forze ignote, per ridurle a prostrarsi davanti all'uomo.",
		"8. Noi siamo sul promontorio estremo dei secoli!..Perche' dovremmo guardarci alle spalle, se vogliamo sfondare le misteriose porte dell'Impossibile ? Il Tempo e lo Spazio morirono ieri.Noi viviamo gia' nell'assoluto, poiche' abbiamo gia' creata l'eterna velocita' onnipresente.",
		"9. Noi vogliamo glorificare la guerra - sola igiene del mondo - il militarismo, il patriottismo, il gesto distruttore dei libertarii, le belle idee per cui si muore e il disprezzo della donna.",
		"10. Noi vogliamo distruggere i musei, le biblioteche, le accademie d'ogni specie, e combattere contro il moralismo, il femminismo e contro ogni vilta' opportunistica o utilitaria.",
		"11. Noi canteremo le grandi folle agitate dal lavoro, dal piacere o dalla sommossa : canteremo le maree multicolori e polifoniche delle rivoluzioni nelle capitali moderne; canteremo il vibrante fervore notturno degli arsenali e dei cantieri incendiati da violente lune elettriche; le stazioni ingorde, divoratrici di serpi che fumano; le officine appese alle nuvole pei contorti fili dei loro fumi; i ponti simili a ginnasti giganti che scavalcano i fiumi, balenanti al sole con un luccichio di coltelli; i piroscafi avventurosi che fiutano l'orizzonte, le locomotive dall'ampio petto, che scalpitano sulle rotaie, come enormi cavalli d'acciaio imbrigliati di tubi, e il volo scivolante degli aereoplani, la cui elica garrisce al vento come una bandiera e sembra applaudire come una folla entusiasta.",
	};

	std::string s;
	for(std::vector<std::string>::iterator it = manifesto_futurista.begin(); it != manifesto_futurista.end(); ++it)
	{
		s += *it;
		s += "\n";
	}
	textField->text = s;
}

void ascii::process(const ProcessArgs &args)
{
	if(textField != NULL)
	{
		if(resetTrigger.process(inputs[RESET].getVoltage()) || masterReset.process(params[M_RESET].value))
		{
			textField->cursor = textField->selection = 0;
			outputs[OUT].setVoltage( LVL_OFF);
		} else if(manifestoTrigger.process(params[FTM].value))
		{
			textField->cursor = textField->selection = 0;
			outputs[OUT].setVoltage( LVL_OFF);
			manifesto();
		} else
		{
			int clk = clockTrig.process(inputs[CLK].getVoltage()); // 1=rise, -1=fall
			if(clk == 1)
			{
				std::string safecopy = textField->text;
				int len = safecopy.length();
				if(len > 0)
				{
					if(textField->cursor >= len)
						textField->cursor = 0;
					char c = safecopy.at(textField->cursor++);
					textField->selection = textField->cursor;
					outputs[OUT].setVoltage( getValue(c));
				} else
					outputs[OUT].setVoltage( LVL_OFF);

			}
		}
	}
}

float ascii::getValue(char c)
{
	return cvs.TransposeableValue(rescale(c, 32.0, 127.0, 0.0, 1.0));
}

asciiWidget::asciiWidget(ascii *module)
{
	CREATE_PANEL(module, this, 18, "res/modules/ascii.svg");

	textField = createWidget<LedDisplayTextField>(mm2px(Vec(3.39962, 14.8373)));
	textField->box.size = mm2px(Vec(74.480, 98.753));
	((LedDisplayTextField *)textField)->color = nvgRGB(0xff, 0xff, 0xff);

	textField->multiline = true;
	addChild(textField);
	if(module != NULL)
		module->setField(textField);

	addParam(createParam<HiddenButton>(Vec(mm2px(26.642), yncscape(114.398, 5.08)), module, ascii::FTM));

	addInput(createInput<PJ301RPort>(Vec(mm2px(10.932), yncscape(4.233, 8.255)), module, ascii::CLK));
	addInput(createInput<PJ301YPort>(Vec(mm2px(36.512), yncscape(4.233, 8.255)), module, ascii::RESET));
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(62.092), yncscape(4.233, 8.255)), module, ascii::OUT));
	addChild(createParam<BefacoPushBig>(Vec(mm2px(3.400), yncscape(116.611, 8.999)), module, ascii::M_RESET));
	if(module != NULL)
		module->cvs.Create(this, 80.179f, 41.284f, ascii::NUM_INPUTS - cvMiniStrip::CVMINISTRIP_INPUTS, ascii::NUM_PARAMS - cvMiniStrip::CVMINISTRIP_PARAMS);

}
