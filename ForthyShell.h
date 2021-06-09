#ifndef EMBEDFORTH_H
#define EMBEDFORTH_H

#include <stddef.h>

namespace e4 {
	class Exception
	{
	protected:
		char* whatstr;
	public:
		Exception(const char* what);
		virtual ~Exception();
		const char* what() const;
	};

	class StackError: public Exception
	{
	public:
		using Exception::Exception;
	};

	class StackOverflow: public StackError
	{
	public:
		StackOverflow(): StackError("stack overflow") {};
	};

	class StackUnderflow: public StackError
	{
	public:
		StackUnderflow(): StackError("stack underflow") {};
	};

	class Stack
	{
		int* contents;
		size_t position;
		const size_t max_depth;
	public:
		Stack(size_t max_depth);
		~Stack();

		void push(int i);
		int pop();
		int get(size_t pos);

		size_t getDepth() const;
		void clear();
	};

	using WordCall = void (*)(Stack&);

	class Word
	{
		const WordCall function;
		const char* name;
	public:
		Word(const char* name, const WordCall function);

		const char* getName() const;
		WordCall getFunction() const;

		void call(Stack&) const;

		void destroy();
	};

	class WordNotFound: public Exception
	{
	public:
		WordNotFound(const char* name);
	};

	class Dictionary
	{
		const Word* first;
		const size_t numEntries;
	public:
		constexpr Dictionary(const Word* first, size_t numEntries);

		const Word* find(const char* name) const;
	};

	class Interpreter
	{
		const Dictionary dict;
		Stack stack;
	public:
		Interpreter(Dictionary&& d, size_t stack_depth = 32);

		Stack& getStack();
		const Dictionary& getDict();

		void execute(const char* text);
	};

}

#endif // EMBEDFORTH_H

