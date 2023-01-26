#include "expressio.hpp"
#include <stack>
#include <cmath>
using namespace std;

expressio::node::node(const token &info, node *f_esq, node *f_dret) : info(info), f_esq(f_esq), f_dret(f_dret)
{
}
expressio::node *expressio::copia_nodes(node *m)
{
  node *n;
  if (m == nullptr)
    n = nullptr;
  else
  {
    n = new node(m->info);
    try
    {
      n->f_esq = copia_nodes(m->f_esq);
      n->f_dret = copia_nodes(m->f_dret);
    }
    catch (const std::exception &)
    {
      delete n;
      throw;
    }
  }
  return n;
}

void expressio::esborra_nodes(node *m)
{
  if (m != nullptr)
  {
    esborra_nodes(m->f_esq);
    esborra_nodes(m->f_dret);
    delete m;
  }
}

expressio::expressio(const token t) throw(error)
{
  switch (t.tipus())
  {
  case token::NULLTOK:
  case token::CT_ENTERA:
  case token::CT_RACIONAL:
  case token::CT_REAL:
  case token::CT_E:
  case token::VARIABLE:
  case token::VAR_PERCENTATGE:
    _arrel = new node(t);
    break;
  default:
    throw error(ErrorSintactic);
  }
}
bool expressio::preferenciaMajor(const token &a, const token &b)
{
  if (isParenthesis(a))
    return true;
  if (isParenthesis(b))
    return false;
  if (isFunction(a) or a.tipus() == token::EXPONENCIACIO)
    return true;
  if (isFunction(b) and (!isFunction(a) and a.tipus() != token::EXPONENCIACIO))
    return false;
  if (a.tipus() == b.tipus())
    return false;
  return a > b;
}
bool expressio::preferenciaMenor(const token &a, const token &b)
{
  if (isParenthesis(a))
    return false;
  if (isParenthesis(b))
    return true;
  if (isFunction(a))
    return false;
  if (isFunction(b) and (!isFunction(a) and a.tipus() != token::EXPONENCIACIO))
    return true;
  if (a.tipus() == b.tipus())
    return false;
  return a < b;
}

bool expressio::associativitatEsquerraDreta(const token &a)
{
  if (isBinary(a) or isUnary(a))
  {
    if (a.tipus() == token::EXPONENCIACIO or isUnary(a))
      return false;
    return true;
  }
  return true;
}

bool expressio::isOperand(const token &a)
{
  switch (a.tipus())
  {
  case token::CT_ENTERA:
  case token::CT_RACIONAL:
  case token::CT_REAL:
  case token::CT_E:
  case token::VARIABLE:
  case token::VAR_PERCENTATGE:
    return true;
    break;
  default:
    return false;
  }
}
bool expressio::isOperator(const token &a)
{
  return isUnary(a) or isBinary(a) or isFunction(a);
}
bool expressio::isUnary(const token &a)
{
  switch (a.tipus())
  {
  case token::CANVI_DE_SIGNE:
  case token::SIGNE_POSITIU:
    return true;
    break;
  default:
    return false;
  }
}
bool expressio::isBinary(const token &a)
{
  switch (a.tipus())
  {
  case token::SUMA:
  case token::RESTA:
  case token::EXPONENCIACIO:
  case token::MULTIPLICACIO:
  case token::DIVISIO:
    return true;
    break;
  default:
    return false;
  }
}
bool expressio::isFunction(const token &a)
{
  switch (a.tipus())
  {
  case token::SQRT:
  case token::EXP:
  case token::LOG:
  case token::EVALF:
    return true;
    break;
  default:
    return false;
  }
}
bool expressio::isParenthesis(const token &a)
{
  return a.tipus() == token::OBRIR_PAR or a.tipus() == token::TANCAR_PAR;
}
bool expressio::provocaError(const token &a, const token &b)
{
  if ((isOperand(a) and isOperand(b)))
    return true;
  if (isOperand(a) and isUnary(b))
    return true;
  if (isOperand(a) and isFunction(b))
    return true;
  if ((isBinary(a) and isBinary(b)))
    return true;
  if ((a.tipus() == token::OBRIR_PAR) and (isBinary(b)))
    return true;
  if ((a.tipus() == token::OBRIR_PAR) and (b.tipus() == token::TANCAR_PAR))
    return true;
  if ((isFunction(a) and !(b.tipus() == token::OBRIR_PAR)))
    return true;
  if (isUnary(a) and isBinary(b))
    return true;
  return false;
}

expressio::expressio(const list<token> &l) throw(error)
{
  if ((int)l.size() == 0)
  {
    throw error(ErrorSintactic);
  }
  stack<token> operadors;
  stack<node *> expresio;
  for (auto it = l.begin(); it != l.end(); ++it)
  {
    token t = *it;
    auto it2 = it;
    it2++;
    if (it2 != l.end())
    {
      token tnext = *it2;
      if (provocaError(t, tnext))
      {
        while (!expresio.empty())
        {
          node *n = expresio.top();
          expresio.pop();
          esborra_nodes(n);
        }
        throw error(ErrorSintactic);
      }
    }
    if (isOperand(t))
    {
      expresio.push(new node(t));
    }
    else if (t.tipus() == token::OBRIR_PAR)
      operadors.push(t);
    else if (t.tipus() == token::TANCAR_PAR)
    {
      while (not(operadors.top().tipus() == token::OBRIR_PAR))
      {
        if (isBinary(operadors.top()))
        {
          if (expresio.size() == 1)
          {
            while (!expresio.empty())
            {
              node *n = expresio.top();
              expresio.pop();
              esborra_nodes(n);
            }
            throw error(ErrorSintactic);
          }
          node *p = new node(operadors.top());
          p->f_dret = expresio.top();
          expresio.pop();
          p->f_esq = expresio.top();
          expresio.pop();
          expresio.push(p);
        }
        else if (isUnary(operadors.top()) or isFunction(operadors.top()))
        {
          node *p = new node(operadors.top());
          p->f_dret = expresio.top();
          expresio.pop();
          p->f_esq = nullptr;
          expresio.push(p);
        }
        operadors.pop();
      }
      operadors.pop();
    }
    else if (isOperator(t))
    {
      while ((not operadors.empty()) and (preferenciaMenor(t, operadors.top()) or !preferenciaMajor(t, operadors.top())) and (associativitatEsquerraDreta(t)) and (not isParenthesis(operadors.top())))
      {
        if (isBinary(operadors.top()))
        {
          if (expresio.size() > 1)
          {
            node *p = new node(operadors.top());
            p->f_dret = expresio.top();
            expresio.pop();
            p->f_esq = expresio.top();
            expresio.pop();
            expresio.push(p);
          }
          else
            break;
        }
        else if (isUnary(operadors.top()) or isFunction(operadors.top()))
        {
          node *p = new node(operadors.top());
          p->f_dret = expresio.top();
          expresio.pop();
          p->f_esq = nullptr;
          expresio.push(p);
        }
        operadors.pop();
      }
      operadors.push(t);
    }
  }

  while (not operadors.empty())
  {
    if (isParenthesis(operadors.top()))
    {
      while (!expresio.empty())
      {
        node *n = expresio.top();
        expresio.pop();
        esborra_nodes(n);
      }
      throw error(ErrorSintactic);
    }
    if (isBinary(operadors.top()))
    {
      if (expresio.size() == 1)
      {
        while (!expresio.empty())
        {
          node *n = expresio.top();
          expresio.pop();
          esborra_nodes(n);
        }
        throw error(ErrorSintactic);
      }
      node *p = new node(operadors.top());
      p->f_dret = expresio.top();
      expresio.pop();
      p->f_esq = expresio.top();
      expresio.pop();
      expresio.push(p);
    }
    else if (isUnary(operadors.top()) or isFunction(operadors.top()))
    {
      node *p = new node(operadors.top());
      p->f_dret = expresio.top();
      expresio.pop();
      p->f_esq = nullptr;
      expresio.push(p);
    }
    operadors.pop();
  }
  if (expresio.size() != 1)
    throw error(ErrorSintactic);
  _arrel = expresio.top();
}

