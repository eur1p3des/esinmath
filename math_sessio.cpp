#include "math_sessio.hpp"
#include <algorithm>
using namespace std;

/* Constructora. Crea una nova sessió buida i emmagatzema a la variable
    especial % l'expressió buida. */
math_sessio::math_sessio() throw(error)
{
    ended = false;
    exp = expressio();
    variable.assign("%", exp);
}

// Constructora per còpia, assignació i destructora.
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

/* Aquest mètode rep una llista de tokens, lin, lèxicament correcta.
    Primerament analitza parcialment lin per verificar si la comanda és
    correcta. Si és correcta executa la comanda que conté lin.
    Les comandes són:
    * avaluació d'una expressió E.
    * assignació v :=  E. S'avalua E i s'assigna el resultat a la variable
    de nom v.
    * desassignació d'una variable v: unassign v.
    * final de sessió: byebye

    En l'anàlisi de la comanda lin es produeix un error de comanda
    incorrecta en els següents casos:
    * si conté el token DESASSIGNACIO i,
        * o bé la comanda no té dos tokens
        * o bé aquest no és el primer token
        * o bé el segon token no és una VARIABLE.
    * si conté el token BYEBYE i aquest no és el primer i únic token
    de la comanda.
    * si conté el token ASSIGNACIO i,
        * o bé la comanda té longitud menor que dos
        * o bé no és el segon token
        * o bé el primer token no és un token VARIABLE.

    Les comandes que involucren avaluació (avaluar i assignar) retornen
    l'expressió avaluada en forma de llista de tokens en lout. La comanda
    unassign retorna la llista que conté com únic token la variable
    desassignada. Finalment la comanda byebye retorna la llista buida.
    Es produeix un error en una assignació, si després l'avaluació de
    l'expressió es comprova que la variable assignada forma part del conjunt
    de variables de l'expressió avaluada, tal i com s'explica a
    l'apartat "Procés d'avaluació". */
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

// COST: O(n), perquè utilitza un iterador per recórrer la llista de tokens "lin" i comprova cada token.
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

// COST: O(1), ja que només es fa una única iteració sobre una llista.
bool math_sessio::segonToken(const list<token> &l, const token::codi &c)
{
    auto it = std::next(l.begin());
    return it != l.end() and (*it).tipus() == c;
}
// Retorna cert si i només si la sessió ha finalitzat.
bool math_sessio::end_of_session() const throw()
{
    return ended;
}

/* Retorna en forma de llista d'strings, en un ordre qualsevol, la llista de
    variables juntament amb el seu valor assignat. Cada string de la llista
    té el format id = e, on id és el nom d'una variable i e és l'expressió
    (com string) assignada a id. Convertim una expressió e en el seu string
    corresponent invocant al mètode tostring del mòdul esinmath_io. */
void math_sessio::dump(list<string> &l) const throw(error)
{
    list<string> nom_variables;
    variable.dump(nom_variables);
    for (const string &nom : nom_variables)
    {
        expressio ex = variable.valor(nom);
        list<token> lot;
        ex.list_of_tokens(lot);
        string ex_str = math_io::tostring(lot);
        if (nom != "")
            l.push_back(nom + " = " + ex_str);
    }
}

/* Donada una expressió e, aplica a les seves variables totes les
    substitucions definides per elles. L'expressió resultant només contindrà
    variables per les quals no hi ha substitució definida (no estan en el
    conjunt). Aquest procés s'explica en detall a l'apartat "Procés
    d'avaluació". S'assumeix que no existeix circularitat infinita entre les
    substitucions de les variables que formen part de l'expressió e. */
void math_sessio::apply_all_substitutions(expressio &e) const throw(error)
{
    // Create a list to store the variables that still need to have substitutions applied.
    std::list<string> vars_to_substitute;
    // Add all the variables in the expression to the list.
    e.vars(vars_to_substitute);

    // While there are still variables in the list, apply substitutions.
    while (!vars_to_substitute.empty())
    {
        // Get the next variable in the list.
        string v = vars_to_substitute.front();
        vars_to_substitute.pop_front();

        // Check if the variable has a substitution defined.
        if (variable.valor(v) != expressio())
        {
            // If it does, substitute the variable and add the resulting variables to the list.
            e.apply_substitution(v, variable.valor(v));
            e.vars(vars_to_substitute);
        }
    }
}
