#include "variables.hpp"
using namespace std;

variables::node::node(const string &k, const expressio &v, node *esq, node *dret) throw(error) : _k(k), _v(v), _esq(esq), _dret(dret), _altura(1)
{
}
// Aquesta funció té cost O(n) on n és el nombre de nodes
void variables::esborra(node *n)
{
    if (n != nullptr) // Si arribem a un node nul, retorna
    {
        esborra(n->_esq);  // Allibera l'arbre esquerre
        esborra(n->_dret); // Allibera l'arbre dret
        delete n;          // Allibera el node actual
    }
}

// Aquesta funció té cost O(n) on n és el nombre de nodes
variables::node *variables::copia(node *n)
{
    if (n == nullptr)
        return nullptr; // Si arribem a un node nul, retorna nullptr

    // Copia el node actual
    node *node_copiat = new node(n->_k, n->_v);

    // Copia l'arbre esquerre
    node_copiat->_esq = copia(n->_esq);

    // Copia l'arbre dret
    node_copiat->_dret = copia(n->_dret);

    return node_copiat;
}

// Aquesta funció té cost O(1)
int variables::altura(node *n)
{
    if (n == nullptr)
        return 0;
    return n->_altura;
}
// Aquesta funció té cost O(1)
int variables::factor_equilibri(node *n)
{
    if (n == nullptr)
        return 0;
    return altura(n->_esq) - altura(n->_dret);
}

// Aquesta funció té cost O(1)
variables::variables() throw(error)
{
    _arrel = NULL;
}

// Aquesta funció té cost O(n) on n és el nombre de nodes
variables::variables(const variables &v) throw(error)
{
    // Copia l'arbre de "v" a l'arbre actual
    _arrel = copia(v._arrel);
}
// Aquesta funció té cost O(n) on n és el nombre de nodes
variables &variables::operator=(const variables &v) throw(error)
{
    esborra(_arrel);
    // Copia l'arbre de "v" a l'arbre actual
    _arrel = copia(v._arrel);
    return *this;
}
// Aquesta funció té cost O(n) on n és el nombre de nodes
variables::~variables() throw(error)
{
    esborra(_arrel);
    _arrel = nullptr;
}

// Aquesta operació té un cost algorísmic d'O(1), ja que només implica canviar alguns apuntadors i actualitzar algunes altures de nodes.
variables::node *variables::rotacio_dreta(node *y)
{
    node *x = y->_esq;
    node *T2 = x->_dret;
    // Realitzem la rotació
    x->_dret = y;
    y->_esq = T2;
    // Actualitzem les altures
    y->_altura = max(altura(y->_esq), altura(y->_dret)) + 1;
    x->_altura = max(altura(x->_esq), altura(x->_dret)) + 1;
    // Retornem la nova arrel
    return x;
}
//  Aquesta operació té un cost algorísmic d'O(1), ja que només implica canviar alguns apuntadors i actualitzar algunes altures de nodes.
variables::node *variables::rotacio_esquerra(node *x)
{
    node *y = x->_dret;
    node *T2 = y->_esq;
    // Realitzem la rotació
    y->_esq = x;
    x->_dret = T2;
    // Actualitzem les altures
    x->_altura = max(altura(x->_esq), altura(x->_dret)) + 1;
    y->_altura = max(altura(y->_esq), altura(y->_dret)) + 1;
    // Retornem la nova arrel
    return y;
}

// Aquesta funció té un cost algorístic d'O(log n) en el cas general, ja que cerca el node on s'ha de fer l'inserció o modificació en l'arbre AVL mitjançant una cerca binària. Un cop ha trobat el node, actualitza la informació del node i, si cal, realitza rotacions per mantenir l'arbre equilibrat. Això fa que el cost de l'operació sigui logarítmic en el nombre de nodes de l'arbre.
void variables::assign(const string &v, const expressio &e) throw(error)
{
    _arrel = assign(_arrel, v, e);
}
// Aquesta funció té un cost algorístic d'O(log n) en el cas general, ja que cerca el node on s'ha de fer l'inserció o modificació en l'arbre AVL mitjançant una cerca binària. Un cop ha trobat el node, actualitza la informació del node i, si cal, realitza rotacions per mantenir l'arbre equilibrat. Això fa que el cost de l'operació sigui logarítmic en el nombre de nodes de l'arbre.
variables::node *variables::assign(node *n, const string &k, const expressio &v)
{
    if (n == nullptr)
        return new node(k, v);

    if (k < n->_k)
        n->_esq = assign(n->_esq, k, v);
    else if (k > n->_k)
        n->_dret = assign(n->_dret, k, v);
    else
    {
        n->_v = v;
        return n;
    }
    // 2. Actualitzem l’altura
    n->_altura = max(altura(n->_esq), altura(n->_dret)) + 1;
    // 3. Obtenim el factor d’equilibri per veure si està balancejat
    int fe = factor_equilibri(n);
    // Cas EE
    if (fe > 1 && k < n->_esq->_k)
        return rotacio_dreta(n);
    // Cas DD
    if (fe < -1 && k > n->_dret->_k)
        return rotacio_esquerra(n);
    // Cas ED
    if (fe > 1 && k > n->_esq->_k)
    {
        n->_esq = rotacio_esquerra(n->_esq);
        return rotacio_dreta(n);
    }
    // Cas DE
    if (fe < -1 && k < n->_dret->_k)
    {
        n->_dret = rotacio_dreta(n->_dret);
        return rotacio_esquerra(n);
    }
    return n;
}

