#include "math_sessio.hpp"
#include <algorithm>
using namespace std;

math_sessio::math_sessio() throw(error)
{
    ended = false;
    exp = expressio();
    variable.assign("%", exp);
}

math_sessio::math_sessio(const math_sessio &es) throw(error) : variable(es.variable), exp(es.exp), ended(es.ended) {}
math_sessio &math_sessio::operator=(const math_sessio &es) throw(error)
{
    variable = es.variable;
    exp = es.exp;
    ended = es.ended;
    return *this;
}
math_sessio::~math_sessio() throw(error)
{
}

void math_sessio::execute(const list<token> &lin, list<token> &lout) throw(error)
{
    teError(lin);
    // Analitza parcialment lin per verificar si la comanda és correcta.analisi_comanda(lin);
    //  Final de sessió: byebye.
    if (lin.front().tipus() == token::BYEBYE)
    {
        ended = true;
        lout = list<token>();
    }
    // Desassignació d'una variable v: unassign v.
    else if (lin.front().tipus() == token::DESASSIGNACIO)
    {
        variable.unassign(lin.back().identificador_variable());
        // Retorna la llista que conté com únic token la variable desassignada.
        lout.push_back(lin.back());
    }
    // Assignació v :=  E. S'avalua E i s'assigna el resultat a la variable de nom v.
    else if ((*next(lin.begin(), 1)).tipus() == token::ASSIGNACIO)
    {
        // Creem una llista de tokens amb l'expressió a assignar.
        list<token> lt_exp;
        auto id = lin.begin();
        auto it = id;
        advance(it, 2);
        for (; it != lin.end(); ++it)
        {
            lt_exp.push_back(*it);
        }
        exp = expressio(lt_exp);
        // Simplifiquem l'expressió i apliquem totes les substitucions.
        exp.simplify();
        apply_all_substitutions(exp);
        exp.simplify();
        list<string> ltvariable;
        exp.vars(ltvariable);
        for (list<string>::iterator it = ltvariable.begin(); it != ltvariable.end(); ++it)
            if ((*it) == (*id).identificador_variable())
                // Si la variable és una expressió que conté la mateixa variable, es llença l'error "Assignació amb circularitat infinita".
                throw error(AssigAmbCirculInfinita);
        // Assignem el valor de l'expressió simplificada a la variable.
        variable.assign("%", exp);
        variable.assign((*id).identificador_variable(), exp);
        exp.list_of_tokens(lout);
    }
    // Avaluació d'una expressió E.
    else
    {
        // Creem una expressió a partir d'una llista de tokens.
        exp = expressio(lin);
        // Simplifiquem l'expressió i apliquem totes les substitucions.
        apply_all_substitutions(exp);
        exp.simplify();
        variable.assign("%", exp);
        exp.list_of_tokens(lout);
    }
}

void math_sessio::teError(const list<token> &l) throw(error)
{
    for (list<token>::const_iterator it = l.begin(); it != l.end(); ++it)
    {
        // Conté el token DESASSIGNACIO i...
        if (*it == token(token::DESASSIGNACIO))
        {
            // ...la comanda no té dos tokens.
            if (l.size() != 2)
                throw error(SintComandaIncorrecta);
            // ...aquest no és el primer token.
            else if (l.front().tipus() != token::DESASSIGNACIO)
                throw error(SintComandaIncorrecta);
            // ...el segon token no és una VARIABLE.
            else if (not segonToken(l, token::VARIABLE))
                throw error(SintComandaIncorrecta);
        }
        // Conté el token BYEBYE...
        else if (*it == token(token::BYEBYE))
        {
            // ...i aquest no és el primer.
            if (l.front().tipus() != token::BYEBYE)
                throw error(SintComandaIncorrecta);
            // ...no és l'únic token de la comanda.
            else if (l.size() != 1)
                throw error(SintComandaIncorrecta);
        }
        // Conté el token ASSIGNACIO...
        else if (*it == token(token::ASSIGNACIO))
        {
            // ...la comanda té longitud menor que dos.
            if (l.size() <= 2)
                throw error(SintComandaIncorrecta);
            // ...no és el segon token.
            else if (not segonToken(l, token::ASSIGNACIO))
                throw error(SintComandaIncorrecta);
            // ...el primer token no és un token VARIABLE.
            else if (l.front().tipus() != token::VARIABLE)
                throw error(SintComandaIncorrecta);
        }
    }
}

bool math_sessio::segonToken(const list<token> &l, const token::codi &c)
{
    auto it = std::next(l.begin());
    return it != l.end() and (*it).tipus() == c;
}

bool math_sessio::end_of_session() const throw()
{
    return ended;
}

void math_sessio::dump(list<string> &l) const throw(error)
{
    // Crea una llista per emmagatzemar els noms de les variables
    list<string> nom_variables;
    // Afegeix tots els noms de les variables a la llista
    variable.dump(nom_variables);
    // Itera a través de cada nom de variable en la llista
    for (const string &nom : nom_variables)
    {
        // Obté l'expressió associada a la variable
        expressio ex = variable.valor(nom);
        // Crea una llista per emmagatzemar els tokens de l'expressió
        list<token> lot;
        // Afegeix els tokens de l'expressió a la llista
        ex.list_of_tokens(lot);
        // Converteix la llista de tokens a una cadena de caràcters
        string ex_str = math_io::tostring(lot);
        // Si el nom de la variable no és buit, afegeix el nom i l'expressió a la llista final
        if (nom != "")
            l.push_back(nom + " = " + ex_str);
    }
}

void math_sessio::apply_all_substitutions(expressio &e) const throw(error)
{
    // Crea una llista per emmagatzemar les variables que encara necessiten aplicar substitucions.
    list<string> vars_a_substituir;
    // Afegeix totes les variables en l'expressió a la llista.
    e.vars(vars_a_substituir);

    // Mentre encara hi hagi variables en la llista, aplica substitucions.
    while (!vars_a_substituir.empty())
    {
        // Obté la següent variable de la llista.
        string v = vars_a_substituir.front();
        vars_a_substituir.pop_front();

        // Comprova si la variable té una substitució definida.
        if (variable.valor(v) != expressio())
        {
            // Si ho fa, substitueix la variable i afegeix les variables resultant a la llista.
            e.apply_substitution(v, variable.valor(v));
            e.vars(vars_a_substituir);
        }
    }
}
