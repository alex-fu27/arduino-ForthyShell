#include "ForthyShell.h"
#include <stdlib.h>
#include <string.h>

using namespace ForthyShell;

Error::Error(Type t, const char* i):
	type(t), info(nullptr)
{
	if (i) {
		info = (char*) malloc(strlen(i) + 1);
		strcpy(info, i);
	}
}

Error::Error(const Error& rhs):
	type(rhs.type), info(nullptr)
{
	if (rhs.info) {
		info = (char*) malloc(strlen(rhs.info) + 1);
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
		free(info);
	}
	type = rhs.type;
	info = rhs.info;
	rhs.info = nullptr;
	return *this;
}

Error::~Error()
{
	if (info) {
		free(info);
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
	contents = (int*) malloc(sizeof(*contents) * max_depth);
}

Stack::~Stack()
{
	free(contents);
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
	lastError = Error(Error::OK);
}

Error Stack::getError() const
{
	return lastError;
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

char* Word::call(Stack& stack, char* line) const
{
	return function(stack, line);
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
	return nullptr;
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

Error Interpreter::execute(const char* text)
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
				result = Error(Error::WORD_NOT_FOUND, current);
				goto cleanup;
			}
			next = word->call(stack, next);

			Error e(stack.getError());
			if (e != Error::OK) {
				stack.clear();
				result = Error(e, current);
				goto cleanup;
			}
		}
	}

cleanup:
	delete[] copy;
	return result;
}

