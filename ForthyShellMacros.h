
#define FORTHY_BINOP2(op, impl) {op, [](InterpretationContext& c) { \
	int a = c.pop(); \
	int b = c.pop(); \
	c.push(b impl a); \
}}

/* ( a b -- [a op b] */
#define FORTHY_BINOP(op) FORTHY_BINOP2(#op, op)

/* ( -- num */
#define FORTHY_NUMBER(num) {#num, [](InterpretationContext& c) { \
	c.push(num); \
}}


