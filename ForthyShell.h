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
		int get(size_t pos) const;

		size_t getDepth() const;
		void clear();

		Error getError() const;

	private:
		Error lastError;
		int* contents;
		size_t position;
		const size_t max_depth;
	};

	class InterpretationContext;
	using WordCall = void (*)(InterpretationContext&);

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

		void call(InterpretationContext&) const;
	};

	class Dictionary
	{
		const Word* first;
		const size_t numEntries;
		const Dictionary* searchAfter;
	public:
		constexpr Dictionary(const Word* first, size_t numEntries,
				const Dictionary* searchAfter = nullptr):
			first(first), numEntries(numEntries), searchAfter(searchAfter)
		{}

		const Word* find(const char* name) const;
		const Word* getWords() const { return first; }
		const Dictionary* getNext() const { return searchAfter; };
		size_t getNumEntries() const { return numEntries; }
		size_t getNumWords() const {
			size_t i = getNumEntries();
			if (searchAfter)
				i += searchAfter->getNumWords();
			return i;
		}

		static const Dictionary Default;
	};

	using OutputCallback = void (*)(const char*);

	class Interpreter
	{
		const Dictionary& dict;
		Stack stack;
		int base;

		bool executeLiteral(const char* test);

	public:
		using PrintCallback = void (*)(const char*);

		Interpreter(const Dictionary& d, size_t stack_depth = 32):
			dict(d), stack(stack_depth), base(16)
		{}

		Stack& getStack();
		const Dictionary& getDict();
		int* getBasePtr();

		Error execute(const char* text, PrintCallback print);
	};

	class InterpretationContext
	{
	public:

		InterpretationContext(Interpreter& i, char* lineBuffer,
				Interpreter::PrintCallback pcb):
			interpreter(i), lineBuffer(lineBuffer), printCallback(pcb)
		{}

		char* readToken(const char* delim);
		void push(int);
		int pop();
		char* getLineBuffer();
		void print(const char*);
		const Dictionary& getDictionary();
		int* getBasePtr();
		int getBase();
		Stack& getStack();
	private:
		Interpreter& interpreter;
		char* lineBuffer;
		Interpreter::PrintCallback printCallback;
	};

}

#endif // FORTHYSHELL_H

