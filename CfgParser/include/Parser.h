#if !defined(hcalcfg_COCO_PARSER_H__)
#define hcalcfg_COCO_PARSER_H__

#include "Defs.h"

#include "TopTagger/CfgParser/include/Scanner.h"

namespace hcalcfg {


class Errors {
public:
	int count;			// number of errors detected

	Errors();
	void SynErr(int line, int col, int n);
	void Error(int line, int col, const wchar_t *s);
	void Warning(int line, int col, const wchar_t *s);
	void Warning(const wchar_t *s);
	void Exception(const wchar_t *s);

}; // Errors

class Parser {
private:
	enum {
		_EOF=0,
		_integer=1,
		_esc=2,
		_string=3,
		_ident=4,
		_trueval=5,
		_falseval=6,
		_op=7
	};
	int maxT;

	Token *dummyToken;
	int errDist;
	int minErrDist;

	void SynErr(int n);
	void Get();
	void Expect(int n);
	bool StartOf(int s);
	void ExpectWeak(int n, int follow);
	bool WeakSeparator(int n, int syFol, int repFol);

public:
	Scanner *scanner;
	Errors  *errors;

	Token *t;			// last recognized token
	Token *la;			// lookahead token

CfgBuilder m_builder;




	Parser(Scanner *scanner);
	~Parser();
	void SemErr(const wchar_t* msg);

	void HcalCfg();
	void Line();
	void GlobalConditional();
	void GroupDef();
	void CompoundIC();
	void SimpleIC();
	void AssignOrCond();
	void Assign();
	void ConditionalAssign();
	void Item();
	void Literal();
	void CompoundTerm(cfg::TermAnd& at);
	void Term(cfg::SimpleTerm*& term);
	void List();

	void Parse();

}; // end Parser

} // namespace


#endif

