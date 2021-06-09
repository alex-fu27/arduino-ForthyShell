#include <Arduino.h>
#include <ForthyShell.h>

using namespace ForthyShell;

Interpreter i{Dictionary::Default};

void setup()
{
	Serial.begin(9600);
	Serial.setTimeout(-1);
	Serial.write("ForthyShell test");
}

void out(const char* c)
{
	Serial.write(c);
}

void loop()
{
	Serial.write("\nok ");
	String s = Serial.readStringUntil('\n');
	Error e = i.execute(s.c_str(), out);
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

