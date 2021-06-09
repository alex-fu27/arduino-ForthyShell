
#define FORTHY_BINOP2(op, impl) {op, [](ForthyShell::Stack& s, char* n) { \
	int a = s.pop(); \
	int b = s.pop(); \
	s.push(a impl b); \
	return n; \
}}

/* ( a b -- [a op b] */
#define FORTHY_BINOP(op) FORTHY_BINOP2(#op, op)

/* ( -- num */
#define FORTHY_NUMBER(num) {#num, [](ForthyShell::Stack& s, char* n) { \
	s.push(num); \
	return n; \
}}