expressio::expressio(const expressio &e) throw(error)
{
  _arrel = copia_nodes(e._arrel);
}
expressio &expressio::operator=(const expressio &e) throw(error)
{
  esborra_nodes(_arrel);
  _arrel = copia_nodes(e._arrel);
  return *this;
}
expressio::~expressio() throw(error)
{
  esborra_nodes(_arrel);
}

expressio::operator bool() const throw()
{
  return _arrel->info.tipus() == token::NULLTOK;
}

bool expressio::operator==(const expressio &e) const throw()
{
  return iguals(_arrel, e._arrel);
}

bool expressio::operator!=(const expressio &e) const throw()
{
  return not(*this == e);
}

bool expressio::iguals(node *a, node *b)
{
  // Si ambdós arbres són nuls, llavors són iguals
  if (a == nullptr and b == nullptr)
    return true;
  // Si només un dels arbres és nul, llavors no són iguals
  if (a == nullptr or b == nullptr)
    return false;
  // Si els valors dels nodes no són iguals, llavors els arbres no són iguals
  if (a->info != b->info)
    return false;
  // Compara recursivament els fills esquerre i dret de tots dos nodes
  return iguals(a->f_esq, b->f_esq) and iguals(a->f_dret, b->f_dret);
}

void expressio::vars(list<string> &l) const throw(error)
{
  recorre(_arrel, l);
}
void expressio::recorre(node *n, list<string> &l)
{
  if (n != nullptr)
  {
    recorre(n->f_esq, l);
    token t = n->info;
    if (t.tipus() == token::VAR_PERCENTATGE)
    {
      bool trobat = false;
      auto it = l.begin();
      while (it != l.end() and not trobat)
      {
        if ("%" == *it)
          trobat = true;
        it++;
      }
      if (not trobat)
      {
        l.push_back("%");
      }
    }
    else if (t.tipus() == token::VARIABLE)
    {
      bool trobat = false;
      auto it = l.begin();
      while (it != l.end() and not trobat)
      {
        if (t.identificador_variable() == *it)
          trobat = true;
        it++;
      }
      if (not trobat)
      {
        l.push_back(t.identificador_variable());
      }
    }
    recorre(n->f_dret, l);
  }
}

void expressio::apply_substitution(const string &v, const expressio &e) throw(error)
{
  if (_arrel->info.tipus() == token::VARIABLE)
  {
    token a = _arrel->info;
    if (a.identificador_variable() == v or v == "%")
    {
      node *aux = _arrel;
      _arrel = copia_nodes(e._arrel);
      delete aux;
    }
  }
  else if (v == "%" and _arrel->info.tipus() == token::VAR_PERCENTATGE)
  {
    node *aux = _arrel;
    _arrel = copia_nodes(e._arrel);
    delete aux;
  }
  else
    apply_substitution(_arrel, v, e._arrel);
}
void expressio::apply_substitution(node *n, const string &v, node *e)
{
  if (n != nullptr)
  {
    // Sustitueix recursivament els fills esquerre i dret del node actual
    apply_substitution(n->f_esq, v, e);
    apply_substitution(n->f_dret, v, e);
    // Si el fill esquerre no és nul, compara el seu valor amb la variable v
    if (n->f_esq != nullptr)
    {
      token a = n->f_esq->info;
      if ((v == "%" and a.tipus() == token::VAR_PERCENTATGE) or (a.tipus() == token::VARIABLE and a.identificador_variable() == v))
      {
        // Si coincideix, crea una còpia del subarbre e i l'assigna com a fill esquerre del node actual
        node *auxiliar = n->f_esq;
        n->f_esq = copia_nodes(e);
        delete auxiliar; // Esborra el node original
      }
    }
    // Si el fill dret no és nul, compara el seu valor amb la variable v
    if (n->f_dret != nullptr)
    {
      token a = n->f_dret->info;
      if ((v == "%" and a.tipus() == token::VAR_PERCENTATGE) or (a.tipus() == token::VARIABLE and a.identificador_variable() == v))
      {
        // Si coincideix, crea una còpia del subarbre e i l'assigna com a fill dret del node actual
        node *auxiliar = n->f_dret;
        n->f_dret = copia_nodes(e);
        delete auxiliar; // Esborra el node original
      }
    }
  }
}

