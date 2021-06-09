#include <Arduino.h>
#include <ForthyShell.h>

using namespace ForthyShell;

const Word words[] {
	{"1", [](Stack& s) {s.push(1);}},
	{".", [](Stack& s) {Serial.print(s.pop());}}
};

Interpreter i{{words, 2}};

void setup()
{
	Serial.begin(9600);
	Serial.setTimeout(-1);
}

void loop()
{
	Serial.write("\nok ");
	String s = Serial.readStringUntil('\n');
	Error e = i.execute(s.c_str());
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

