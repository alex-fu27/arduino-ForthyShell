#include "ForthyShell.h"
#include "ForthyShellMacros.h"

#include <stdlib.h>

using namespace ForthyShell;

static void output_literal_str(InterpretationContext& c) 
{
	c.print(c.readToken("\""));
}

static void output_cell(InterpretationContext& c)
{
	char buf[65];
	itoa(c.pop(), buf, c.getBase());
	c.print(buf);
}

static void getBasePtr(InterpretationContext& c)
{
	c.push((int) c.getBasePtr());
}

static void store(InterpretationContext& c)
{
	int* addr = (int*) c.pop();
	if (!addr)
		return;
	int x = c.pop();
	*addr = x;
}

static void fetch(InterpretationContext& c)
{
	int* addr = (int*) c.pop();
	c.push(*addr);
}

static void cstore(InterpretationContext& c)
{
	char* addr = (char*) c.pop();
	char x = c.pop();
	*addr = x;
}

static void cfetch(InterpretationContext& c)
{
	char* addr = (char*) c.pop();
	c.push(*addr);
}

static void words(InterpretationContext& c)
{
	const Dictionary* d = &c.getDictionary();
	do {
		const Word* words = d->getWords();
		size_t num = d->getNumEntries();
		for (size_t i = 0; i < num; ++i) {
			c.print(words[i].getName());
			c.print(" ");
		}
	} while ((d = d->getNext()));
}

static void stack(InterpretationContext& c)
{
	const Stack& s = c.getStack();
	size_t num = s.getDepth();
	char buf[65];
	itoa(num, buf, 10);
	c.print("<");
	c.print(buf);
	c.print("> ");
	for (size_t i = 0; i < num; ++i) {
		itoa(s.get(i), buf, c.getBase());
		c.print(buf);
		c.print(" ");
	}
}

static void dup(InterpretationContext& c)
{
	int i = c.pop();
	c.push(i);
	c.push(i);
}

static void drop(InterpretationContext& c)
{
	(void) c.pop();
}

static void swap(InterpretationContext& c)
{
	int a = c.pop();
	int b = c.pop();
	c.push(a);
	c.push(b);
}

static void rot(InterpretationContext& c)
{
	int a = c.pop();
	int b = c.pop();
	int d = c.pop();
	c.push(b);
	c.push(a);
	c.push(d);
}


#if __cplusplus >= 201703L
constexpr
#endif
const Word defaultWords[] {
	FORTHY_NUMBER(0),
	FORTHY_NUMBER(1),
	FORTHY_BINOP(+),
	FORTHY_BINOP(-),
	{"!", store},
	{"@", fetch},
	{"c!", cstore},
	{"c@", cfetch},
	{".", output_cell},
	FORTHY_BINOP2("=", ==),
	FORTHY_BINOP2("~=", !=),
	FORTHY_BINOP(<),
	FORTHY_BINOP(<=),
	FORTHY_BINOP(>),
	FORTHY_BINOP(>=),
	FORTHY_BINOP(*),
	FORTHY_BINOP(/),
	{"dup", dup},
	{"drop", drop},
	{"swap", swap},
	{"rot", rot},
	{".\"", output_literal_str},
	{".s", stack},
	{"BASE", getBasePtr},
	{"hex", [](InterpretationContext& c){ *c.getBasePtr() = 16; }},
	{"dec", [](InterpretationContext& c){ *c.getBasePtr() = 10; }},
	{"words", words}
};

const Dictionary Dictionary::Default{ defaultWords, sizeof(defaultWords) / sizeof(Word) };
