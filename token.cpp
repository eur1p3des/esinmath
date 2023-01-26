#include "token.hpp"
#include <iostream>
#include <string>
using namespace std;
/*Constructores: Construeixen tokens pels operadors, les constants enteres,
les constants racionals, les constants reals i les variables(el seu
identificador), respectivament.

La primera constructora s'utiliza para aquells tokens que són operadors,
funcions predefinides, noms de comandes o símbols de ``puntuació''{} per
tant es produeix un error si el codi és CT_ENTERA, CT_RACIONAL, CT_REAL o
VARIABLE.

L'última constructora torna un error si l'string donat no és un
identificador vàlid per una variable: ha d'estar format exclusivament per
lletres majúscules, minúscules o caràcters de subratllat _. Dit d'una altra
forma, només conté caràcters els codis ASCII dels quals estan entre 65
('A') i 90('Z'), entre 97 ('a') i 122 ('z') o el 95 ('_') i no pot
coincidir amb un nom reseoperadorat: unassign, e, sqrt, log, exp i evalf.*/
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

// Constructora por còpia, assignació i destructora.
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

/*Consultores: Retornen respectivament el codi i el valor (en el cas de
constants enteres, racionals o reals) o l'identificador (en el cas de
variables). Es produeix un error si apliquem una consultora inadequada
sobre un token, p.e. si apliquem valor_enter sobre un token que no sigui
una CT_ENTERA.*/
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

/*Igualtat i desigualtat entre tokens. Dos tokens es consideren iguals si els
seus codis ho són i si 1) en cas de ser CT_ENTERA, CT_RACIONAL o CT_REAL,
els seus valors són iguals i 2) en cas de ser VARIABLE, tenen el mateix
nom. */
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

// Pre: existeix un token a
// POST: Retorna true si i només si a és un operador, altrament retorna false
// Aquesta funció té cost O(1)
bool token::isOperator(const token &a)
{
    switch (a.tipus())
    {
    case SUMA:
        return true;
        break;
    case RESTA:
        return true;
        break;
    case MULTIPLICACIO:
        return true;
        break;
    case DIVISIO:
        return true;
        break;
    case EXPONENCIACIO:
        return true;
        break;
    case CANVI_DE_SIGNE:
        return true;
        break;
    case SIGNE_POSITIU:
        return true;
        break;
    default:
        return false;
    };
}
/*Precedència entre tokens. L'operador > retorna cert si i només si el token
és un operador amb major precedència que l'operador del token t. Si algun
dels tokens no és un operador es produeix un error.*/
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
