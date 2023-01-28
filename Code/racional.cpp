#include "racional.hpp"
#include <iostream>

using namespace std;

// Aquesta funció té cost O(n+d), on n i d són el numerador i el denominador. Aquesta funció té un cost de O(n + d) ja que crida a la funció mcd amb n i d com a arguments.La funció mcd té un cost de O(n + d), de manera que el cost total de la funció és O(n + d).
racional::racional(int n, int d) throw(error)
{
	if (d == 0)
		throw error(21);
	nume = n;
	den = d;
	if (d < 0)
	{
		nume = -1 * nume;
		den = -1 * den;
		d = -1 * d;
	}
	if (n < 0)
		n = -1 * n;
	int gcd = mcd(n, d);
	nume /= gcd;
	den /= gcd;
}

// Aquesta funció té cost O(1)
racional::racional(const racional &r) throw(error)
{
	if (r.den == 0)
		throw error(DenominadorZero);
	nume = r.nume;
	den = r.den;
}
// Aquesta funció té cost O(1)
racional &racional::operator=(const racional &r) throw(error)
{
	if (r.den == 0)
		throw error(DenominadorZero);
	nume = r.nume;
	den = r.den;
	return *this;
}
// Aquesta funció té cost O(1)
racional::~racional() throw()
{
}

// Aquesta funció té cost O(1)
int racional::num() const throw()
{
	return nume;
}
// Aquesta funció té cost O(1)
int racional::denom() const throw()
{
	return den;
}
// Aquesta funció té cost O(1)
int racional::part_entera() const throw()
{
	if (den == 1)
		return nume;
	if (nume < 0)
	{
		return (nume / den) - 1;
	}
	else
		return nume / den;
}
// Aquesta funció té cost O(1)
racional racional::residu() const throw()
{
	if (denom() == 1)
		return racional();
	return racional(num(), denom()) - racional(racional(num(), denom()).part_entera(), 1);
}

// El cost algorístic d'aquesta funció és O(n + d + rd), on n i d són el numerador i el denominador de l 'objecte racional i rd és el denominador de l' objecte racional r.
racional racional::operator+(const racional &r) const throw(error)
{
	int n = num();
	int rn = r.num();
	int d = denom();
	int rd = r.denom();
	if (rd == d) // denes iguales
	{
		n = rn + n;
	}
	else // diferentes denes
	{
		n = n * rd;
		rn = rn * d;
		n = n + rn;
		if (mcm(d, rd) < rd * d)
			d = rd * d;
		else
			d = mcm(d, rd);
	}
	return racional(n, d);
}
// El cost algorístic d'aquesta funció és O(n + d + rd), on n i d són el numerador i el denominador de l 'objecte racional i rd és el denominador de l' objecte racional r.
racional racional::operator-(const racional &r) const throw(error)
{
	int n = num();
	int rn = r.num();
	int d = denom();
	int rd = r.denom();
	if (rd == d) // denes iguales
	{
		if (rn < 0)
		{
			rn = (-1) * rn;
			n = rn + n;
		}
		else
			n = rn - n;
	}
	else // diferentes denes
	{
		n = n * rd;
		rn = rn * d;
		n = n - rn;
		if (mcm(d, rd) < rd * d)
			d = rd * d;
		else
			d = mcm(d, rd);
	}
	return racional(n, d);
}
// El cost d'aquesta funció és O(1)
racional racional::operator*(const racional &r) const throw(error)
{
	int n = num();
	int rn = r.num();
	int d = denom();
	int rd = r.denom();
	if (rd == 0 or d == 0)
		throw error(DenominadorZero);
	n = n * rn;
	d = d * rd;
	return racional(n, d);
}
// El cost d'aquesta funció és O(1)
racional racional::operator/(const racional &r) const throw(error)
{
	int n = num();
	int rn = r.num();
	int d = denom();
	int rd = r.denom();
	if (rd == 0 or d == 0)
		throw error(DenominadorZero);
	n = n * rd;
	d = d * rn;
	return racional(n, d);
}

// El cost d'aquesta funció és O(1)
bool racional::operator==(const racional &r) const throw()
{
	int n = num();
	int rn = r.num();
	int d = denom();
	int rd = r.denom();
	return n == rn and d == rd;
}
// El cost d'aquesta funció és O(1)
bool racional::operator!=(const racional &r) const throw()
{
	return not(*this == r);
}
// El cost d'aquesta funció és O(1)
bool racional::operator<(const racional &r) const throw()
{
	int n = num();
	int rn = r.num();
	int d = denom();
	int rd = r.denom();
	double a = (n * 1.0) / d;
	double b = (rn * 1.0) / rd;
	return a < b;
}
// El cost d'aquesta funció és O(1)
bool racional::operator>(const racional &r) const throw()
{
	int n = num();
	int rn = r.num();
	int d = denom();
	int rd = r.denom();
	double a = (n * 1.0) / d;
	double b = (rn * 1.0) / rd;
	return a > b;
}
// El cost d'aquesta funció és O(1)
bool racional::operator<=(const racional &r) const throw()
{
	return not(*this > r);
}
// El cost d'aquesta funció és O(1)
bool racional::operator>=(const racional &r) const throw()
{
	return not(*this < r);
}

// El cost algorístic d'aquesta funció és O(a+b), on a i b són els dos arguments enters passats a la funció.
int racional::mcd(int a, int b)
{
	int R;
	while ((a % b) > 0)
	{
		R = a % b;
		a = b;
		b = R;
	}
	return b;
}

// El cost algorístic d'aquesta és O(a+b), on a i b són els dos arguments enters passats a la funció. Aquesta funció té un cost de O(a + b) ja que crida a la funció mcd amb a i b com a arguments.La funció mcd té un cost de O(a + b), de manera que el cost total de la funció és O(a + b).
int racional::mcm(int a, int b)
{
	return a * b / mcd(a, b);
}
