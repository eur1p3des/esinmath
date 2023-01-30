# ESINMATH
ESINMATH és un petit programa de càlcul simbòlic que consta d'una sèrie de classes i mòduls. Encara que no ofereix la funcionalitat de programes com MAPLE, MATHEMATICA o MUPAD, ESINMATH és capaç de realitzar les funcions més bàsiques típiques d'aquests sistemes.

Habitualment ESINMATH s’usarà en mode interactiu: l’usuari entra una comanda com a resposta al prompt del sistema, ESINMATH la processa i dóna la resposta requerida, i torna a esperar una nova comanda de l’usuari.
Hi ha quatre tipus de comandes reconeguts per ESINMATH:

1. expressions que ESINMATH avalua i el resultat de les quals imprimeix.
2. assignacions de la forma x := E, on x és una variable qualsevol i E una expressió.
3. unassign x, ens permet desassignar el valor assignat a una variable x.
4. la comanda byebye finalitza la sessió amb ESINMATH.

El següent exemple mostra una possible interacció amb el sistema (les respostes del sistema apareixen amb un color de lletra diferent i les comandes entrades per l’usuari apareixen precedits pel prompt ’>’). Per comoditat representarem en aquest document l’operació d’exponenciació mitjançant l’accent circumflex (ˆ).

```bash
> 2 + 5
7
> -2+8*9
70
> f:=a-b*c
a-b*c
> a:=3
3
> f
3-b*c
> b:= 0
0
> f
3
> 3/4-1/3
5/12
> sqrt(2)
2ˆ(1/2)
> evalf(%)
1.414213562
> p:= xˆ2-8*x+3
xˆ2-8*x+3
> q:= diff(p,x)
2*x-8
> x:= 3
3
> q
-2
> unassign x
x
> q
2*x-8
```
Les principals funcions que realitza ESINMATH són:
- Gestionar expressions amb nombres enters, racionals i reals en coma flotant
- Emmagatzemar expressions en variables
- Aplicar funcions com arrel quadrada, logaritmes i exponencials
- Calcular el valor d'una expressió en coma flotant
- Realitzar simplificacions
- La funció evalf avalua l'expressió donada en coma flotant
- El símbol % és sempre l'última expressió avaluada
- Una vegada desassignem el valor a la variable x l'avaluació de la variable q té com a resultat l'expressió que se li va assignar originalment.

## Nota:
> Una característica singular d'ESINMATH és que una vegada desassignem el valor a la variable x l'avaluació de la variable q té com a resultat l'expressió que se li va assignar originalment.

> Abans de compilar el projecte, és important instalar la biblioteca LIBESIN als vostres dispositius, seguint la guia que hi ha al fitxer .zip.
