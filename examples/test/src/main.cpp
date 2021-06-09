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
}

void loop()
{
	Serial.write("\nok ");
	String s = Serial.readStringUntil('\n');
	try {
		i.execute(s.c_str());
	} catch(const Exception& e) {
		Serial.write("error: ");
		Serial.println(e.what());
	}
}

