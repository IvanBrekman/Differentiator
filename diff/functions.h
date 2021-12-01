//
// Created by IvanBrekman on 27.11.2021
//

FUNC( "abs", 5, "|L|", DIV(CL, NEW_OPP(get_code("abs"), CL, NULL)))
FUNC("sqrt", 3, "\\sqrt{L}", DIV(DL, MUL(NEW_VCONST(2), NEW_OPP(get_code("sqrt"), CL, NULL))))

FUNC("sin",  5, "\\sin{L}", MUL(                    NEW_OPP(get_code("cos"), CL, NULL),  DL))
FUNC("cos",  5, "\\cos{L}", MUL(MUL(NEW_VCONST(-1), NEW_OPP(get_code("sin"), CL, NULL)), DL))
FUNC( "tg",  5,  "\\tg{L}", DIV(DL, DEG(NEW_OPP(get_code("cos"), CL, NULL), NEW_VCONST(2))))
FUNC("ctg",  5, "\\ctg{L}", DIV(MUL(NEW_VCONST(-1), DL), DEG(NEW_OPP(get_code("sin"), CL, NULL), NEW_VCONST(2))))

FUNC("arcsin", 5, "\\arcsin{L}", DIV(                    DL,  NEW_OPP(get_code("sqrt"), SUB(NEW_VAL, DEG(CL, NEW_VCONST(2))), NULL)))
FUNC("arccos", 5, "\\arccos{L}", DIV(MUL(NEW_VCONST(-1), DL), NEW_OPP(get_code("sqrt"), SUB(NEW_VAL, DEG(CL, NEW_VCONST(2))), NULL)))
FUNC( "arctg", 5,  "\\arctg{L}", DIV(                    DL,  ADD(NEW_VAL, DEG(CL, NEW_VCONST(2)))))
FUNC("arcctg", 5, "\\arcctg{L}", DIV(MUL(NEW_VCONST(-1), DL), ADD(NEW_VAL, DEG(CL, NEW_VCONST(2)))))

FUNC( "sh",  5,  "\\sh{L}", MUL(NEW_OPP(get_code("ch"), CL, NULL), DL))
FUNC( "ch",  5,  "\\ch{L}", MUL(NEW_OPP(get_code("sh"), CL, NULL), DL))
FUNC( "th",  5,  "\\th{L}", DIV(DL, DEG(NEW_OPP(get_code("ch"), CL, NULL), NEW_VCONST(2))))
FUNC("cth",  5, "\\cth{L}", DIV(MUL(NEW_VCONST(-1), DL), DEG(NEW_OPP(get_code("sh"), CL, NULL), NEW_VCONST(2))))