// Aquesta funció té cost O(log n), on n és el nombre de nodes de l'arbre, ja que cercal el node a eliminar i un cop trobat elmina el node i si cal fa les rotacions per a mantindre l'arbre equilibrat, fet que fa que el cost sigui O(log n)
void variables::unassign(const string &v) throw()
{
    _arrel = unassign(_arrel, v);
}
// Aquesta funció té cost O(log n), on n és el nombre de nodes de l'arbre, ja que cercal el node a eliminar i un cop trobat elmina el node i si cal fa les rotacions per a mantindre l'arbre equilibrat, fet que fa que el cost sigui O(log n)
variables::node *variables::unassign(node *n, const string &k)
{
    if (n == nullptr)
    {
        // Retorna null si l'arbre és buit o la variable no existeix
        return nullptr;
    }
    else if (k < n->_k)
    {
        // Es recorre a l'arbre esquerre si el nom de la variable és lexicogràficament menor que el nom de la variable del node actual
        n->_esq = unassign(n->_esq, k);
    }
    else if (k > n->_k)
    {
        // Es recorre a l'arbre dret si el nom de la variable és lexicogràficament major que el nom de la variable del node actual
        n->_dret = unassign(n->_dret, k);
    }
    else
    {
        // Esborra el node si la variable existeix
        if (n->_esq == nullptr)
        {
            // Cas 1: El node no té fill esquerre
            node *temp = n->_dret;
            delete n;
            return temp;
        }
        else if (n->_dret == nullptr)
        {
            // Cas 2: El node no té fill dret
            node *temp = n->_esq;
            delete n;
            return temp;
        }
        else
        {
            // Cas 3: El node té dos fills
            node *temp = minValueNode(n->_dret);
            n->_k = temp->_k;
            n->_v = temp->_v;
            n->_dret = unassign(n->_dret, temp->_k);
        }
    }
    if (n == nullptr)
        return n;
    // 2. Actualitzem l’altura
    n->_altura = max(altura(n->_esq), altura(n->_dret)) + 1;
    // 3. Obtenim el factor d’equilibri per veure si està balancejat
    int fe = factor_equilibri(n);
    // Cas EE
    if (fe > 1 and factor_equilibri(n->_esq) >= 0)
        return rotacio_dreta(n);
    // Cas DD
    if (fe < -1 and factor_equilibri(n->_dret) <= 0)
        return rotacio_esquerra(n);
    // Cas ED
    if (fe > 1 and factor_equilibri(n->_esq) < 0)
    {
        n->_esq = rotacio_esquerra(n->_esq);
        return rotacio_dreta(n);
    }
    // Cas DE
    if (fe < -1 and factor_equilibri(n->_dret) > 0)
    {
        n->_dret = rotacio_dreta(n->_dret);
        return rotacio_esquerra(n);
    }
    return n;
}
// Aquesta funció té un cost algorístic d'O(log n) ja que es recorre l'arbre des de l'arrel fins al node amb la clau mínima.
variables::node *variables::minValueNode(node *n)
{
    node *actual = n;
    while (actual->_esq != nullptr)
        actual = actual->_esq;
    return actual;
}

// Aquesta funció  té un cost algorístic d'O(log n), ja que cerca el node amb la clau cercada en l'arbre AVL mitjançant una cerca binària. Això fa que el cost de l'operació sigui logarítmic en el nombre de nodes de l'arbre.
expressio variables::valor(const string &lv) const throw(error)
{
    // Cerca la variable en l'arbre
    node *n = cerca(_arrel, lv);

    // Si la variable es troba en l'arbre, retorna el seu valor
    if (n != nullptr)
        return n->_v;

    // Si la variable no es troba en l'arbre, retorna l'expressió buida
    return expressio();
}
// Aquesta funció  té un cost algorístic d'O(log n), ja que cerca el node amb la clau cercada en l'arbre AVL mitjançant una cerca binària. Això fa que el cost de l'operació sigui logarítmic en el nombre de nodes de l'arbre.
variables::node *variables::cerca(node *n, const string &lv)
{
    if (n == nullptr)
        return nullptr; // Si arribem a un node nul, retorna nullptr
    if (lv == n->_k)
        return n; // Si trobem la variable, retorna l'adreça del node
    if (lv < n->_k)
        return cerca(n->_esq, lv); // Si la variable és més petita, cerca a l'arbre esquerre
    else
        return cerca(n->_dret, lv); // Si la variable és més gran, cerca a l'arbre dret
}

// Aquesta funció té cost algorístic d'O(n), ja que cada node de l'arbre es visita només un cop i es fa una única operació per node. Això fa que el cost de l'operació sigui lineal en el nombre de nodes de l'arbre.
void variables::dump(list<string> &l) const throw(error)
{
    // Crida a la funció auxiliar per recórrer l'arbre en ordre de l'esquerra a la dreta
    rdump(_arrel, l);
}

// Aquesta funció té cost algorístic d'O(n), ja que cada node de l'arbre es visita només un cop i es fa una única operació per node. Això fa que el cost de l'operació sigui lineal en el nombre de nodes de l'arbre.
void variables::rdump(node *n, list<string> &l)
{
    if (n == nullptr)
        return; // Si arribem a un node nul, retorna

    // Recorre l'arbre esquerre
    rdump(n->_esq, l);

    // Afegeix la clau del node a la llista
    l.push_back(n->_k);

    // Recorre l'arbre dret
    rdump(n->_dret, l);
}
