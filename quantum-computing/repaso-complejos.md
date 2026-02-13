# Números Complejos ($\mathbb{C}$)
>[!Definition] i
>Definimos a $i$ como a la unidad imaginaria, cuyo valor elevado al cuadrado nos da -1.
> $$i \in \mathbb{C} : i^2 = -1$$

- Las potencias de $i$ son cíclicas.
	- $i^0 = 1$
	- $i^1 = i$
	- $i^2 = -1$
	- $i^3 = -i$
	- $i^4 = 1$
	- ...
	- $i^k=i^{k \ mod \ 4}$

> [!Definition] Número complejo
> Un número complejo es una expresión $c = a + b \cdot i$, donde $a,b \in \mathbb{R}$, e $i$ es el número tal que $i^2 = -1$. Al conjunto denotado por todos los números complejos, lo notamos $\mathbb{C}$. Cuando se sobreentiende el producto, no hace falta notar el $\cdot$.
> $$ \mathbb{C}= \{ w \ | \ w = a + bi: a,b \in \mathbb{R} \wedge i^2 = -1\}$$

- Los números complejos son cerrados bajo multiplicación y adición.
	- Sean $w_1 = a + bi$ y $w_2 = c + d i$, entonces:
		- $w_1 + w_2 = (a + bi) + (c + di) = (a+c) + (b + d) i \ \in \ \mathbb{C}$
		- $w_1 \cdot w_2 = (a + bi) \cdot (c + di) = (ac + adi) + (bci + bd i^2) = ac + adi + bci - bd = (ac - bd) + (ad + bc) i \in \mathbb{C}$
- Sea $w = a + bi$, podemos notar a la **Parte Real** ($Re$) y a la **Parte Imaginaria** ($Im$) de $w$ como:
	- $Re(w) = a$
	- $Im(w) = b$
- Podemos representar a estos números en un sistema de ejes cartesianos, donde el eje horizontal representa $Re$ y el eje vertical, $Im$:
![[Pasted image 20260126185220.png]]
> [!Theorem] Teorema Fundamental del Álgebra
> Toda ecuación polinómica de una variable con coeficientes complejos tiene una solución compleja.


```Haskell
newtype ComplexNumber = ComplexNumber (Float, Float)

  
instance Show ComplexNumber where
    show (ComplexNumber (a, b)) = show a ++ " + " ++ show b ++ "*i"

sumProductOfComplex :: ComplexNumber -> ComplexNumber -> (ComplexNumber, ComplexNumber)
sumProductOfComplex w1 w2 = (complexSum w1 w2, complexProduct w1 w2)
  where
    complexSum (ComplexNumber (a,b)) (ComplexNumber (c,d)) = ComplexNumber (a + c, b + d)
    complexProduct (ComplexNumber (a,b)) (ComplexNumber (c,d)) = ComplexNumber (a*c - b*d, a*d + b*c)
```

- Los números reales pueden ser pensados como pares $(a,0)$
	- $a \mapsto (a,0)$
- Los números puramente imaginarios son pares $(0,b)$, particularmente
	- $i \mapsto (0,1)$
- Las operaciones se pueden definir como
	- $(a,b) + (c,d) = (a + c, b + d)$
	- $(a,b) * (c,d) = (ac - bd, ad + bc)$
		- Entonces veamos que $i*i = (0,1) * (0,1) = (-1,0) = -1$
			- $(0,1) * (0,1) = (0*0-1*1, 0*1 + 1*0) = (-1, 0) = -1$
- Ahora entonces, pasamos de tener un número misterioso $i$ a poder definirlo bajo un álgebra nuevo, donde $i = (0,1)$.
- Las operaciones de los números complejos cumplen con la **conmutatividad**, **asociatividad** y **distributividad**.
- Sea $z = a + bi$ Definimos el módulo de z como$|z| = \sqrt{a^2 + b^2}$ y definimos al conjugado de z como $\bar{z} = a - bi$.
	- Ver que $z*\bar{z} = |z|^2$
	- La conjugación respeta a la suma $\bar{z} + \bar{w} = {\overline{z+w}}$
	- La conjugación respeta el producto $\bar{z} * \bar{w} = \overline{z * w}$
	- La conjugación es biyectiva, es un **isomorfismo de campo** $\mathbb{C} \rightarrow \mathbb{C}$ 
	- Entonces ahora, la división queda definida como $\frac{z}{w} = \frac{a+bi}{c+di} * \frac{c-di}{c-di} =\frac{ac-(bd)i}{c^2+d^2} = \frac{z*\bar{w}}{|w|^2}$
		- Esto cumple con que si $\frac{z}{z} = 1$, pues $\frac{z*\bar{z}}{|z|^2} = \frac{|z|^2}{|z|^2} = 1$
		- Vale entonces que $\frac{1}{z} = \frac{|z|}{|z|^2}$
- $z = a+bi, w=c+di \in \mathbb{C}$, definimos $z-w = (a-c) + (b-d)i$.
- Como $\mathbb{C}$ es un conjunto definido bajo las 4 operaciones, que verifican:
	- Suma es conmutativa y asociativa.
	- Multiplicación es conmutativa y asociativa.
	- La suma tiene una identidad: (0,0).
	- La multiplicación tiene una identidad: (1,0).
	- La multiplicación distribuye respecto a la suma.
	- El inverso de la suma (resta) está definido en todos lados.
	- El inverso de la multiplicación (división) está definido en todos lados excepto cuando el divisor es cero.
	Entonces $\mathbb{C}$ es un **campo**. Podemos pensar a $\mathbb{R}$ como un subconjunto de los números complejos $\mathbb{R} = \{w \in \mathbb{C} : Im(w) = 0 \}$. Encima, como $\mathbb{C}$ contiene todas las soluciones para cualquiera de sus ecuaciones polinómicas, entonces se dice que $\mathbb{C}$ es **algebraicamente completo**.

- **Representación polar** de un número complejo
	- $w = a + bi = \rho(cos(\theta) + i * sin(\theta))$, donde 
		- $\rho = \sqrt{a^2+b^2}$
		- $\theta = tan^{-1} \lparen \frac{b}{a} \rparen$

>[!Theorem] Teorema de De Moivre
>Dado un número complejo dado en forma polar $z=\rho(cos(\theta)+i*sin(\theta))$, entonces se cumple que:
>$$\rho e^{i*\theta} = \rho (cos(\theta) + i*sin(\theta))$$

- También se cumple:
$$(\rho e^{\theta*i})^n = \rho^n e^{n*\theta*i} = \rho^n (cos(n\theta) + i*sin(n\theta))$$
>[!Definition] Raíces de la unidad ($G_n$)
>Sea $n \in \mathbb{N}$, entonces definimos al grupo $G_n$ como las raíces n-ésimas de la unidad:
> $$G_{n} = \{ w \in \mathbb{C} : w^n = e^{\frac{2k\pi i}{n}} = 1, \ con \ 0 \le k \le n - 1\}$$

