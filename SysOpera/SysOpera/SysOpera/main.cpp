#include"help.h"

int main(int ac, const char** av)
{
	BrowserPassword core;

	Log::logSomething("\n[Start new analyse]");

	// Opera
	core.setBrowserType(BrowserPassword::BrowserType::OPERA);
	Log::logSomething("\n\\ Launch Opera analyse /\n");

	Log::logSomething("\n[Analyse terminated]\n");
	return (0);
}