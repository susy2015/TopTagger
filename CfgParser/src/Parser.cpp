#include <wchar.h>
#include "TopTagger/CfgParser/include/Parser.h"
#include "TopTagger/CfgParser/include/Scanner.h"
//#include "hcal/exception/Exception.hh"
#include "TopTagger/CfgParser/include/TTException.h"

#include <cstdio>
#include <string>

namespace hcalcfg {


    void Parser::SynErr(int n) {
	if (errDist >= minErrDist) errors->SynErr(la->line, la->col, n);
	errDist = 0;
    }

    void Parser::SemErr(const wchar_t* msg) {
	if (errDist >= minErrDist) errors->Error(t->line, t->col, msg);
	errDist = 0;
    }

    void Parser::Get() {
	for (;;) {
            t = la;
            la = scanner->Scan();
            if (la->kind <= maxT) { ++errDist; break; }

            if (dummyToken != t) {
                dummyToken->kind = t->kind;
                dummyToken->pos = t->pos;
                dummyToken->col = t->col;
                dummyToken->line = t->line;
                dummyToken->next = NULL;
                coco_string_delete(dummyToken->val);
                dummyToken->val = coco_string_create(t->val);
                t = dummyToken;
            }
            la = t;
	}
    }

    void Parser::Expect(int n) 
    {
	if (la->kind==n) Get();
        else           { SynErr(n); }
    }

    void Parser::ExpectWeak(int n, int follow)  //NEVER Called
    {
	if (la->kind == n) Get();
	else
        {
            SynErr(n);
            while (!StartOf(follow)) Get();
	}
    }

    bool Parser::WeakSeparator(int n, int syFol, int repFol)  //NEVER called
    {
	if (la->kind == n) {Get(); return true;}
	else if (StartOf(repFol)) {return false;}
	else 
        {
            SynErr(n);
            while (!(StartOf(syFol) || StartOf(repFol) || StartOf(0))) {
                Get();
            }
            return StartOf(syFol);
	}
    }

    void Parser::HcalCfg()
    {
        Line();
        while (la->kind == 4 || la->kind == 21) //string (not in "") or group
        {
            Line();
        }
    }

    void Parser::Line()
    {
        if (la->kind == 4)  //string (not in "")
        {
            GlobalConditional();
        }
        else if (la->kind == 21)  //group
        {
            GroupDef();
        }
        else SynErr(23);
    }

    void Parser::GlobalConditional()
    {
        Expect(4);   // string (not in "")
        m_builder.setNamespace(CfgBuilder::toString(t->val)); 
        if (la->kind == 8)  // {
        {
            CompoundIC();
        }
        else if (la->kind == 10)  // ::
        {
            SimpleIC();
        }
        else SynErr(24);
    }

    void Parser::GroupDef()
    {
        Expect(21); //group
        Expect(4);  //string (not in "")
        Expect(13); // =
        cfg::TermAnd terms; 
        CompoundTerm(terms);
    }

    void Parser::CompoundIC()
    {
        Expect(8); // {
        AssignOrCond();
        while (la->kind == 4 || la->kind == 14) //string (not in "") || if
        {
            AssignOrCond();
        }
        while (!(la->kind == 0 || la->kind == 9)) // !( EOF || } )
        {
            SynErr(25);
            Get();
        }
        Expect(9); // }
    }

    void Parser::SimpleIC()
    {
        Expect(10); // ::
        Assign();
    }

    void Parser::AssignOrCond()
    {
        if (la->kind == 4) //string (not in "")
        {
            Assign();
        }
        else if (la->kind == 14) //if
        {
            ConditionalAssign();
        }
        else SynErr(26);
    }

    void Parser::Assign() {
        Item();
        Expect(13); // =
        Literal();
        m_builder.assign(); 
    }

    void Parser::ConditionalAssign()
    {
        Expect(14); //if
        Expect(15); //(
        cfg::TermAnd* terms=new cfg::TermAnd(); 
        CompoundTerm(*terms);
        while (!(la->kind == 0 || la->kind == 16)) //!(EOF || ')' )
        {
            SynErr(27);
            Get();
        }
        Expect(16); // )
        m_builder.newCondition()->set(terms); 
        Expect(8); // {
        AssignOrCond();
        while (la->kind == 4 || la->kind == 14) // string (not in ") || if
        {
            AssignOrCond();
        }
        Expect(9); //}
        m_builder.popCondition(); 
    }

