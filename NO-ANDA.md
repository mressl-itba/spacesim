# NO-ANDA

## La Épica Guía de Debugging en C con Visual Studio

¡Ey, valiente programador! ¿Tu código en C no funciona? ¿Sientes que estás en una batalla épica contra bugs monstruosos que acechan en las sombras de tu código? ¡No temas! Esta guía es tu brújula para salir del pozo.

## Paso 0: DON'T PANIC

Como dice esa vieja broma: **¿Qué haces si estás en un pozo? Deja de cavar.**

Tu código tiene un bug, pero el bug no se va a mover. Está ahí, agazapado en tu código, esperando que lo encuentres. Si empiezas a cambiar cosas al azar, ¡boom!: ahora tienes dos bugs.

**Consejo motivacional:** Respira hondo. Pon música épica. Cambia a modo detective.

Los novatos agregan código correctivo; los expertos eliminan el defectuoso. ¡Tú vas por experto!

## Paso 1: Identifica el DÓNDE

Pregúntate: **Exactamente, ¿dónde está el problema?**

No digas "no anda". Sé preciso, como un cirujano. Clasifica tu error en una de estas categorías malvadas:

* **Error de compilación:** No compila. Mensaje críptico en la ventana de salida.
* **Crash del programa:** Compila, corre, pero explota (¡kabum!).
* **Bucle infinito:** Corre... y corre... y nunca termina. ¿O está esperando input?
* **Error lógico:** Corre, pero da resultados equivocados. ¡El más traicionero!

Ahora, profundiza:

* **Error de compilación:** ¿Cuál es el mensaje de error específico? ¿En qué línea? Enfócate sólo en el primer error — Ignora los siguientes porque son como ecos de un grito inicial. En Visual Studio, haz doble clic en el error para ir directo al código.
* **Crash del programa:** ¿En qué línea crashea? Usa el depurador: Presiona F5 (o Depurar > Iniciar depuración). Cuando crashee, VS te muestra el **stack trace**. Nota el tipo de error (e.g., access violation – ¡intentaste leer memoria prohibida!).
* **Bucle infinito:** Ejecuta en depuración (F5), cuando parezca colgado, presiona el botón de pausa (Break All). Mira el **call stack** para ver dónde está atascado.
* **Error lógico:** ¿Qué función produce la salida errónea? Localiza: Pon breakpoints (F9 en una línea) y corre paso a paso.

## Paso 2: Identifica el CUÁNDO

Ya localizaste el bug. Bien. Ahora pregúntate: **¿Qué es lo que causa el error?**

* No asumas que es "siempre". Prueba variaciones para encontrar patrones.
* Escribe casos que fallen y que no. ¡Reduce el input al mínimo que reproduzca el bug! E.g., si falla en archivo gigante, encuentra el patrón mínimo.

## Paso 3: Hipotetiza sobre la causa

Con el bug localizado y reproducible, ¡juega a Sherlock! Pregúntate: ¿Por qué pasa esto?

Lista hipótesis:

* ¿Off-by-one?
* ¿Puntero nulo?
* ¿Overflow?
* ¿Memoria no inicializada?

Errores comunes:

* **Punteros salvajes:** ¿Dereferencias NULL? Usa `if (ptr != NULL)`.
* **Memory leaks/overflows:** En VS, usa Diagnostic Tools (Depurar > Windows > Diagnostic Tools) para ver memoria.
* **Off-by-one en loops:** `for(int i = 0; i <= n; i++)`: ¡el <= mata!
* **No free() malloc:** Pero crashes tardíos.
* **Missing dependencies:** Librerías no encontradas. Usa `find_package()` correctamente o ajusta `CMAKE_PREFIX_PATH`.
* **Cache stale:** Cierra Visual Studio, borra la carpeta `build` e intenta de nuevo.

**Tip entusiasta:** ¡Sé científico! Hipótesis -> Prueba -> Refuta. No adivines, verifica.

## Paso 4: Prueba tu hipótesis

¡Acción! Usa el depurador para validar:

* Pon **breakpoints** en spots sospechosos.
* **Inspecciona variables:** En VS, agrega a Watch con expresiones como array[5].
* **Conditional breakpoints:** Derecho-clic en breakpoint > Settings > Conditions (e.g., cuando i==10).
* Si es memory issue, corre con `/DEBUG` en propiedades del proyecto.
* **Para lógica:** Imprime con `printf()` temporales (pero quítalos después, ¡no seas perezoso!).

Si falla la hipótesis, vuelve a Paso 3. ¡Itera hasta la victoria!

## Errores comunes en C y cómo matarlos

Aquí una lista rápida, con fixes:

* **Segmentation Fault/Access Violation:** Intentas acceder memoria inválida. Fix: Chequea punteros, bounds en arrays.
* **Infinite loop:** Condición nunca falsa. Fix: Pausa depurador, chequea variables en loop.
* **Undefined behavior:** E.g., división por cero. Fix: Agrega chequeos.
* **Memory leaks:** No hay `free()`. Fix: Usa **VS tools**, **_CrtDumpMemoryLeaks()** o **valgrind**.
* **Wrong output:** Lógica off. Fix: Dry-run en papel, compara con salida esperada.

## Paso 5: ¡Conviértete en depurador legendario!

* **Rubber Duck Debugging:** Explica tu código a un patito de goma. ¡Magia! Encontrarás bugs solo hablando.
* **Control de versiones:** Usa Git. Haz Commit a menudo, así puedes revertir los cambios si algo se rompe.
* **Lee Docs:** MSDN para VS debugger – busca "Debugging in Visual Studio".
* **Practica:** Buggea código adrede y arréglalo. ¡No hay como entrenar!
* **No te rindas:** Cada bug arreglado te hace 10x mejor. ¡Tú puedes!

¿Aún atascado? Te esperamos con tu bug localizado y reproducible, ¡los ayudantes te amarán!

Ahora, ¡vuelve a tu código y conquístalo! 🚀
