# Tarea-Progra---III

## Con esta actividad buscamos crear un tensor, usando clases. También aplicaremos operaciones entre tensores, producto bidimensionales, producto punto y algunos métodos para los tensores.

## Atributos:

#### - double* Datos; (alamacena los datos del tensor)
#### - vector<int> dimensiones; (Recibe el vector con las dimensiones - max. 3)
#### - int tamanio_total; (Obtenemos el tamaño del tensor, para eso usamos una funcion pequeña en private)
#### - bool posee_datos; (verificamos la existencia de Datos)

![alt text](image-1.png)


### Constructores

#### 1.- Principal -> Este recibe un vector de dimensiones y otro de valores. El vector de dimensiones se le atribuye a dimensiones y con el otro vector almacenamos los datos en el array.

![alt text](image.png)

#### //Cabe resaltar que se verifica que tenga max. 3 dimensiones y que el tamaño del tensor coincida con el de los valores.

#### También añadimos un constructor vacío y uno para que tanga datos nulos en caso se requieras.

![alt text](image-2.png)


#### 2.- Constructor y asiganción de copia

#### Creamos el constructor de copia y su asignación

![alt text](image-3.png)



