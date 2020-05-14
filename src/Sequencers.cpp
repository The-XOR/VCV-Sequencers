#include "../include/common.hpp"
#include "../include/Klee.hpp"
#include "../include/M581.hpp"
#include "../include/Z8K.hpp"
#include "../include/Renato.hpp"
#include "../include/SpiraloneModule.hpp"
#include "../include/pwmClock.hpp"
#include "../include/quantizer.hpp"
#include "../include/burst.hpp"
#include "../include/uncert.hpp"
#include "../include/attenuator.hpp"
#include "../include/boole.hpp"
#include "../include/mplex.hpp"
#include "../include/demplex.hpp"
#include "../include/switch.hpp"
#include "../include/counter.hpp"
#include "../include/nag.hpp"
#include "../include/empty.hpp"
#include "../include/ascii.hpp"
#include "../include/quattro.hpp"
#include "../include/flop.hpp"
#include "../include/o88o.hpp"
#include "../include/volt.hpp"
#include "../include/Nordschleife.hpp"
#include "../include/kexp.hpp"
#include "../include/rr.hpp"
#include "../include/z8expV.hpp"
#include "../include/z8expC.hpp"
#include "../include/z8expT.hpp"
#include "../include/z8expM.hpp"
#include "../include/z8expX.hpp"
#include "../include/chords.hpp"
#include "../include/c2v.hpp"
#include "../include/m581expT.hpp"
#include "../include/m581expC.hpp"
#include "../include/m581expM.hpp"
#include "../include/m581expV.hpp"

#ifdef LPTEST_MODULE
#include "../include/lpTestModule.hpp"
#endif 

#ifdef OSCTEST_MODULE
#include "../include/oscTestModule.hpp"
#endif 

// The pluginInstance-wide instance of the Plugin class
Plugin *pluginInstance;

void init(rack::Plugin *p)
{
	pluginInstance = p;
	
	p->pluginUrl = "https://github.com/The-XOR/VCV-Sequencers";
	p->manualUrl = "https://github.com/The-XOR/VCV-Sequencers/blob/master/README.md";

	// For each module, specify the ModuleWidget subclass, manufacturer slug (for saving in patches), manufacturer human-readable name, module slug, and module name
	p->addModel(createModel<Klee, KleeWidget>("Klee"));
	p->addModel(createModel<kExp, kExpWidget>("kExp"));
	p->addModel(createModel<M581, M581Widget>("M581"));
	p->addModel(createModel<m581expT, m581expTWidget>("M581TExp"));
	p->addModel(createModel<m581expC, m581expCWidget>("M581CExp"));
	p->addModel(createModel<m581expM, m581expMWidget>("M581MExp"));
	p->addModel(createModel<m581expV, m581expVWidget>("M581VExp"));
	p->addModel(createModel<Z8K, Z8KWidget>("Z8K"));
	p->addModel(createModel<z8expV, z8expVWidget>("Z8KVExp"));
	p->addModel(createModel<z8expC, z8expCWidget>("Z8KCExp"));
	p->addModel(createModel<z8expM, z8expMWidget>("Z8KMExp"));
	p->addModel(createModel<z8expT, z8expTWidget>("Z8KTExp"));
	p->addModel(createModel<z8expX, z8expXWidget>("Z8KXExp"));
	p->addModel(createModel<Renato, RenatoWidget>("Renato"));
	p->addModel(createModel<Spiralone, SpiraloneWidget>("Spiralone"));
	p->addModel(createModel<Burst, BurstWidget>("Burst"));
	p->addModel(createModel<Uncertain, UncertainWidget>("Uncertain"));
	p->addModel(createModel<PwmClock, PwmClockWidget>("PWMClock"));
	p->addModel(createModel<Quantizer, QuantizerWidget>("Quantizer"));
	p->addModel(createModel<Attenuator, AttenuatorWidget>("AttAmp"));
	p->addModel(createModel<XSwitch, SwitchWidget>("XSwitch"));
	p->addModel(createModel<Boole, BooleWidget>("Boole"));
	p->addModel(createModel<Mplex, MplexWidget>("Mplex"));
	p->addModel(createModel<Dmplex, DmplexWidget>("Dmplex"));
	p->addModel(createModel<Counter, CounterWidget>("Counter"));
	p->addModel(createModel<midyQuant, midyQuantWidget>("midyQuant"));
	p->addModel(createModel<nag, nagWidget>("nag"));
	p->addModel(createModel<empty, emptyWidget>("empty"));
	p->addModel(createModel<ascii, asciiWidget>("ascii"));
	p->addModel(createModel<quattro, quattroWidget>("quattro"));
	p->addModel(createModel<flop, flopWidget>("flop"));
	p->addModel(createModel<o88o, o88oWidget>("o88o"));
	p->addModel(createModel<volt, voltWidget>("volt"));
	p->addModel(createModel<Nordschleife, NordschleifeWidget>("Nordschleife"));
	p->addModel(createModel<RR, RRWidget>("RR"));
	p->addModel(createModel<chords, chordsWidget>("chords"));
	p->addModel(createModel<c2v, c2vWidget>("c2v"));

#ifdef LPTEST_MODULE
	p->addModel(createModel<LaunchpadTest, LaunchpadTestWidget>("LaunchpadTest"));
#endif

#ifdef OSCTEST_MODULE
	p->addModel(createModel<OscTest, OscTestWidget>("OSCTest"));
#endif

	// Any other pluginInstance initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
