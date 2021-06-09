#include "ForthyShell.h"
#include <stdlib.h>
#include <string.h>

#include <Arduino.h>

using namespace ForthyShell;

Error::Error(Type t, const char* i):
	type(t), info(nullptr)
{
	if (i) {
		info = new char[strlen(i) + 1];
		strcpy(info, i);
	}
}

Error::Error(const Error& rhs):
	type(rhs.type), info(nullptr)
{
	if (rhs.info) {
		info = new char[strlen(rhs.info) + 1];
		strcpy(info, rhs.info);
	}
}

Error::Error(Error&& rhs):
	type(rhs.type), info(rhs.info)
{
	rhs.info = nullptr;
}

Error& Error::operator =(Error&& rhs)
{
	if (info) {
		delete[] info;
	}
	type = rhs.type;
	info = rhs.info;
	rhs.info = nullptr;
	return *this;
}

Error::~Error()
{
	if (info) {
		delete[] info;
	}
}

const char* Error::getMessage() const
{
	const char* messages[]{
		"ok",
		"stack underflow",
		"stack overflow",
		"undefined word"
	};
	return messages[type];
}

Stack::Stack(size_t max_depth):
	lastError(Error::OK), position(0), max_depth(max_depth)
{
	contents = new int[sizeof(*contents) * max_depth];
}

Stack::~Stack()
{
	delete[] contents;
}

void Stack::push(int i)
{
	if (position == max_depth) {
		lastError = Error(Error::STACK_OVERFLOW);
		return;
	}
	contents[position++] = i;
}

int Stack::pop()
{
	if (position <= 0) {
		lastError = Error(Error::STACK_UNDERFLOW);
		return 0;
	}
	return contents[--position];
}

int Stack::get(size_t pos) const
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
	lastError = Error(Error::OK);
}

Error Stack::getError() const
{
	return lastError;
}


const char* Word::getName() const
{
	return name;
}

WordCall Word::getFunction() const
{
	return function;
}

void Word::call(InterpretationContext& c) const
{
	function(c);
}

const Word* Dictionary::find(const char* name) const
{
	for (size_t i = 0; i < numEntries; ++i) {
		const Word* ptr = &first[i];
		int c = strcmp(name, ptr->getName());
		if (c == 0) {
			return ptr;
		}
	}
	if (!searchAfter)
		return nullptr;
	return searchAfter->find(name);
}

Stack& Interpreter::getStack()
{
	return stack;
}

const Dictionary& Interpreter::getDict()
{
	return dict;
}

int Interpreter::getBase()
{
	return base;
}

bool Interpreter::executeLiteral(const char* str)
{
	char* endptr;
	long x = strtol(str, &endptr, getBase());
	stack.push(x);
	return (*endptr == '\0');
}

Error Interpreter::execute(const char* text, PrintCallback print)
{
	char* copy = new char[strlen(text) + 1];
	strcpy(copy, text);
	char* next = copy;
	char* current;
	Error result(Error::OK);
	while((current = strtok_r(next, " ", &next))) {
		if (current[0] != '\0') {
			const Word* word = getDict().find(current);
			if (!word) {
				if (!executeLiteral(current)) {
					result = Error(Error::WORD_NOT_FOUND, current);
				}
				goto cleanup;
			}
			InterpretationContext ic(*this, next, print);
			word->call(ic);
			next = ic.getLineBuffer();

			Error e(stack.getError());
			if (e != Error::OK) {
				result = Error(e, current);
				goto cleanup;
			}
		}
	}

cleanup:
	if (result != Error::OK) {
		stack.clear();
	}

	delete[] copy;
	return result;
}

char* InterpretationContext::readToken(const char *delim)
{
	return strtok_r(lineBuffer, delim, &lineBuffer);
}

void InterpretationContext::push(int i)
{
	interpreter.getStack().push(i);
}

int InterpretationContext::pop()
{
	return interpreter.getStack().pop();
}

char* InterpretationContext::getLineBuffer()
{
	return lineBuffer;
}

const Dictionary& InterpretationContext::getDictionary()
{
	return interpreter.getDict();
}

Stack& InterpretationContext::getStack()
{
	return interpreter.getStack();
}

void InterpretationContext::print(const char* str)
{
	printCallback(str);
}

int InterpretationContext::getBase()
{
	return interpreter.getBase();
}

