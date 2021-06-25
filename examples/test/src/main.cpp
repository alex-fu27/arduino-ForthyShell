#include <Arduino.h>
#include <ForthyShell.h>

using namespace ForthyShell;

constexpr const Word customWords[] {
	{"myword", [](InterpretationContext& c){
		c.print("myword called");
	}}
};
constexpr const Dictionary d(customWords, sizeof(customWords) / sizeof(Word),
	&Dictionary::Default);

Interpreter i(d);

void setup()
{
	Serial.begin(115200);
	Serial.setTimeout(-1);
	Serial.write("ForthyShell test. Number of words: ");
	Serial.println(i.getDict().getNumWords());
}

void outputHelper(const char* c)
{
	Serial.write(c);
}

void loop()
{
	Serial.write("\nok ");
	String s = Serial.readStringUntil('\n');
	Error e = i.execute(s.c_str(), outputHelper);
	if (e != Error::OK) {
		Serial.write("error: ");
		Serial.write(e.getMessage());
		const char* i = e.getInfo();
		if (i) {
			Serial.write(": ");
			Serial.write(i);
		}
	}
}

