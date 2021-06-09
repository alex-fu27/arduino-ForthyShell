#include <Arduino.h>
#include <ForthyShell.h>
#include <ForthyShellMacros.h>

using namespace ForthyShell;

char* output_literal_str(Stack&, char* next) 
{
	Serial.write(strtok_r(next, "\"", &next));
	return next;
}

#if __cplusplus >= 201703L
constexpr // put words into flash
#endif
const Word words[] {
	FORTHY_NUMBER(1),
	FORTHY_BINOP(+),
	FORTHY_BINOP(-),
	FORTHY_BINOP2("=", ==),
	FORTHY_BINOP2("~=", !=),
	FORTHY_BINOP(<),
	FORTHY_BINOP(<=),
	FORTHY_BINOP(>),
	FORTHY_BINOP(>=),
	FORTHY_BINOP(*),
	FORTHY_BINOP(/),
	{".", [](Stack& s, char* c) {Serial.print((int) s.pop());return c;}},
	{"u.", [](Stack& s, char* c) {Serial.print((unsigned int) s.pop());return c;}},
	{".\"", output_literal_str},
};

Interpreter i{{words, sizeof(words) / sizeof(Word)}};

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