    void Parser::Item()  //Only called by Assign()
    {
        Expect(4); //string (not in ")
        m_builder.theItemName = CfgBuilder::toString(t->val);
        m_builder.theItemIndex=-1; 
        while (la->kind == 11) // [
        {
            Get();
            Expect(1); //integer
            m_builder.theItemIndex=CfgBuilder::toInt(t->val); 
            while (!(la->kind == 0 || la->kind == 12)) //!( EOF || ] )
            {
                SynErr(28);
                Get();
            }
            Expect(12); // ]
        }
    }

    void Parser::Literal() 
    {
        if (la->kind == 1) //integer
        {
            Get();
            m_builder.iLiteral(t->val);  
        }
        else if (la->kind == 22) //float
        {
            Get();
            m_builder.fLiteral(t->val);  
        }
        else if (la->kind == 5) //true
        {
            Get();
            m_builder.bLiteral(true); 
        }
        else if (la->kind == 6) //false
        {
            Get();
            m_builder.bLiteral(false); 
        }
        else if (la->kind == 3) //string (in "")
        {
            Get();
            m_builder.sLiteral(t->val); 
        }
        else SynErr(29);
    }

    void Parser::CompoundTerm(cfg::TermAnd& at)
    {
        cfg::SimpleTerm* term=0; 
        Term(term);
        if (term!=0)
        {
            at.And(term);
            term=0;
        } 
        while (la->kind == 17)  // &&
        {
            Get();
            Term(term);
            if (term!=0)
            {
                at.And(term);
                term=0;
            } 
        }
    }

    void Parser::Term(cfg::SimpleTerm*& term)
    {
        if (la->kind == 4) //string (not in ")
        {
            Get();
            std::string item(CfgBuilder::toString(t->val)); 
            if (la->kind == 7)  //conditional
            {
                Get();
                std::string ops(CfgBuilder::toString(t->val)); 
                Literal();
                term=m_builder.simpleTerm(item, ops); 
            }
            else if (la->kind == 19) //in
            {
                Get();
                List();
                term=m_builder.listTerm(item,"in"); 
            }
            else SynErr(30);
        }
        else if (la->kind == 20) //is
        {
            Get();
            Expect(4); //string (not in ")
        }
        else SynErr(31);
    }

    void Parser::List()
    {
        Expect(15); // (
        Literal();
        while (la->kind == 18) // ,
        {
            Get();
            Literal();
        }
        Expect(16); // )
    }




// If the user declared a method Init and a mehtod Destroy they should
// be called in the contructur and the destructor respctively.
//
// The following templates are used to recognize if the user declared
// the methods Init and Destroy.

    template<typename T>
    struct ParserInitExistsRecognizer {
	template<typename U, void (U::*)() = &U::Init>
	struct ExistsIfInitIsDefinedMarker{};

	struct InitIsMissingType {
            char dummy1;
	};
	
	struct InitExistsType {
            char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static InitIsMissingType is_here(...);

	// exist only if ExistsIfInitIsDefinedMarker is defined
	template<typename U>
	static InitExistsType is_here(ExistsIfInitIsDefinedMarker<U>*);

	enum { InitExists = (sizeof(is_here<T>(NULL)) == sizeof(InitExistsType)) };
    };

    template<typename T>
    struct ParserDestroyExistsRecognizer {
	template<typename U, void (U::*)() = &U::Destroy>
	struct ExistsIfDestroyIsDefinedMarker{};

	struct DestroyIsMissingType {
            char dummy1;
	};
	
	struct DestroyExistsType {
            char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static DestroyIsMissingType is_here(...);

	// exist only if ExistsIfDestroyIsDefinedMarker is defined
	template<typename U>
	static DestroyExistsType is_here(ExistsIfDestroyIsDefinedMarker<U>*);

