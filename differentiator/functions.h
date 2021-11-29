//
// Created by IvanBrekman on 27.11.2021
//

FUNC( "abs", DIV(CL, NEW_OPP(get_code("abs"), CL, NULL)))
FUNC("sqrt", DIV(DL, MUL(NEW_VCONST(2), NEW_OPP(get_code("sqrt"), CL, NULL))))

FUNC("sin",  MUL(                    NEW_OPP(get_code("cos"), CL, NULL),  DL))
FUNC("cos",  MUL(MUL(NEW_VCONST(-1), NEW_OPP(get_code("sin"), CL, NULL)), DL))
FUNC( "tg",  DIV(DL, DEG(NEW_OPP(get_code("cos"), CL, NULL), NEW_VCONST(2))))
FUNC("ctg",  DIV(MUL(NEW_VCONST(-1), DL), DEG(NEW_OPP(get_code("sin"), CL, NULL), NEW_VCONST(2))))

FUNC("arcsin", DIV(                    DL,  NEW_OPP(get_code("sqrt"), SUB(NEW_VAL, DEG(CL, NEW_VCONST(2))), NULL)))
FUNC("arccos", DIV(MUL(NEW_VCONST(-1), DL), NEW_OPP(get_code("sqrt"), SUB(NEW_VAL, DEG(CL, NEW_VCONST(2))), NULL)))
FUNC( "arctg", DIV(                    DL,  ADD(NEW_VAL, DEG(CL, NEW_VCONST(2)))))
FUNC("arcctg", DIV(MUL(NEW_VCONST(-1), DL), ADD(NEW_VAL, DEG(CL, NEW_VCONST(2)))))

FUNC( "sh",  MUL(NEW_OPP(get_code("ch"), CL, NULL), DL))
FUNC( "ch",  MUL(NEW_OPP(get_code("sh"), CL, NULL), DL))
FUNC( "th",  DIV(DL, DEG(NEW_OPP(get_code("ch"), CL, NULL), NEW_VCONST(2))))
FUNC("cth",  DIV(MUL(NEW_VCONST(-1), DL), DEG(NEW_OPP(get_code("sh"), CL, NULL), NEW_VCONST(2))))
