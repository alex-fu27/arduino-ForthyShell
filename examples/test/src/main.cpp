#include <Arduino.h>
#include <ForthyShell.h>

using namespace ForthyShell;

const Word customWords[] {
	{"myword", [](InterpretationContext& c){
		c.print("myword called");
	}}
};

Interpreter i(Dictionary(customWords, sizeof(customWords) / sizeof(Word),
	&Dictionary::Default));

void setup()
{
	Serial.begin(9600);
	Serial.setTimeout(-1);
	Serial.write("ForthyShell test");
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