	enum { DestroyExists = (sizeof(is_here<T>(NULL)) == sizeof(DestroyExistsType)) };
    };

// The folloing templates are used to call the Init and Destroy methods if they exist.

// Generic case of the ParserInitCaller, gets used if the Init method is missing
    template<typename T, bool = ParserInitExistsRecognizer<T>::InitExists>
    struct ParserInitCaller {
	static void CallInit(T *t) {
            // nothing to do
            (void)t;
	}
    };

// True case of the ParserInitCaller, gets used if the Init method exists
    template<typename T>
    struct ParserInitCaller<T, true> {
	static void CallInit(T *t) {
            t->Init();
	}
    };

// Generic case of the ParserDestroyCaller, gets used if the Destroy method is missing
    template<typename T, bool = ParserDestroyExistsRecognizer<T>::DestroyExists>
    struct ParserDestroyCaller {
	static void CallDestroy(T *t) {
            // nothing to do
            (void)t;
	}
    };

// True case of the ParserDestroyCaller, gets used if the Destroy method exists
    template<typename T>
    struct ParserDestroyCaller<T, true> {
	static void CallDestroy(T *t) {
            t->Destroy();
	}
    };

    void Parser::Parse() {
	t = NULL;
	la = dummyToken = new Token();
	la->val = coco_string_create(L"Dummy Token");
	Get();
	HcalCfg();
	Expect(0);
    }

    Parser::Parser(Scanner *scanner) {
	maxT = 22;

	ParserInitCaller<Parser>::CallInit(this);
	dummyToken = NULL;
	t = la = NULL;
	minErrDist = 2;
	errDist = minErrDist;
	this->scanner = scanner;
	errors = new Errors();
    }

    bool Parser::StartOf(int s) { //Only called in NEVER called functions
	const bool T = true;
	const bool x = false;

	static bool set[1][24] = {
            {T,x,x,x, x,x,x,x, x,T,x,x, T,x,x,x, T,x,x,x, x,x,x,x}
	};



	return set[s][la->kind];
    }

    Parser::~Parser() {
	ParserDestroyCaller<Parser>::CallDestroy(this);
	delete errors;
	delete dummyToken;
    }

    Errors::Errors() {
	count = 0;
    }

    void Errors::SynErr(int line, int col, int n) {
	wchar_t* s;
	switch (n) {
        case 0: s = coco_string_create(L"EOF expected"); break;
        case 1: s = coco_string_create(L"integer expected"); break;
        case 2: s = coco_string_create(L"esc expected"); break;
        case 3: s = coco_string_create(L"string expected"); break;
        case 4: s = coco_string_create(L"ident expected"); break;
        case 5: s = coco_string_create(L"trueval expected"); break;
        case 6: s = coco_string_create(L"falseval expected"); break;
        case 7: s = coco_string_create(L"op expected"); break;
        case 8: s = coco_string_create(L"\"{\" expected"); break;
        case 9: s = coco_string_create(L"\"}\" expected"); break;
        case 10: s = coco_string_create(L"\"::\" expected"); break;
        case 11: s = coco_string_create(L"\"[\" expected"); break;
        case 12: s = coco_string_create(L"\"]\" expected"); break;
        case 13: s = coco_string_create(L"\"=\" expected"); break;
        case 14: s = coco_string_create(L"\"if\" expected"); break;
        case 15: s = coco_string_create(L"\"(\" expected"); break;
        case 16: s = coco_string_create(L"\")\" expected"); break;
        case 17: s = coco_string_create(L"\"&&\" expected"); break;
        case 18: s = coco_string_create(L"\",\" expected"); break;
        case 19: s = coco_string_create(L"\"in\" expected"); break;
        case 20: s = coco_string_create(L"\"is\" expected"); break;
        case 21: s = coco_string_create(L"\"group\" expected"); break;
        case 22: s = coco_string_create(L"??? expected"); break;
        case 23: s = coco_string_create(L"invalid Line"); break;
        case 24: s = coco_string_create(L"invalid GlobalConditional"); break;
        case 25: s = coco_string_create(L"this symbol not expected in CompoundIC"); break;
        case 26: s = coco_string_create(L"invalid AssignOrCond"); break;
        case 27: s = coco_string_create(L"this symbol not expected in ConditionalAssign"); break;
        case 28: s = coco_string_create(L"this symbol not expected in Item"); break;
        case 29: s = coco_string_create(L"invalid Literal"); break;
        case 30: s = coco_string_create(L"invalid Term"); break;
        case 31: s = coco_string_create(L"invalid Term"); break;

        default:
        {
            wchar_t format[20];
            wchar_t buffer[1024];
            swprintf(buffer,1024,format, 20, L"error %d", n);
            char cbuffer[1024];
            memset(cbuffer,0,1024);
            for (int i=0; i<1024 && buffer[i]!=0; i++) cbuffer[i]=wctob(buffer[i]);
            //LOG4CPLUS_ERROR(log4cplus::Logger::getInstance("Hcal.CfgScriptParser"),cbuffer);
            printf("CfgScriptParser: %s\n", cbuffer);
            s = coco_string_create(format);
        }
        break;
	}
        wchar_t buffer[1024];
        swprintf(buffer,1024,L"-- line %d col %d: %ls\n", line, col, s);
        char cbuffer[1024];
        memset(cbuffer,0,1024);
        for (int i=0; i<1024 && buffer[i]!=0; i++) cbuffer[i]=wctob(buffer[i]);
        //LOG4CPLUS_ERROR(log4cplus::Logger::getInstance("Hcal.CfgScriptParser"),cbuffer);
        printf("CfgScriptParser: %s\n", cbuffer);
	coco_string_delete(s);
	count++;
    }

