#include "token.hpp"
#include <iostream>
#include <string>
using namespace std;

// Aquesta funció té cost O(1)
token::token(codi cod) throw(error)
{
    switch (cod)
    {
    case CT_ENTERA:
        throw error(ConstructoraInadequada);
        break;
    case CT_RACIONAL:
        throw error(ConstructoraInadequada);
        break;
    case CT_REAL:
        throw error(ConstructoraInadequada);
        break;
    case VARIABLE:
        throw error(ConstructoraInadequada);
        break;
    default:
        co = cod;
        break;
    }
}
// Aquesta funció té cost O(1)
token::token(int n) throw(error)
{
    data.enter = n;
    co = CT_ENTERA;
}
// Aquesta funció té cost O(1)
token::token(const racional &r) throw(error)
{
    data.rn = r;
    co = CT_RACIONAL;
}
// Aquesta funció té cost O(1)
token::token(double x) throw(error)
{
    data.real = x;
    co = CT_REAL;
}
// Aquesta funció té cost O(n) on n és la longitud de var_name.
token::token(const string &var_name) throw(error)
{
    if (var_name == "unassign" or var_name == "e" or var_name == "sqrt" or var_name == "log" or var_name == "exp" or var_name == "evalf" or var_name == "")
        throw error(IdentificadorIncorrecte);
    for (int i = 0; i < int(var_name.size()); i++)
    {
        if (not((var_name[i] >= 'A' and var_name[i] <= 'Z') or (var_name[i] >= 'a' and var_name[i] <= 'z') or var_name[i] == '_'))
            throw error(IdentificadorIncorrecte);
    }
    data.s = new string(var_name);
    co = VARIABLE;
}

// Aquesta funció té cost O(1)
token::token(const token &t) throw(error)
{
    delete data.s;
    co = t.co;
    switch (t.co)
    {
    case CT_ENTERA:
        data.enter = t.data.enter;
        break;
    case CT_RACIONAL:
        data.rn = t.data.rn;
        break;
    case CT_REAL:
        data.real = t.data.real;
        break;
    case VARIABLE:
        data.s = new string(*t.data.s);
        break;
    default:
        break;
    }
}
// Aquesta funció té cost O(1)
token &token::operator=(const token &t) throw(error)
{
    if (co == VARIABLE)
        delete data.s;
    co = t.co;
    switch (t.co)
    {
    case CT_ENTERA:
        data.enter = t.data.enter;
        break;
    case CT_RACIONAL:
        data.rn = t.data.rn;
        break;
    case CT_REAL:
        data.real = t.data.real;
        break;
    case VARIABLE:
        data.s = new string(*t.data.s);
        break;
    default:
        break;
    }
    return *this;
}
// Aquesta funció té cost O(1)
token::~token() throw()
{
    if (co == VARIABLE)
        delete data.s;
}

// Aquesta funció té cost O(1)
token::codi token::tipus() const throw()
{
    return co;
}
// Aquesta funció té cost O(1)
int token::valor_enter() const throw(error)
{
    if (co != CT_ENTERA)
        throw error(ConsultoraInadequada);
    return data.enter;
}
// Aquesta funció té cost O(1)
racional token::valor_racional() const throw(error)
{
    if (co != CT_RACIONAL)
        throw error(ConsultoraInadequada);
    return data.rn;
}
// Aquesta funció té cost O(1)
double token::valor_real() const throw(error)
{
    if (co != CT_REAL)
        throw error(ConsultoraInadequada);
    return data.real;
}
// Aquesta funció té cost O(1)
string token::identificador_variable() const throw(error)
{
    if (co != VARIABLE)
        throw error(ConsultoraInadequada);
    return *(data.s);
}

// Aquesta funció té cost O(1)
bool token::operator==(const token &t) const throw()
{
    if (tipus() != t.tipus())
        return false;
    else
    {
        if (tipus() == CT_ENTERA)
            return valor_enter() == t.valor_enter();
        if (tipus() == CT_RACIONAL)
            return valor_racional() == t.valor_racional();
        if (tipus() == CT_REAL)
            return valor_real() == t.valor_real();
        if (tipus() == VARIABLE)
            return identificador_variable() == t.identificador_variable();
        return true;
    }
}
// Aquesta funció té cost O(1)
bool token::operator!=(const token &t) const throw()
{
    return not(*this == t);
}

// Aquesta funció té cost O(1)
bool token::isOperator(const token &a)
{
    switch (a.tipus())
    {
    case SUMA:
    case RESTA:
    case MULTIPLICACIO:
    case DIVISIO:
    case EXPONENCIACIO:
    case CANVI_DE_SIGNE:
    case SIGNE_POSITIU:
        return true;
        break;
    default:
        return false;
    };
}

// Aquesta funció té cost O(1)
bool token::operator>(const token &t) const throw(error)
{
    if (not isOperator(t) or not isOperator(*this))
        throw error(14);
    if (co == EXPONENCIACIO and t.co != EXPONENCIACIO)
        return true;
    if ((co == CANVI_DE_SIGNE or co == SIGNE_POSITIU) and (t.co != EXPONENCIACIO and t.co != CANVI_DE_SIGNE and t.co != SIGNE_POSITIU))
        return true;
    if ((co == MULTIPLICACIO or co == DIVISIO) and (t.co == SUMA or t.co == RESTA))
        return true;
    return false;
}
// Aquesta funció té cost O(1)
bool token::operator<(const token &t) const throw(error)
{
    if (not isOperator(t) or not isOperator(*this))
        throw error(14);
    if (co == EXPONENCIACIO)
        return false;
    if ((co == CANVI_DE_SIGNE or co == SIGNE_POSITIU) and t.co != EXPONENCIACIO)
        return false;
    if ((co == MULTIPLICACIO or co == DIVISIO) and (t.co != EXPONENCIACIO and t.co != CANVI_DE_SIGNE and t.co != SIGNE_POSITIU))
        return false;
    if ((co == SUMA or co == RESTA) and (t.co == SUMA or t.co == RESTA))
        return false;
    return true;
}
