#ifndef FORTHYSHELL_H
#define FORTHYSHELL_H

#include <stddef.h>

namespace ForthyShell {
	class Error {
	public:
		enum Type {
			OK = 0,
			STACK_UNDERFLOW = 1,
			STACK_OVERFLOW = 2,
			WORD_NOT_FOUND = 3
		};

		Error(Type type, const char* info = nullptr);
		Error(const Error&);
		Error(Error&&);
		~Error();
		Error& operator =(Error&&);

		const char* getInfo() const { return info; }
		const char* getMessage() const;

		operator Type() { return type; }
	private:
		Type type;
		char* info;
	};

	class Stack
	{
	public:
		Stack(size_t max_depth);
		~Stack();

		void push(int i);
		int pop();
		int get(size_t pos);

		size_t getDepth() const;
		void clear();

		Error getError() const;

	private:
		Error lastError;
		int* contents;
		size_t position;
		const size_t max_depth;
	};

	using WordCall = char* (*)(Stack&, char*);

	class Word
	{
		const WordCall function;
		const char* name;
	public:
		constexpr Word(const char* name, const WordCall function):
			function(function), name(name)
		{}

		const char* getName() const;
		WordCall getFunction() const;

		char* call(Stack&, char*) const;
	};

	class Dictionary
	{
		const Word* first;
		const size_t numEntries;
	public:
		constexpr Dictionary(const Word* first, size_t numEntries):
			first(first), numEntries(numEntries)
		{}

		const Word* find(const char* name) const;
	};

	class Interpreter
	{
		const Dictionary dict;
		Stack stack;
	public:
		Interpreter(Dictionary&& d, size_t stack_depth = 32):
			dict(d), stack(stack_depth)
		{}

		Stack& getStack();
		const Dictionary& getDict();

		Error execute(const char* text);
	};

}

#endif // FORTHYSHELL_H