    void Errors::Error(int line, int col, const wchar_t *s) {
        wchar_t buffer[1024];
        swprintf(buffer,1024,L"-- line %d col %d: %ls\n", line, col, s);
        char cbuffer[1024];
        memset(cbuffer,0,1024);
        for (int i=0; i<1024 && buffer[i]!=0; i++) cbuffer[i]=wctob(buffer[i]);
        //LOG4CPLUS_ERROR(log4cplus::Logger::getInstance("Hcal.CfgScriptParser"),cbuffer);
        printf("CfgScriptParser: %s\n", cbuffer);
	count++;
    }

    void Errors::Warning(int line, int col, const wchar_t *s) {
        wchar_t buffer[1024];
        swprintf(buffer,1024,L"-- line %d col %d: %ls\n", line, col, s);
        char cbuffer[1024];
        memset(cbuffer,0,1024);
        for (int i=0; i<1024 && buffer[i]!=0; i++) cbuffer[i]=wctob(buffer[i]);
        //LOG4CPLUS_WARN(log4cplus::Logger::getInstance("Hcal.CfgScriptParser"),cbuffer);
        printf("CfgScriptParser: %s\n", cbuffer);
    }

    void Errors::Warning(const wchar_t *s) {
        wchar_t buffer[1024];
        swprintf(buffer,1024,L"%ls\n", s);
        char cbuffer[1024];
        memset(cbuffer,0,1024);
        for (int i=0; i<1024 && buffer[i]!=0; i++) cbuffer[i]=wctob(buffer[i]);
        //LOG4CPLUS_WARN(log4cplus::Logger::getInstance("Hcal.CfgScriptParser"),cbuffer);
        printf("CfgScriptParser: %s\n", cbuffer);
    }

    void Errors::Exception(const wchar_t* s) {
        wchar_t buffer[1024];
        swprintf(buffer,1024,L"%ls", s);
        char cbuffer[1024];
        memset(cbuffer,0,1024);
        for (int i=0; i<1024 && buffer[i]!=0; i++) cbuffer[i]=wctob(buffer[i]);
        //LOG4CPLUS_FATAL(log4cplus::Logger::getInstance("Hcal.CfgScriptParser"),cbuffer);
        printf("CfgScriptParser: %s\n", cbuffer);
        //XCEPT_RAISE(hcal::exception::Exception,cbuffer);
        //throw std::string(cbuffer);
        THROW_TTEXCEPTION(cbuffer);
    }

} // namespace

