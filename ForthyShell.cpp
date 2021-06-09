#include "ForthyShell.h"
#include <stdlib.h>
#include <string.h>

using namespace ForthyShell;

Exception::Exception(const char* what)
{
	size_t len = strlen(what);
	whatstr = new char[len + 1];
	strcpy(whatstr, what);
}

Exception::~Exception()
{
	delete[] whatstr;
}

const char* Exception::what() const
{
	return whatstr;
}


Stack::Stack(size_t max_depth):
	position(0), max_depth(max_depth)
{
	contents = (int*) malloc(sizeof(*contents) * max_depth);
}

Stack::~Stack()
{
	free(contents);
}

void Stack::push(int i)
{
	if (position == max_depth) {
		throw StackOverflow();
	}
	contents[position++] = i;
}

int Stack::pop()
{
	if (position <= 0) {
		throw StackUnderflow();
	}
	return contents[--position];
}

int Stack::get(size_t pos)
{
	return contents[pos];
}

size_t Stack::getDepth() const
{
	return position;
}

void Stack::clear()
{
	position = 0;
}


Word::Word(const char* name, const WordCall function):
	function(function), name(name)
{}

const char* Word::getName() const
{
	return name;
}

WordCall Word::getFunction() const
{
	return function;
}

void Word::call(Stack& stack) const
{
	function(stack);
}

WordNotFound::WordNotFound(const char* name):
	Exception("word not found: \"")
{
	realloc(whatstr, strlen(whatstr) + strlen(name) + 2);
	strcat(whatstr, name);
	strcat(whatstr, "\"");
}

constexpr Dictionary::Dictionary(const Word* first, size_t numEntries):
	first(first), numEntries(numEntries)
{}

const Word* Dictionary::find(const char* name) const
{
	for (size_t i = 0; i < numEntries; ++i) {
		const Word* ptr = &first[i];
		int c = strcmp(name, ptr->getName());
		if (c == 0) {
			return ptr;
		}
	}
	throw WordNotFound(name);
}

Interpreter::Interpreter(Dictionary&& d, size_t stack_depth):
	dict(d), stack(stack_depth)
{}

Stack& Interpreter::getStack()
{
	return stack;
}

const Dictionary& Interpreter::getDict()
{
	return dict;
}

void Interpreter::execute(const char* text)
{
	char* copy = new char[strlen(text) + 1];
	try {
		char* current = copy;
		char* next;
	    while(current) {
			next = strtok(current, " ");
			if (next[0] != '\0') {
				const Word* word = getDict().find(current);
				word->call(stack);
			}
			current = next;
		}

		delete[] copy;
	} catch (const Exception& e) {
		stack.clear();
		delete[] copy;
		throw;
	}
}

