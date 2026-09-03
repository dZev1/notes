# Practica 2 ( la hago aca porque ya la habia hecho en el cuaderno del cuatri pasado y quiero desoxidarme)

## 1. A

Queremos probar que
$$
COPRIME = \{
\langle a,b \rangle
\ | \ (a:b) = 1
\}
\in P
$$
Empecemos definiendo que es nuestro tamaño de entrada.
$$n = \lceil log \ a\rceil + \lceil log \ b\rceil + 1 = O(log\ a + log\ b)$$
Defino M, una Máquina de Turing Determinística (MTD de ahora en adelante), que haga lo siguiente:

```
<while b >= 0>
	<X = a mod b>
	<a = b>
	<b = X>
<return a == 1> (<(qf)>)
```

Si $n = | \langle a,b \rangle |$, M llega a resultado en $O(n^3)$, pues `mod` como operación consiste en restar en $O(n^2)$, y esto lo repetimos $O(n)$ veces, (hasta que le saquemos todos los bits a b). Entonces $T(M(\langle a,b\rangle)) = O(n^3)$.

Por lo tanto, como M corre en tiempo polinomial, y resuelve COPRIME, entonces $COPRIME \in P$.