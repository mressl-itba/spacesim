# Level 1: SPACESIM

En esta primera práctica vamos a optimizar una simulación gravitatoria de N-cuerpos del sistema solar.

## Contexto: la misión está en tus manos

![Logo](img/logo.png)

Año 2028. Un glitch cuántico borró del mapa a **todos** los ingenieros de [verificación y validación](https://en.wikipedia.org/wiki/Verification_and_validation) de SpaceX. La misión tripulada a Marte pende de un hilo.

**Tu tarea**: optimizar una simulación numérica que permita predecir con precisión la trayectoria de la nave y asegurar que los parámetros del vuelo sean óptimos.

¡La humanidad cuenta contigo!

## El estado de las cosas

Tus primeras tareas son:

1. Instalar [Microsoft Visual Studio](https://www.visualstudio.com/). Selecciona **Desarrollo para el escritorio con C++**.

2. Instalar Git desde https://git-scm.com/downloads.

3. Instalar [vcpkg](https://vcpkg.io/).

    En Windows:

        mkdir C:\dev
        cd C:\dev
        git clone https://github.com/Microsoft/vcpkg.git
        .\vcpkg\bootstrap-vcpkg.bat
        .\vcpkg\vcpkg integrate install

    En Linux:

        mkdir ~/dev
        cd ~/dev
        git clone https://github.com/Microsoft/vcpkg.git
        ./vcpkg/bootstrap-vcpkg.sh
        ./vcpkg/vcpkg integrate install

4. Lo único que dejaron los ingenieros glitcheados es este proyecto. Ábrelo en Visual Studio. Compíla y prueba los targets `solar_system` y `mars_mission` del proyecto.
5. Parece que no sólo los ingenieros resultaron glitcheados, sino también el mismo código que dejaron. Trata de comprenderlo para determinar qué debes solucionar. Para ello la información a continuación te será muy útil.

## Physics brushup: Ley de gravitación

La **fuerza gravitatoria** que ejerce el cuerpo *j* (masa $m_j$, posición $\mathbf{r}_j$) sobre el cuerpo *i* (masa $m_i$, posición $\mathbf{r}_i$) está dada por:

$$
\mathbf{F}_{i \leftarrow j} = -G \frac{m_i m_j}{|\mathbf{r}_i - \mathbf{r}_j|^2} \, \hat{\mathbf{u}}_{j \to i}
$$

donde $\hat{\mathbf{u}}_{j \to i}$ es el **vector unitario** que apunta **desde** *j* **hacia** *i*:

La **constante gravitacional** vale:

$$
G = 6.67430 \times 10^{-11} \, \mathrm{m}^3 \mathrm{kg}^{-1} \mathrm{s}^{-2}
$$

La **fuerza neta** sobre el cuerpo *i* es la suma vectorial de todas las contribuciones:

$$
\mathbf{F}_i = \sum_{\substack{j=1 \\ j \neq i}}^N \mathbf{F}_{i \leftarrow j}
$$

De aquí se obtiene directamente la **aceleración** (segunda ley de Newton):

$$
\mathbf{a}_i = \frac{\mathbf{F}_i}{m_i}
$$

Para conocer cómo cambian las posiciones y velocidades con el tiempo, debemos resolver el siguiente sistema de ecuaciones diferenciales ordinarias (EDOs) de primer orden:

Para obtener la evolución temporal de las posiciones y velocidades, debemos integrar la aceleración dos veces:

$$
\frac{d \mathbf{v}_i}{dt} = \mathbf{a}_i(\mathbf{r}_1, \dots, \mathbf{r}_N) \qquad \text{(ecuación del movimiento)}
$$

$$
\frac{d \mathbf{r}_i}{dt} = \mathbf{v}_i \qquad \text{(ecuación de la velocidad)}
$$

donde $i = 1, 2, \dots, N$.

En la práctica esto se realiza con un **método de integración numérica** para aproximar estas ecuaciones diferenciales.

## La misión a rescatar

Ahora que comprendes el código, haz lo siguiente:

* Encuentra y corrige todos los errores del módulo `physics/solver`. Documenta el proceso de debugging en el archivo `ENTREGA.md`, **justificando** cada decisión que tomes. Aprovecha la guía `NO-ANDA.md` para aprender sobre debugging y cosas que tu código debe verificar/validar.
* **Verifica y valida** que `mars_mission` permita que la misión aterrice en determinado lugar de marte. Investiga qué requisitos de precisión necesitas. Para ello aprovecha las efemérides del `2028-12-31_00-00-00.csv` para el estado inicial de los cuerpos celestes, y `2028-12-31_00-00-00.csv` para validar tus resultados. Investiga también metódos de integración numérica.  Aprovecha los modelos de lenguaje: [ChatGPT](https://chatgpt.com/), [Gemini](https://gemini.google.com/), [Claude](https://claude.ai/), [Grok](https://grok.com/) u otro. Documenta todos los cambios que hagas en el archivo `ENTREGA.md`, **justificando** cada decisión que tomes. 
* Optimiza `mars_mission`, quitando todos los cuellos de botella computacionales y de memoria, sin afectar la precisión necesaria, al fin y al cabo no quieres que la simulación tarde años en ejecutar. También debes cuidar la seguridad, para que tu nave no pase cerca de un asteroide. Documenta los cambios que haces en el archivo `ENTREGA.md`, **justificando** cada decisión que tomes.
* Simula una misión a marte simplificada, con una nave que se encuentra inicialmente en órbita terrestre baja (LEO, a 6871 km del centro de la tierra, velocidad 7.79 km/h). En el momento apropiado aplica un impulso para inyección transmarciana (TMI), y cuando llegas, en el momento apropiado un impulso para inserción en órbita marciana (MOI). Si necesitas ayuda para el diseño de la misión, utiliza tu modelo de lenguaje favorito. Documenta todas las decisiones que tomes en el archivo `ENTREGA.md`, **justificando** cada una de ellas.

## Cosas a cuidar

* En la industria es estándar que los equipos de trabajo respeten un estilo de código particular para mejorar la comunicación entre ellos. Los ingenieros glitcheados usaban el estilo **Google C++** con comentarios **Doxygen**: úsalos tú tambien.
* Revisa las unidades.
* Si quieres saber más sobre cómo usar Git, lee: https://learn.github.com/skills

## Notas al pie

* Las efemérides fueron calculadas con: https://ssd.jpl.nasa.gov/horizons/app.html#/