expressio::node *expressio::simplifySuma(node *n, bool &trobat) throw(error)
{
  if (isOperand(n->f_esq->info) and isOperand(n->f_dret->info) and n->f_esq->info.tipus() != token::VARIABLE and n->f_dret->info.tipus() != token::VARIABLE)
  {

    if (n->f_esq->info.tipus() == n->f_dret->info.tipus())
    {
      if (n->f_esq->info.tipus() == token::CT_ENTERA)
      {
        int res = n->f_esq->info.valor_enter() + n->f_dret->info.valor_enter();
        n->info = token(res);
        delete n->f_esq;
        delete n->f_dret;
        n->f_dret = nullptr;
        n->f_esq = nullptr;
      }
      else if (n->f_esq->info.tipus() == token::CT_REAL)
      {
        double res = n->f_esq->info.valor_real() + n->f_dret->info.valor_real();
        n->info = token(res);
        delete n->f_esq;
        delete n->f_dret;
        n->f_dret = nullptr;
        n->f_esq = nullptr;
      }
      else if (n->f_esq->info.tipus() == token::CT_RACIONAL)
      {
        racional res = n->f_esq->info.valor_racional() + n->f_dret->info.valor_racional();
        n->info = token(res);
        delete n->f_esq;
        delete n->f_dret;
        n->f_dret = nullptr;
        n->f_esq = nullptr;
      }
      trobat = true;
      return n;
    }
    else if (n->f_esq->info.tipus() == token::CT_ENTERA and n->f_dret->info.tipus() == token::CT_RACIONAL)
    {
      racional a(n->f_esq->info.valor_enter()), res = a + n->f_dret->info.valor_racional();
      n->info = token(res);
      delete n->f_esq;
      delete n->f_dret;
      n->f_dret = nullptr;
      n->f_esq = nullptr;
      trobat = true;
      return n;
    }
    else if (n->f_dret->info.tipus() == token::CT_ENTERA and n->f_esq->info.tipus() == token::CT_RACIONAL)
    {
      racional a(n->f_dret->info.valor_enter()), res = a + n->f_esq->info.valor_racional();
      n->info = token(res);
      delete n->f_esq;
      delete n->f_dret;
      n->f_dret = nullptr;
      n->f_esq = nullptr;
      trobat = true;
      return n;
    }
  }
  else if (n->f_esq->info.tipus() == token::CT_ENTERA and n->f_esq->info.valor_enter() == 0)
  {
    node *aux = n->f_dret;
    n->info = aux->info;
    delete n->f_esq;
    n->f_esq = aux->f_esq;
    n->f_dret = aux->f_dret;
    delete aux;
    trobat = true;
    return n;
  }
  else if (n->f_dret->info.tipus() == token::CT_ENTERA and n->f_dret->info.valor_enter() == 0)
  {
    node *aux = n->f_esq;
    n->info = aux->info;
    n->f_esq = aux->f_esq;
    delete n->f_dret;
    n->f_dret = aux->f_dret;
    delete aux;
    trobat = true;
    return n;
  }
  else if (iguals(n->f_esq, n->f_dret))
  {
    n->info = token(token::MULTIPLICACIO);
    n->f_esq->info = token(2);
    delete n->f_esq->f_esq;
    delete n->f_esq->f_dret;
    n->f_esq->f_esq = nullptr;
    n->f_esq->f_dret = nullptr;
    trobat = true;
    return n;
  }
  else if (n->f_dret->info.tipus() == token::CANVI_DE_SIGNE)
  {
    node *aux = n->f_dret;
    n->info = token(token::RESTA);
    n->f_dret = aux->f_dret;
    delete aux;
    trobat = true;
    return n;
  }
  else if (n->f_esq->info.tipus() == token::CANVI_DE_SIGNE)
  {
    node *aux = n->f_esq;
    n->info = token(token::RESTA);
    delete n->f_esq->f_esq;
    n->f_esq = aux->f_dret;
    delete aux;
    trobat = true;
    return n;
  }
  else if (n->f_esq->info.tipus() == token::MULTIPLICACIO and n->f_dret->info.tipus() == token::MULTIPLICACIO)
  {
    if (iguals(n->f_esq->f_dret, n->f_dret->f_dret))
    {
      node *aux = n->f_dret;
      n->info = token(token::MULTIPLICACIO);
      n->f_esq->info = token(token::SUMA);
      delete n->f_esq->f_dret;
      n->f_esq->f_dret = n->f_dret->f_esq;
      n->f_dret = n->f_dret->f_dret;
      delete aux;
      trobat = true;
      return n;
    }
    else if (iguals(n->f_esq->f_dret, n->f_dret->f_esq))
    {
      node *aux = n->f_dret;
      n->info = token(token::MULTIPLICACIO);
      n->f_esq->info = token(token::SUMA);
      delete n->f_esq->f_dret;
      n->f_esq->f_dret = n->f_dret->f_dret;
      n->f_dret = n->f_dret->f_esq;
      delete aux;
      trobat = true;
      return n;
    }
    else if (iguals(n->f_esq->f_esq, n->f_dret->f_dret))
    {
      node *aux = n->f_dret;
      n->info = token(token::MULTIPLICACIO);
      n->f_esq->info = token(token::SUMA);
      delete n->f_esq->f_esq;
      n->f_esq->f_esq = n->f_esq->f_dret;
      n->f_esq->f_dret = n->f_dret->f_esq;
      n->f_dret = n->f_dret->f_dret;
      delete aux;
      trobat = true;
      return n;
    }
    else if (iguals(n->f_esq->f_esq, n->f_dret->f_esq))
    {
      node *aux = n->f_dret;
      n->info = token(token::MULTIPLICACIO);
      n->f_esq->info = token(token::SUMA);
      delete n->f_esq->f_esq;
      n->f_esq->f_esq = n->f_esq->f_dret;
      n->f_esq->f_dret = n->f_dret->f_dret;
      n->f_dret = n->f_dret->f_esq;
      delete aux;
      trobat = true;
      return n;
    }
  }
  else if (n->f_esq->info.tipus() == token::DIVISIO and n->f_dret->info.tipus() == token::DIVISIO)
  {
    if (iguals(n->f_esq->f_dret, n->f_dret->f_dret))
    {
      node *aux = n->f_dret;
      n->info = token(token::DIVISIO);
      n->f_esq->info = token(token::SUMA);
      delete n->f_esq->f_dret;
      n->f_esq->f_dret = n->f_dret->f_esq;
      n->f_dret = n->f_dret->f_dret;
      delete aux;
      trobat = true;
      return n;
    }
  }
  return n;
}
expressio::node *expressio::simplifyResta(node *n, bool &trobat) throw(error)
{
  if (isOperand(n->f_esq->info) and isOperand(n->f_dret->info) and n->f_esq->info.tipus() != token::VARIABLE and n->f_dret->info.tipus() != token::VARIABLE)
  {
    if (n->f_esq->info.tipus() == n->f_dret->info.tipus() and n->f_esq->info.tipus() != token::VARIABLE)
    {
      if (n->f_esq->info.tipus() == token::CT_ENTERA)
      {
        int res = n->f_esq->info.valor_enter() - n->f_dret->info.valor_enter();
        n->info = token(res);
        delete n->f_esq;
        delete n->f_dret;
        n->f_dret = nullptr;
        n->f_esq = nullptr;
      }
      else if (n->f_esq->info.tipus() == token::CT_REAL)
      {
        double res = n->f_esq->info.valor_real() - n->f_dret->info.valor_real();
        n->info = token(res);
        delete n->f_esq;
        delete n->f_dret;
        n->f_dret = nullptr;
        n->f_esq = nullptr;
      }
      else if (n->f_esq->info.tipus() == token::CT_RACIONAL)
      {
        racional res = n->f_esq->info.valor_racional() - n->f_dret->info.valor_racional();
        n->info = token(res);
        delete n->f_esq;
        delete n->f_dret;
        n->f_dret = nullptr;
        n->f_esq = nullptr;
      }
      trobat = true;
      return n;
    }
    else if (n->f_esq->info.tipus() == token::CT_ENTERA and n->f_dret->info.tipus() == token::CT_RACIONAL)
    {
      racional a(n->f_esq->info.valor_enter()), res = a - n->f_dret->info.valor_racional();
      n->info = token(res);
      delete n->f_esq;
      delete n->f_dret;
      n->f_dret = nullptr;
      n->f_esq = nullptr;
      trobat = true;
      return n;
    }
    else if (n->f_dret->info.tipus() == token::CT_ENTERA and n->f_esq->info.tipus() == token::CT_RACIONAL)
    {
      racional a(n->f_dret->info.valor_enter()), res = n->f_esq->info.valor_racional() - a;
      n->info = token(res);
      delete n->f_esq;
      delete n->f_dret;
      n->f_dret = nullptr;
      n->f_esq = nullptr;
      trobat = true;
      return n;
    }
  }
  else if (n->f_esq->info.tipus() == token::CT_ENTERA and n->f_esq->info.valor_enter() == 0)
  {
    n->info = token(token::CANVI_DE_SIGNE);
    delete n->f_esq;
    n->f_esq = nullptr;
    trobat = true;
    return n;
  }
  else if (n->f_dret->info.tipus() == token::CT_ENTERA and n->f_dret->info.valor_enter() == 0)
  {
    node *aux = n->f_esq;
    n->info = n->f_esq->info;
    n->f_esq = aux->f_esq;
    delete n->f_dret;
    n->f_dret = aux->f_dret;
    delete aux;
    trobat = true;
    return n;
  }
  else if (iguals(n->f_esq, n->f_dret))
  {
    esborra_nodes(n->f_esq);
    esborra_nodes(n->f_dret);
    n->info = token(0);
    n->f_dret = nullptr;
    n->f_esq = nullptr;
    trobat = true;
    return n;
  }
  else if (n->f_dret->info.tipus() == token::CANVI_DE_SIGNE)
  {
    node *aux = n->f_dret;
    n->info = token(token::SUMA);
    n->f_dret = aux->f_dret;
    delete aux;
    trobat = true;
    return n;
  }
  else if (n->f_esq->info.tipus() == token::MULTIPLICACIO and n->f_dret->info.tipus() == token::MULTIPLICACIO)
  {
    if (iguals(n->f_esq->f_dret, n->f_dret->f_dret))
    {
      node *aux = n->f_dret;
      n->info = token(token::MULTIPLICACIO);
      n->f_esq->info = token(token::RESTA);
      delete n->f_esq->f_dret;
      n->f_esq->f_dret = n->f_dret->f_esq;
      n->f_dret = n->f_dret->f_dret;
      delete aux;
      trobat = true;
      return n;
    }
    else if (iguals(n->f_esq->f_dret, n->f_dret->f_esq))
    {
      node *aux = n->f_dret;
      n->info = token(token::MULTIPLICACIO);
      n->f_esq->info = token(token::RESTA);
      delete n->f_esq->f_dret;
      n->f_esq->f_dret = n->f_dret->f_dret;
      n->f_dret = n->f_dret->f_esq;
      delete aux;
      trobat = true;
      return n;
    }
    else if (iguals(n->f_esq->f_esq, n->f_dret->f_dret))
    {
      node *aux = n->f_dret;
      n->info = token(token::MULTIPLICACIO);
      n->f_esq->info = token(token::RESTA);
      delete n->f_esq->f_esq;
      n->f_esq->f_esq = n->f_esq->f_dret;
      n->f_esq->f_dret = n->f_dret->f_esq;
      n->f_dret = n->f_dret->f_dret;
      delete aux;
      trobat = true;
      return n;
    }
    else if (iguals(n->f_esq->f_esq, n->f_dret->f_esq))
    {
      node *aux = n->f_dret;
      n->info = token(token::MULTIPLICACIO);
      n->f_esq->info = token(token::RESTA);
      delete n->f_esq->f_esq;
      n->f_esq->f_esq = n->f_esq->f_dret;
      n->f_esq->f_dret = n->f_dret->f_dret;
      n->f_dret = n->f_dret->f_esq;
      delete aux;
      trobat = true;
      return n;
    }
  }
  else if (n->f_esq->info.tipus() == token::DIVISIO and n->f_dret->info.tipus() == token::DIVISIO)
  {
    if (iguals(n->f_esq->f_dret, n->f_dret->f_dret))
    {
      node *aux = n->f_dret;
      n->info = token(token::DIVISIO);
      n->f_esq->info = token(token::RESTA);
      delete n->f_esq->f_dret;
      n->f_esq->f_dret = n->f_dret->f_esq;
      n->f_dret = n->f_dret->f_dret;
      delete aux;
      trobat = true;
      return n;
    }
  }
  return n;
}
expressio::node *expressio::simplifyCanviSigne(node *n, bool &trobat) throw(error)
{
  if (isOperand(n->f_dret->info) and n->f_dret->info.tipus() != token::VARIABLE)
  {
    if (n->f_dret->info.tipus() == token::CT_ENTERA)
    {
      int res = n->f_dret->info.valor_enter() * (-1);
      n->info = token(res);
      delete n->f_esq;
      delete n->f_dret;
      n->f_dret = nullptr;
      n->f_esq = nullptr;
      trobat = true;
      return n;
    }
    else if (n->f_dret->info.tipus() == token::CT_REAL)
    {
      double res = n->f_dret->info.valor_real() * double(-1);
      n->info = token(res);
      delete n->f_esq;
      delete n->f_dret;
      n->f_dret = nullptr;
      n->f_esq = nullptr;
      trobat = true;
      return n;
    }
    else if (n->f_dret->info.tipus() == token::CT_RACIONAL)
    {
      racional res = n->f_dret->info.valor_racional() * racional(-1);
      n->info = token(res);
      delete n->f_esq;
      delete n->f_dret;
      n->f_dret = nullptr;
      n->f_esq = nullptr;
      trobat = true;
      return n;
    }
  }
  else if (n->f_dret->info.tipus() == token::CANVI_DE_SIGNE)
  {
    node *aux = n->f_dret, *aux2 = n->f_dret->f_dret;
    n->info = aux->f_dret->info;
    n->f_dret = aux->f_dret->f_dret;
    delete n->f_esq;
    n->f_esq = aux->f_dret->f_esq;
    delete aux;
    delete aux2;
    trobat = true;
    return n;
  }
  return n;
}
expressio::node *expressio::simplifyMultiplicacio(node *n, bool &trobat) throw(error)
{
  if (isOperand(n->f_esq->info) and isOperand(n->f_dret->info) and n->f_esq->info.tipus() != token::VARIABLE and n->f_dret->info.tipus() != token::VARIABLE)
  {
    if (n->f_esq->info.tipus() == n->f_dret->info.tipus() and n->f_esq->info.tipus() != token::VARIABLE)
    {
      if (n->f_esq->info.tipus() == token::CT_ENTERA)
      {
        int res = n->f_esq->info.valor_enter() * n->f_dret->info.valor_enter();
        n->info = token(res);
        delete n->f_esq;
        delete n->f_dret;
        n->f_dret = nullptr;
        n->f_esq = nullptr;
      }
      else if (n->f_esq->info.tipus() == token::CT_REAL)
      {
        double res = n->f_esq->info.valor_real() * n->f_dret->info.valor_real();
        n->info = token(res);
        delete n->f_esq;
        delete n->f_dret;
        n->f_dret = nullptr;
        n->f_esq = nullptr;
      }
      else if (n->f_esq->info.tipus() == token::CT_RACIONAL)
      {
        racional res = n->f_esq->info.valor_racional() * n->f_dret->info.valor_racional();
        n->info = token(res);
        delete n->f_esq;
        delete n->f_dret;
        n->f_dret = nullptr;
        n->f_esq = nullptr;
      }
      trobat = true;
      return n;
    }
    else if (n->f_esq->info.tipus() == token::CT_ENTERA and n->f_dret->info.tipus() == token::CT_RACIONAL)
    {
      racional a(n->f_esq->info.valor_enter()), res = a * n->f_dret->info.valor_racional();
      n->info = token(res);
      delete n->f_esq;
      delete n->f_dret;
      n->f_dret = nullptr;
      n->f_esq = nullptr;
      trobat = true;
      return n;
    }
    else if (n->f_dret->info.tipus() == token::CT_ENTERA and n->f_esq->info.tipus() == token::CT_RACIONAL)
    {
      racional a(n->f_dret->info.valor_enter()), res = n->f_esq->info.valor_racional() * a;
      n->info = token(res);
      delete n->f_esq;
      delete n->f_dret;
      n->f_dret = nullptr;
      n->f_esq = nullptr;
      trobat = true;
      return n;
    }
  }
  else if (n->f_esq->info.tipus() == token::CT_ENTERA)
  {
    if (n->f_esq->info.valor_enter() == 0)
    {
      n->info = token(0);
      delete n->f_esq;
      delete n->f_dret;
      n->f_dret = nullptr;
      n->f_esq = nullptr;
      trobat = true;
      return n;
    }
    else if (n->f_esq->info.valor_enter() == 1)
    {
      node *aux = n->f_dret;
      n->info = n->f_dret->info;
      delete n->f_esq;
      n->f_esq = aux->f_esq;
      n->f_dret = aux->f_dret;
      delete aux;
      trobat = true;
      return n;
    }
  }
  else if (n->f_dret->info.tipus() == token::CT_ENTERA)
  {
    if (n->f_dret->info.valor_enter() == 0)
    {
      n->info = token(0);
      delete n->f_esq;
      delete n->f_dret;
      n->f_dret = nullptr;
      n->f_esq = nullptr;
      trobat = true;
      return n;
    }
    else if (n->f_dret->info.valor_enter() == 1)
    {
      node *aux = n->f_esq;
      n->info = n->f_esq->info;
      n->f_esq = aux->f_esq;
      delete n->f_dret;
      n->f_dret = aux->f_dret;
      delete aux;
      trobat = true;
      return n;
    }
  }
  else if (iguals(n->f_esq, n->f_dret))
  {
    n->info = token(token::EXPONENCIACIO);
    esborra_nodes(n->f_dret->f_dret);
    esborra_nodes(n->f_dret->f_esq);
    n->f_dret->info = token(2);
    n->f_dret->f_esq = nullptr;
    n->f_dret->f_dret = nullptr;
    trobat = true;
    return n;
  }
  else if (n->f_dret->info.tipus() == token::CANVI_DE_SIGNE)
  {
    n->info = token(token::CANVI_DE_SIGNE);
    n->f_dret->info = token(token::MULTIPLICACIO);
    n->f_dret->f_esq = n->f_esq;
    n->f_esq = nullptr;
    trobat = true;
    return n;
  }
  else if (n->f_esq->info.tipus() == token::CANVI_DE_SIGNE)
  {
    n->info = token(token::CANVI_DE_SIGNE);
    node *aux = n->f_esq, *auxd = n->f_dret;
    n->f_dret = new node(token(token::MULTIPLICACIO), aux->f_dret, auxd);
    n->f_esq = nullptr;
    delete aux;
    trobat = true;
    return n;
  }
  else if (n->f_dret->info.tipus() == token::DIVISIO)
  {
    if (n->f_dret->f_esq->info.tipus() == token::CT_ENTERA and n->f_dret->f_esq->info.valor_enter() == 1)
    {
      node *aux = n->f_dret, *auxe = n->f_dret->f_esq;
      n->info = token(token::DIVISIO);
      n->f_dret = aux->f_dret;
      delete auxe;
      delete aux;
      trobat = true;
      return n;
    }
  }
  else if (n->f_esq->info.tipus() == token::DIVISIO)
  {
    if (n->f_esq->f_esq->info.tipus() == token::CT_ENTERA and n->f_esq->f_esq->info.valor_enter() == 1)
    {
      node *ae = n->f_esq, *aee = n->f_esq->f_esq;
      n->info = token(token::DIVISIO);
      n->f_esq = n->f_dret;
      n->f_dret = ae->f_dret;
      delete aee;
      delete ae;
      trobat = true;
      return n;
    }
  }
  else if (n->f_esq->info.tipus() == token::EXPONENCIACIO and n->f_dret->info.tipus() == token::EXPONENCIACIO)
  {
    if (iguals(n->f_esq->f_dret, n->f_dret->f_dret))
    {
      node *aux = n->f_dret;
      n->info = token(token::EXPONENCIACIO);
      n->f_esq->info = token(token::MULTIPLICACIO);
      delete n->f_esq->f_dret;
      n->f_esq->f_dret = aux->f_esq;
      n->f_dret = aux->f_dret;
      delete aux;
      trobat = true;
      return n;
    }
    else if (iguals(n->f_esq->f_esq, n->f_dret->f_esq))
    {
      node *aux = n->f_esq;
      n->info = token(token::EXPONENCIACIO);
      n->f_dret->info = token(token::SUMA);
      delete n->f_dret->f_esq;
      n->f_dret->f_esq = aux->f_dret;
      n->f_esq = aux->f_esq;
      delete aux;
      trobat = true;
      return n;
    }
  }
  else if (n->f_esq->info.tipus() == token::EXP and n->f_dret->info.tipus() == token::EXP)
  {
    node *aux = n->f_esq;
    n->info = token(token::EXP);
    n->f_dret->info = token(token::SUMA);
    delete n->f_dret->f_esq;
    n->f_dret->f_esq = aux->f_dret;
    n->f_esq = nullptr;
    delete aux;
    trobat = true;
    return n;
  }
  return n;
}
expressio::node *expressio::simplifyDivisio(node *n, bool &trobat) throw(error)
{
  if (n->f_dret->info.tipus() == token::CT_ENTERA and n->f_dret->info.valor_enter() == 0)
    throw error(DivPerZero);
  else if (isOperand(n->f_esq->info) and isOperand(n->f_dret->info) and n->f_esq->info.tipus() != token::VARIABLE and n->f_dret->info.tipus() != token::VARIABLE)
  {
    if (n->f_esq->info.tipus() == n->f_dret->info.tipus() and n->f_esq->info.tipus() != token::VARIABLE)
    {
      if (n->f_esq->info.tipus() == token::CT_ENTERA)
      {
        if (n->f_esq->info.valor_enter() % n->f_dret->info.valor_enter() == 0)
        {
          int res = n->f_esq->info.valor_enter() / n->f_dret->info.valor_enter();
          n->info = token(res);
          delete n->f_esq;
          delete n->f_dret;
          n->f_dret = nullptr;
          n->f_esq = nullptr;
          trobat = true;
          return n;
        }
        else
        {
          racional res(n->f_esq->info.valor_enter(), n->f_dret->info.valor_enter());
          n->info = token(res);
          delete n->f_esq;
          delete n->f_dret;
          n->f_dret = nullptr;
          n->f_esq = nullptr;
          trobat = true;
          return n;
        }
      }
      else if (n->f_esq->info.tipus() == token::CT_REAL)
      {
        double res = n->f_esq->info.valor_real() / n->f_dret->info.valor_real();
        n->info = token(res);
        delete n->f_esq;
        delete n->f_dret;
        n->f_dret = nullptr;
        n->f_esq = nullptr;
        trobat = true;
        return n;
      }
      else if (n->f_esq->info.tipus() == token::CT_RACIONAL)
      {
        racional res = n->f_esq->info.valor_racional() / n->f_dret->info.valor_racional();
        n->info = token(res);
        delete n->f_esq;
        delete n->f_dret;
        n->f_dret = nullptr;
        n->f_esq = nullptr;
        trobat = true;
        return n;
      }
    }
    else if (n->f_esq->info.tipus() == token::CT_ENTERA and n->f_dret->info.tipus() == token::CT_RACIONAL)
    {
      racional a(n->f_esq->info.valor_enter()), res = a / n->f_dret->info.valor_racional();
      n->info = token(res);
      delete n->f_esq;
      delete n->f_dret;
      n->f_dret = nullptr;
      n->f_esq = nullptr;
      trobat = true;
      return n;
    }
    else if (n->f_dret->info.tipus() == token::CT_ENTERA and n->f_esq->info.tipus() == token::CT_RACIONAL)
    {
      racional a(n->f_dret->info.valor_enter()), res = n->f_esq->info.valor_racional() / a;
      n->info = token(res);
      delete n->f_esq;
      delete n->f_dret;
      n->f_dret = nullptr;
      n->f_esq = nullptr;
      trobat = true;
      return n;
    }
  }
  else if (n->f_esq->info.tipus() == token::CT_ENTERA and n->f_esq->info.valor_enter() == 0)
  {
    n->info = token(0);
    delete n->f_esq;
    delete n->f_dret;
    n->f_dret = nullptr;
    n->f_esq = nullptr;
    trobat = true;
    return n;
  }
  else if (n->f_dret->info.tipus() == token::CT_ENTERA and n->f_dret->info.valor_enter() == 1)
  {
    node *aux = n->f_esq;
    n->info = n->f_esq->info;
    n->f_esq = aux->f_esq;
    delete n->f_dret;
    n->f_dret = aux->f_dret;
    delete aux;
    trobat = true;
    return n;
  }
  else if (iguals(n->f_esq, n->f_dret))
  {
    esborra_nodes(n->f_esq);
    esborra_nodes(n->f_dret);
    n->info = token(1);
    n->f_dret = nullptr;
    n->f_esq = nullptr;
    trobat = true;
    return n;
  }
  else if (n->f_dret->info.tipus() == token::CANVI_DE_SIGNE)
  {
    n->info = token(token::CANVI_DE_SIGNE);
    n->f_dret->info = token(token::DIVISIO);
    n->f_dret->f_esq = n->f_esq;
    n->f_esq = nullptr;
    trobat = true;
    return n;
  }
  else if (n->f_dret->info.tipus() == token::DIVISIO)
  {
    if (n->f_dret->f_esq->info.tipus() == token::CT_ENTERA and n->f_dret->f_esq->info.valor_enter() == 1)
    {
      node *aux = n->f_dret, *auxe = n->f_dret->f_esq;
      n->info = token(token::MULTIPLICACIO);
      n->f_dret = aux->f_dret;
      delete auxe;
      delete aux;
      trobat = true;
      return n;
    }
  }
  else if (n->f_esq->info.tipus() == token::EXPONENCIACIO and n->f_dret->info.tipus() == token::EXPONENCIACIO)
  {
    if (iguals(n->f_esq->f_dret, n->f_dret->f_dret))
    {
      node *aux = n->f_dret;
      n->info = token(token::EXPONENCIACIO);
      n->f_esq->info = token(token::DIVISIO);
      delete n->f_esq->f_dret;
      n->f_esq->f_dret = aux->f_esq;
      n->f_dret = aux->f_dret;
      delete aux;
      trobat = true;
      return n;
    }
    else if (iguals(n->f_esq->f_esq, n->f_dret->f_esq))
    {
      node *aux = n->f_esq;
      n->info = token(token::EXPONENCIACIO);
      n->f_dret->info = token(token::RESTA);
      delete n->f_dret->f_esq;
      n->f_dret->f_esq = aux->f_dret;
      n->f_esq = aux->f_esq;
      delete aux;
      trobat = true;
      return n;
    }
  }
  else if (n->f_esq->info.tipus() == token::EXP and n->f_dret->info.tipus() == token::EXP)
  {
    node *aux = n->f_esq;
    n->info = token(token::EXP);
    n->f_dret->info = token(token::RESTA);
    delete n->f_dret->f_esq;
    n->f_dret->f_esq = aux->f_dret;
    n->f_esq = nullptr;
    delete aux;
    trobat = true;
    return n;
  }
  return n;
}
expressio::node *expressio::simplifyExponenciacio(node *n, bool &trobat) throw(error)
{
  if (isOperand(n->f_dret->info) and isOperand(n->f_esq->info) and n->f_dret->info.tipus() != token::VARIABLE and n->f_esq->info.tipus() != token::VARIABLE)
  {
    token c1 = n->f_esq->info, c2 = n->f_dret->info;
    if (c2.tipus() == token::CT_ENTERA)
    {
      if (c2.valor_enter() >= 0)
      {
        if (c1.tipus() == token::CT_RACIONAL)
        {
          int numerador = (int)pow(c1.valor_racional().num(), c2.valor_enter()), denominador = (int)pow(c1.valor_racional().denom(), c2.valor_enter());
          node *ae = n->f_esq, *ad = n->f_dret;
          n->info = token(racional(numerador, denominador));
          n->f_dret = nullptr;
          n->f_esq = nullptr;
          delete ae;
          delete ad;
          trobat = true;
          return n;
        }
        else if (c1.tipus() == token::CT_ENTERA)
        {
          int res = (int)pow(c1.valor_enter(), c2.valor_enter());
          node *ae = n->f_esq, *ad = n->f_dret;
          n->info = token(res);
          n->f_esq = nullptr;
          n->f_dret = nullptr;
          delete ae;
          delete ad;
          trobat = true;
          return n;
        }
        else if (c1.tipus() == token::CT_REAL)
        {
          double res = pow(c1.valor_real(), c2.valor_enter());
          node *ae = n->f_esq, *ad = n->f_dret;
          n->info = token(res);
          n->f_esq = nullptr;
          n->f_dret = nullptr;
          delete ae;
          delete ad;
          trobat = true;
          return n;
        }
      }
      else
      {
        if (c1.tipus() == token::CT_ENTERA)
        {
          int denominador = (int)pow(c1.valor_enter(), (c2.valor_enter() * (-1)));
          node *ae = n->f_esq, *ad = n->f_dret;
          n->info = token(racional(1, denominador));
          n->f_esq = nullptr;
          n->f_dret = nullptr;
          delete ae;
          delete ad;
          trobat = true;
          return n;
        }
        else if (c1.tipus() == token::CT_RACIONAL)
        {
          int numerador = (int)pow(c1.valor_racional().denom(), c2.valor_enter() * (-1)), denominador = (int)pow(c1.valor_racional().num(), c2.valor_enter() * (-1));
          node *ae = n->f_esq, *ad = n->f_dret;
          n->info = token(racional(numerador, denominador));
          n->f_dret = nullptr;
          n->f_esq = nullptr;
          delete ae;
          delete ad;
          trobat = true;
          return n;
        }
        else if (c1.tipus() == token::CT_REAL)
        {
          double res = pow(c1.valor_real(), c2.valor_enter());
          node *ae = n->f_esq, *ad = n->f_dret;
          n->info = token(res);
          n->f_esq = nullptr;
          n->f_dret = nullptr;
          delete ae;
          delete ad;
          trobat = true;
          return n;
        }
      }
    }
    else
    {
      if (c1.tipus() == token::CT_REAL and c2.tipus() == token::CT_REAL)
      {
        if (c1.valor_real() < double(0))
          throw error(NegatElevNoEnter);
        double res = pow(c1.valor_real(), c2.valor_real());
        node *ae = n->f_esq, *ad = n->f_dret;
        n->info = token(res);
        n->f_esq = nullptr;
        n->f_dret = nullptr;
        delete ae;
        delete ad;
        trobat = true;
        return n;
      }
      else if (c1.tipus() == token::CT_ENTERA and c1.valor_enter() < 0)
        throw error(NegatElevNoEnter);
      else if (c1.tipus() == token::CT_RACIONAL and c1.valor_racional() < racional(0))
        throw error(NegatElevNoEnter);
    }
  }
  else if (n->f_dret->info.tipus() == token::CT_ENTERA and n->f_dret->info.valor_enter() == 0)
  {
    n->info = token(1);
    delete n->f_esq;
    delete n->f_dret;
    n->f_dret = nullptr;
    n->f_esq = nullptr;
    trobat = true;
    return n;
  }
  else if (n->f_dret->info.tipus() == token::CT_ENTERA and n->f_dret->info.valor_enter() == 1)
  {
    node *aux = n->f_esq;
    n->info = aux->info;
    n->f_esq = aux->f_esq;
    delete n->f_dret;
    n->f_dret = aux->f_dret;
    delete aux;
    trobat = true;
    return n;
  }
  else if (n->f_dret->info.tipus() == token::CANVI_DE_SIGNE)
  {
    node *aux = n->f_dret;
    n->info = token(token::DIVISIO);
    n->f_dret = new node(token(token::EXPONENCIACIO), n->f_esq, aux->f_dret);
    n->f_esq = new node(token(1));
    delete aux;
    trobat = true;
    return n;
  }
  else if (n->f_esq->info.tipus() == token::EXPONENCIACIO)
  {
    node *aux = n->f_esq;
    n->f_esq = aux->f_esq;
    n->f_dret = new node(token(token::MULTIPLICACIO), aux->f_dret, n->f_dret);
    delete aux;
    trobat = true;
    return n;
  }
  return n;
}
expressio::node *expressio::simplifySqrt(node *n, bool &trobat) throw(error)
{
  token c = n->f_dret->info;
  if (c.tipus() == token::CT_ENTERA)
  {
    if (c.valor_enter() < 0)
      throw error(SqrtDeNegatiu);
    else if (c.valor_enter() == 0)
    {
      n->info = token(0);
      delete n->f_esq;
      delete n->f_dret;
      n->f_dret = nullptr;
      n->f_esq = nullptr;
      trobat = true;
      return n;
    }
    else
    {
      token potencia(racional(1, 2));
      n->info = token(token::EXPONENCIACIO);
      n->f_esq = n->f_dret;
      n->f_dret = new node(potencia);
      trobat = true;
      return n;
    }
  }
  else if (c.tipus() == token::CT_RACIONAL and c.valor_racional() < racional(0))
    throw error(SqrtDeNegatiu);
  else if (c.tipus() == token::CT_REAL)
  {
    if (c.valor_real() < double(0))
      throw error(SqrtDeNegatiu);
    else
    {
      double res = sqrt(c.valor_real());
      n->info = token(res);
      delete n->f_esq;
      delete n->f_dret;
      n->f_dret = nullptr;
      n->f_esq = nullptr;
      trobat = true;
      return n;
    }
  }
  else
  {
    token potencia(racional(1, 2));
    n->info = token(token::EXPONENCIACIO);
    n->f_esq = n->f_dret;
    n->f_dret = new node(potencia);
    trobat = true;
    return n;
  }
}
expressio::node *expressio::simplifyExp(node *n, bool &trobat) throw(error)
{
  if (n->f_dret->info.tipus() == token::CT_ENTERA)
  {
    if (n->f_dret->info.valor_enter() == 0)
    {
      n->info = token(0);
      delete n->f_esq;
      delete n->f_dret;
      n->f_dret = nullptr;
      n->f_esq = nullptr;
      trobat = true;
      return n;
    }
    else if (n->f_dret->info.valor_enter() == 1)
    {
      n->info = token(token::CT_E);
      delete n->f_esq;
      delete n->f_dret;
      n->f_dret = nullptr;
      n->f_esq = nullptr;
      trobat = true;
      return n;
    }
  }
  else if (n->f_dret->info.tipus() == token::CT_REAL)
  {
    double res = exp(n->f_dret->info.valor_real());
    n->info = token(res);
    delete n->f_esq;
    delete n->f_dret;
    n->f_dret = nullptr;
    n->f_esq = nullptr;
    trobat = true;
    return n;
  }
  else if (n->f_dret->info.tipus() == token::LOG)
  {
    node *aux = n->f_dret->f_dret;
    n->info = aux->info;
    delete n->f_dret;
    delete n->f_esq;
    n->f_esq = aux->f_esq;
    n->f_dret = aux->f_dret;
    delete aux;
    trobat = true;
    return n;
  }
  return n;
}
expressio::node *expressio::simplifyLog(node *n, bool &trobat) throw(error)
{
  if (n->f_dret->info.tipus() == token::CT_ENTERA)
  {
    if (n->f_dret->info.valor_enter() <= 0)
      throw error(LogDeNoPositiu);
    else if (n->f_dret->info.valor_enter() == 1)
    {
      n->info = token(0);
      delete n->f_esq;
      delete n->f_dret;
      n->f_dret = nullptr;
      n->f_esq = nullptr;
      trobat = true;
      return n;
    }
  }
  else if (n->f_dret->info.tipus() == token::CT_REAL)
  {
    if (n->f_dret->info.valor_real() < double(0))
      throw error(LogDeNoPositiu);
    double res = log(n->f_dret->info.valor_real());
    n->info = token(res);
    delete n->f_esq;
    delete n->f_dret;
    n->f_dret = nullptr;
    n->f_esq = nullptr;
    trobat = true;
    return n;
  }
  else if (n->f_dret->info.tipus() == token::CT_E)
  {
    n->info = token(1);
    delete n->f_esq;
    delete n->f_dret;
    n->f_dret = nullptr;
    n->f_esq = nullptr;
    trobat = true;
    return n;
  }
  else if (n->f_dret->info.tipus() == token::EXP)
  {
    node *fill = n->f_dret->f_dret;
    delete n->f_dret;
    delete n->f_esq;
    n->info = fill->info;
    n->f_dret = fill->f_dret;
    n->f_esq = fill->f_esq;
    trobat = true;
    return n;
  }
  else if (n->f_dret->info.tipus() == token::MULTIPLICACIO)
  {
    node *aux = n;
    node *fe = new node(token(token::LOG)), *fd = new node(token(token::LOG));
    fe->f_dret = n->f_dret->f_esq;
    fe->f_esq = nullptr;
    fd->f_dret = n->f_dret->f_dret;
    fd->f_dret = nullptr;
    delete n->f_dret;
    delete n->f_esq;
    n = new node(token(token::SUMA), fe, fd);
    delete aux;
    delete fe;
    delete fd;
    trobat = true;
    return n;
  }
  else if (n->f_dret->info.tipus() == token::DIVISIO)
  {
    node *aux = n;
    node *fe = new node(token(token::LOG)), *fd = new node(token(token::LOG));
    fe->f_dret = n->f_dret->f_esq;
    fe->f_esq = nullptr;
    fd->f_dret = n->f_dret->f_dret;
    fd->f_dret = nullptr;
    n = new node(token(token::RESTA), fe, fd);
    delete aux;
    delete fe;
    delete fd;
    trobat = true;
    return n;
  }
  else if (n->f_dret->info.tipus() == token::EXPONENCIACIO)
  {
    node *aux = n;
    node *fd = new node(token(token::LOG));
    fd->f_dret = n->f_dret->f_dret;
    n = new node(token(token::MULTIPLICACIO), n->f_dret->f_esq, fd);
    delete aux;
    delete fd;
    trobat = true;
    return n;
  }
  return n;
}
expressio::node *expressio::simplifyEvalf(node *n, bool &trobat) throw(error)
{
  if (n->f_dret->info.tipus() == token::CT_REAL)
  {
    n->info = token(n->f_dret->info);
    delete n->f_esq;
    delete n->f_dret;
    n->f_esq = nullptr;
    n->f_dret = nullptr;
    trobat = true;
    return n;
  }
  else if (n->f_dret->info.tipus() == token::CT_ENTERA)
  {
    double valor = (double)n->f_dret->info.valor_enter();
    n->info = token(valor);
    delete n->f_esq;
    delete n->f_dret;
    n->f_esq = nullptr;
    n->f_dret = nullptr;
    trobat = true;
    return n;
  }
  else if (n->f_dret->info.tipus() == token::CT_RACIONAL)
  {
    double res = (double)n->f_dret->info.valor_racional().num() / (double)n->f_dret->info.valor_racional().denom();
    n->info = token(res);
    delete n->f_esq;
    delete n->f_dret;
    n->f_esq = nullptr;
    n->f_dret = nullptr;
    trobat = true;
    return n;
  }
  else if (n->f_dret->info.tipus() == token::CT_E)
  {
    double e = 2.718281828;
    n->info = token(e);
    delete n->f_esq;
    delete n->f_dret;
    n->f_esq = nullptr;
    n->f_dret = nullptr;
    trobat = true;
    return n;
  }
  else if (n->f_dret->info.tipus() == token::VARIABLE or n->f_dret->info.tipus() == token::VAR_PERCENTATGE)
  {
    n->info = token(n->f_dret->info);
    delete n->f_esq;
    delete n->f_dret;
    n->f_esq = nullptr;
    n->f_dret = nullptr;
    trobat = true;
    return n;
  }
  else
  {
    if (isBinary(n->f_dret->info))
    {
      node *esq = new node(token(token::EVALF)), *dret = new node(token(token::EVALF));
      esq->f_dret = n->f_dret->f_esq;
      dret->f_dret = n->f_dret->f_dret;
      n->info = n->f_dret->info;
      delete n->f_dret;
      delete n->f_esq;
      n->f_esq = esq;
      n->f_dret = dret;
      trobat = true;
      return n;
    }
    else if (isFunction(n->f_dret->info) and n->f_dret->info.tipus() != token::EVALF)
    {
      n->info = n->f_dret->info;
      n->f_dret->info = token(token::EVALF);
      trobat = true;
      return n;
    }
  }
  return n;
}
void expressio::simplify_one_step() throw(error)
{
  bool trobat = false;
  simplifyParts(_arrel, trobat);
}
void expressio::simplifyParts(node *n, bool &trobat) throw(error)
{
  if (n != nullptr)
  {
    simplifyParts(n->f_esq, trobat);
    simplifyParts(n->f_dret, trobat);
    if (trobat == false)
    {
      if (n->info.tipus() == token::CT_RACIONAL)
      {
        if (n->info.valor_racional().denom() == 1)
        {
          n->info = token(n->info.valor_racional().num());
          delete n->f_esq;
          delete n->f_dret;
          n->f_dret = nullptr;
          n->f_esq = nullptr;
          trobat = true;
        }
      }
      else if (n->info.tipus() == token::SUMA)
        n = simplifySuma(n, trobat);
      else if (n->info.tipus() == token::RESTA)
        n = simplifyResta(n, trobat);
      else if (n->info.tipus() == token::CANVI_DE_SIGNE)
        n = simplifyCanviSigne(n, trobat);
      else if (n->info.tipus() == token::SIGNE_POSITIU)
      {
        node *aux = n->f_dret;
        n->info = n->f_dret->info;
        delete n->f_esq;
        n->f_esq = aux->f_esq;
        n->f_dret = aux->f_dret;
        delete aux;
        trobat = true;
      }
      else if (n->info.tipus() == token::MULTIPLICACIO)
        n = simplifyMultiplicacio(n, trobat);
      else if (n->info.tipus() == token::DIVISIO)
        n = simplifyDivisio(n, trobat);
      else if (n->info.tipus() == token::EXPONENCIACIO)
        n = simplifyExponenciacio(n, trobat);
      else if (n->info.tipus() == token::SQRT)
        n = simplifySqrt(n, trobat);

      else if (n->info.tipus() == token::EXP)
        n = simplifyExp(n, trobat);

      else if (n->info.tipus() == token::LOG)
        n = simplifyLog(n, trobat);

      else if (n->info.tipus() == token::EVALF)
        n = simplifyEvalf(n, trobat);
    }
  }
}
void expressio::simplify() throw(error)
{
  bool trobat = false;
  simplifyTot(_arrel, trobat);
  while (trobat)
  {
    trobat = false;
    simplifyTot(_arrel, trobat);
  }
}
void expressio::simplifyTot(node *n, bool &trobat) throw(error)
{
  if (n != nullptr)
  {
    simplifyParts(n->f_esq, trobat);
    simplifyParts(n->f_dret, trobat);
    if (n->info.tipus() == token::CT_RACIONAL)
    {
      if (n->info.valor_racional().denom() == 1)
      {
        n->info = token(n->info.valor_racional().num());
        delete n->f_esq;
        delete n->f_dret;
        n->f_dret = nullptr;
        n->f_esq = nullptr;
        trobat = true;
      }
    }
    else if (n->info.tipus() == token::SUMA)
      n = simplifySuma(n, trobat);
    else if (n->info.tipus() == token::RESTA)
      n = simplifyResta(n, trobat);
    else if (n->info.tipus() == token::CANVI_DE_SIGNE)
      n = simplifyCanviSigne(n, trobat);
    else if (n->info.tipus() == token::SIGNE_POSITIU)
    {
      node *aux = n->f_dret;
      n->info = n->f_dret->info;
      delete n->f_esq;
      n->f_esq = aux->f_esq;
      n->f_dret = aux->f_dret;
      delete aux;
      trobat = true;
    }
    else if (n->info.tipus() == token::MULTIPLICACIO)
      n = simplifyMultiplicacio(n, trobat);
    else if (n->info.tipus() == token::DIVISIO)
      n = simplifyDivisio(n, trobat);
    else if (n->info.tipus() == token::EXPONENCIACIO)
      n = simplifyExponenciacio(n, trobat);
    else if (n->info.tipus() == token::SQRT)
      n = simplifySqrt(n, trobat);
    else if (n->info.tipus() == token::EXP)
      n = simplifyExp(n, trobat);
    else if (n->info.tipus() == token::LOG)
      n = simplifyLog(n, trobat);
    else if (n->info.tipus() == token::EVALF)
      n = simplifyEvalf(n, trobat);
  }
}

