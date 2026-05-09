# Espacios vectoriales complejos

La teoría cuántica, se transmite en el lenguaje de espacios vectoriales complejos, los cuales son estructuras matemáticas basadas en números complejos.

## $\mathbb{C}^{n}$ como el principal ejemplo

El principal ejemplo de espacios vectoriales complejos es el conjunto de vectores de una longitud fija con entradas complejas. Estos vectores describirán los estados de los sistemas cuánticos y las computadoras cuánticas.

Veamos por ejemplo, $\mathbb{C}^4$. Un elemento de $\mathbb{C}^4$ se ve así:
$$
V = \begin{bmatrix}
6 - 4i \\
7 + 3i \\
4.2 - 8.1i \\
-3i
\end{bmatrix}
$$
Denotamos al j-ésimo elemento de $V$ como $V[j]$. $0 \leq j < 4$, entonces $V[1] = 7 + 3i \neq 6 - 4i$

### Operaciones en $\mathbb{C}^n$

#### Suma

Sean
$$
V = \begin{bmatrix}
v_0 \\
v_1 \\
... \\
v_{n-1}
\end{bmatrix} \ \ y \  \ \
W = \begin{bmatrix}
 w_0 \\
 w_1 \\
 ... \\
 w_{n-1}
\end{bmatrix}
$$
Definimos la suma como la suma de sus entradas

$$
V + W = \begin{bmatrix}
v_0 + w_0 \\
v_1 + w_1 \\
... \\
v_{n-1} + w_{n-1}
\end{bmatrix}
$$
Es decir que $(V+W)[j] = V[j] + W[j]$

La suma es **conmutativa** ($V + W = W + V$) y es **asociativa** ($(V + W) + X = V + (W + X)$).

#### Vector Nulo

$$
\mathbf{0} = \begin{bmatrix}
0 \\
0 \\
... \\
0
\end{bmatrix}
$$
El vector nulo satisface que, $\forall V \in \mathbb{C}^n,$ tenemos
$$V + \mathbf{0} = V = \mathbf{0} + V$$

#### Inverso aditivo

Sea $V \in \mathbb{C}^4$, definimos $-V$ como el vector tal que $V + (-V) = \mathbf{0}$

>[!Theorem] Inverso aditivo
>Para todo vector $V \in \mathbb{C}^n$, existe $-V \in \mathbb{C}^n$ tal que 
>$$
>V + (-V) = V - V = \mathbf{0}
>$$
>donde $(-V)[j] = -(V[j])$