void expressio::list_of_tokens(list<token> &lt) throw(error)
{
  listoftokens(_arrel, lt);
}

void expressio::listoftokens(node *n, list<token> &tokens)
{
  if (n != nullptr)
  {
    // Si el node actual és un operador unari, es posa primer a la llista
    if (isUnary(n->info))
    {
      tokens.push_back(n->info);
      // Si el fill dret del token actual és un operador (que no sigui l'operador d'exponenciació) es posa el contingut del fill dret entre parèntesis
      if (isOperator(n->f_dret->info) and n->f_dret->info.tipus() != token::EXPONENCIACIO)
      {
        tokens.push_back(token(token::OBRIR_PAR));
        listoftokens(n->f_dret, tokens);
        tokens.push_back(token(token::TANCAR_PAR));
      }
      else
        listoftokens(n->f_dret, tokens);
    }
    // Si el node actual és una funció, es posa primer a la llista
    else if (isFunction(n->info))
    {
      tokens.push_back(n->info);
      tokens.push_back(token(token::OBRIR_PAR));
      listoftokens(n->f_dret, tokens);
      tokens.push_back(token(token::TANCAR_PAR));
    }
    // Si el node actual és un operador binari, es posa entre els operands
    else if (isBinary(n->info) and n->info.tipus() != token::EXPONENCIACIO)
    {
      // Si el node esquerre del node actual és un operador binari amb menor precedència que el node actual, es posa primer a la llista
      if (isBinary(n->f_esq->info) and preferenciaMenor(n->f_esq->info, n->info))
      {
        tokens.push_back(token(token::OBRIR_PAR));
        listoftokens(n->f_esq, tokens);
        tokens.push_back(token(token::TANCAR_PAR));
      }
      // En qualsevol altre cas, es fa una crida recursiva a "listoftokens" amb el node esquerre del node actual
      else
        listoftokens(n->f_esq, tokens);
      // Es posa el node actual a la llista
      tokens.push_back(n->info);
      // Si el node dret del node actual és un operador binari amb menor precedència que el node actual o bé si ens trobem en un node que es tracta d'una divisió i al fill dret d'aquest node hi ha una multipiclació o una divisió, o bé, si ens trobem en una resta, i al seu fill dret i ha una suma o una resta, posem el contingut del fill dret entre parèntesis.
      if ((isBinary(n->f_dret->info) and preferenciaMenor(n->f_dret->info, n->info)) or (n->info.tipus() == token::DIVISIO and (n->f_dret->info.tipus() == token::MULTIPLICACIO or n->f_dret->info.tipus() == token::DIVISIO)) or (n->info.tipus() == token::RESTA and (n->f_dret->info.tipus() == token::SUMA or n->f_dret->info.tipus() == token::RESTA)))
      {
        tokens.push_back(token(token::OBRIR_PAR));
        listoftokens(n->f_dret, tokens);
        tokens.push_back(token(token::TANCAR_PAR));
      }
      // En qualsevol altre cas, es fa una crida recursiva a "listoftokens" amb el node dret del node actual
      else
        listoftokens(n->f_dret, tokens);
    }
    else if (n->info.tipus() == token::EXPONENCIACIO)
    {
      // Aquest if realitza les següents comprovacions:
      // 1. Comprova si el fill esquerre és una exponenciació
      // 2. Comprova si el fill esquerre és un operador binari i amb menor precedència que l'actual
      // 3. Comprova si al fill esquerre hi ha un racional, si aquest ha de portar parèntesis.
      // Si es compleix alguna de les condicions ( o totes ), el fill esquerre ha de portar parèntesis.
      if (n->f_esq->info.tipus() == token::EXPONENCIACIO or (isBinary(n->f_esq->info) and preferenciaMenor(n->f_esq->info, n->info)) or (n->f_esq->info.tipus() == token::CT_RACIONAL and (isUnary(n->f_dret->info) or (n->f_dret->info.tipus() == token::CT_ENTERA and n->f_dret->info.valor_enter() < 0) or (n->f_dret->info.tipus() == token::CT_REAL and n->f_dret->info.valor_real() < double(0)))))
      {
        tokens.push_back(token(token::OBRIR_PAR));
        listoftokens(n->f_esq, tokens);
        tokens.push_back(token(token::TANCAR_PAR));
      }
      // En qualsevol altre cas, es fa una crida recursiva a "listoftokens" amb el node dret del node actual
      else
        listoftokens(n->f_esq, tokens);
      // Es posa el node actual a la llista
      tokens.push_back(n->info);
      // Si el node esquerre del node actual és un operador binari amb menor precedència que el node actual o un racional, es posa després a la llista
      if ((isBinary(n->f_dret->info) and preferenciaMenor(n->f_dret->info, n->info)) or n->f_dret->info.tipus() == token::CT_RACIONAL)
      {
        tokens.push_back(token(token::OBRIR_PAR));
        listoftokens(n->f_dret, tokens);
        tokens.push_back(token(token::TANCAR_PAR));
      }
      // En qualsevol altre cas, es fa una crida recursiva a "listoftokens" amb el node dret del node actual
      else
        listoftokens(n->f_dret, tokens);
    }
    else
      tokens.push_back(n->info);
  }
}