#include <chrono>
#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
using namespace std;

class Tensor;

class TensorTransform {
public :
virtual Tensor apply ( const Tensor & t ) const = 0;
    virtual ~ TensorTransform () = default ;
};

class Tensor {
private:
    double* datos;
    vector<int> dimensiones;
    int tamanio_total;
    bool posee_datos;
    friend class ReLu;
    friend class Sigmoid;
    friend Tensor dot(const Tensor& t1, const Tensor& t2);
    friend Tensor matmul(const Tensor& t1, const Tensor& t2);
    int calcular_tamanio(const vector<int>& dims) {
        if (dims.empty()) return 0;
        int t = 1;
        for (int d : dims) t *= d;
        return t;
    }



public:


    // Constructor principal (Parte 3)
    Tensor(const vector<int>& dims, const vector<double>& valores) {
        dimensiones = dims;
        tamanio_total = calcular_tamanio(dims);
        posee_datos = true;

        if (dimensiones.size() > 3) {
            cout << "Error: El tensor no puede tener mas de 3 dimensiones" << endl;
            datos = nullptr;
            return;
        }

        if (valores.size() != tamanio_total) {
            cout << "Error: La cantidad de valores no coincide con la forma del tensor" << endl;
            datos = nullptr;
            return;
        }

        datos = new double[tamanio_total];
        for (int i = 0; i < tamanio_total; i++) {
            datos[i] = valores[i];
        }
    }

    // Constructor vacío (para operaciones)
    Tensor() : datos(nullptr), tamanio_total(0), posee_datos(true) {}

    // Constructor para vista (no posee datos)
    Tensor(const vector<int>& dims, double* ptr, bool posee)
        : dimensiones(dims), tamanio_total(calcular_tamanio(dims)), datos(ptr), posee_datos(posee) {}

    //--------------------------------constructos copia-----------------------------------------
    // Constructor de copia
    Tensor(const Tensor& otro) {
        dimensiones = otro.dimensiones;
        tamanio_total = otro.tamanio_total;
        posee_datos = true;

        datos = new double[tamanio_total];
        for (int i = 0; i < tamanio_total; i++) {
            datos[i] = otro.datos[i];
        }
    }

    // Asignación de copia
    Tensor& operator=(const Tensor& otro) {
        if (this != &otro) {
            if (posee_datos && datos != nullptr) {
                delete[] datos;
            }

            dimensiones = otro.dimensiones;
            tamanio_total = otro.tamanio_total;
            posee_datos = true;

            datos = new double[tamanio_total];
            for (int i = 0; i < tamanio_total; i++) {
                datos[i] = otro.datos[i];
            }
        }
        return *this;
    }

    //Constructor de movimiento
    Tensor(Tensor&& otro) {
        datos = otro.datos;
        dimensiones = move(otro.dimensiones);
        tamanio_total = otro.tamanio_total;
        posee_datos = otro.posee_datos;

        otro.datos = nullptr;
    }
    Tensor& operator=(Tensor&& otro) {
        if (this != &otro) {
            if (posee_datos && datos != nullptr) {
                    delete datos;
            }
            datos = otro.datos;
            dimensiones = move(otro.dimensiones);
            tamanio_total = otro.tamanio_total;
            posee_datos = otro.posee_datos;
            otro.datos = nullptr;
        }
        return *this;
    }
    // Destructor
    ~Tensor() {
        if (posee_datos && datos != nullptr) {
            delete[] datos;
        }
    }

    //----------------------------------------------------------------------------------------
    // Metodos estaticos (Parte 3)

    static Tensor ceros(const vector<int>& dimensiones) {
        int size = 1;
        for (int d : dimensiones) size *= d;

        vector<double> valores;
        for (int i = 0; i < size; i++) {
            valores.push_back(0.0);
        }
        return Tensor(dimensiones, valores);
    }

    static Tensor unos(const vector<int>& dimensiones) {
        int size = 1;
        for (int d : dimensiones) size *= d;

        vector<double> valores;
        for (int i = 0; i < size; i++) {
            valores.push_back(1.0);
        }
        return Tensor(dimensiones, valores);
    }

    static Tensor aleatorio(const vector<int>& dimensiones, double minimo, double maximo) {
        int size = 1;
        for (int d : dimensiones) size *= d;

        vector<double> valores;
        for (int i = 0; i < size; i++) {
            double aleatorio = minimo + (double)rand() / RAND_MAX * (maximo - minimo);
            valores.push_back(aleatorio);
        }
        return Tensor(dimensiones, valores);
    }

    static Tensor rango(double inicio, double fin, double paso = 1.0) {
        if (paso <= 0) {
            cout << "Error: El paso debe ser positivo" << endl;
            return Tensor({0}, {0});
        }

        vector<double> valores;
        for (double v = inicio; v < fin; v += paso) {
            valores.push_back(v);
        }

        return Tensor({(int)valores.size()}, valores);
    }

    // PARTE 6

    // Operador + (tensor + tensor)
    Tensor operator+(const Tensor& otro) const {
        if (dimensiones.size() != otro.dimensiones.size()) {
            cout << "Error: Dimensiones incompatibles para suma" << endl;
            return Tensor();
        }

        for (int i = 0; i < (int)dimensiones.size(); i++) {
            if (dimensiones[i] != otro.dimensiones[i]) {
                cout << "Error: Dimensiones incompatibles para suma" << endl;
                return Tensor();
            }
        }

        vector<double> valores_resultado;
        for (int i = 0; i < tamanio_total; i++) {
            valores_resultado.push_back(datos[i] + otro.datos[i]);
        }

        return Tensor(dimensiones, valores_resultado);
    }

    // Operador - (tensor - tensor)
    Tensor operator-(const Tensor& otro) const {
        if (dimensiones.size() != otro.dimensiones.size()) {
            cout << "Error: Dimensiones incompatibles para resta" << endl;
            return Tensor();
        }

        for (int i = 0; i < (int)dimensiones.size(); i++) {
            if (dimensiones[i] != otro.dimensiones[i]) {
                cout << "Error: Dimensiones incompatibles para resta" << endl;
                return Tensor();
            }
        }

        vector<double> valores_resultado;
        for (int i = 0; i < tamanio_total; i++) {
            valores_resultado.push_back(datos[i] - otro.datos[i]);
        }

        return Tensor(dimensiones, valores_resultado);
    }

    // Operador * (tensor * tensor) - multiplicación elemento a elemento
    Tensor operator*(const Tensor& otro) const {
        if (dimensiones.size() != otro.dimensiones.size()) {
            cout << "Error: Dimensiones incompatibles para multiplicacion" << endl;
            return Tensor();
        }

        for (int i = 0; i < (int)dimensiones.size(); i++) {
            if (dimensiones[i] != otro.dimensiones[i]) {
                cout << "Error: Dimensiones incompatibles para multiplicacion" << endl;
                return Tensor();
            }
        }

        vector<double> valores_resultado;
        for (int i = 0; i < tamanio_total; i++) {
            valores_resultado.push_back(datos[i] * otro.datos[i]);
        }

        return Tensor(dimensiones, valores_resultado);
    }

    // Operador * (tensor * escalar)
    Tensor operator*(double escalar) const {
        vector<double> valores_resultado;
        for (int i = 0; i < tamanio_total; i++) {
            valores_resultado.push_back(datos[i] * escalar);
        }
        return Tensor(dimensiones, valores_resultado);
    }

    // PARTE 7

    // 7.1 view - reinterpretar la forma sin copiar datos
    Tensor view(const vector<int>& nuevas_dimensiones) const {
        int nuevo_tamanio = 1;
        for (int d : nuevas_dimensiones) nuevo_tamanio *= d;

        if (nuevo_tamanio != tamanio_total) {
            cout << "Error: Las nuevas dimensiones deben tener la misma cantidad de elementos" << endl;
            return Tensor();
        }

        if (nuevas_dimensiones.size() > 3) {
            cout << "Error: View no puede tener mas de 3 dimensiones" << endl;
            return Tensor();
        }

        // Retorna un tensor que NO es dueño de los datos (vista)
        return Tensor(nuevas_dimensiones, datos, false);
    }

    // 7.2 unsqueeze - agregar una dimensión de tamaño 1
    Tensor unsqueeze(int posicion) const {
        if (posicion < 0 || posicion > (int)dimensiones.size()) {
            cout << "Error: Posicion invalida para unsqueeze" << endl;
            return Tensor();
        }

        vector<int> nuevas_dimensiones = dimensiones;
        nuevas_dimensiones.insert(nuevas_dimensiones.begin() + posicion, 1);

        if (nuevas_dimensiones.size() > 3) {
            cout << "Error: Unsqueeze excederia las 3 dimensiones" << endl;
            return Tensor();
        }

        // Retorna un tensor que NO es dueño de los datos (vista)
        return Tensor(nuevas_dimensiones, datos, false);
    }

    // ============ PARTE 8: CONCATENACIÓN ============

    // Metodo estático concat - une múltiples tensores a lo largo de una dimensión
    static Tensor concat(const vector<Tensor>& tensores, int dim) {
        if (tensores.empty()) {
            cout << "Error: No hay tensores para concatenar" << endl;
            return Tensor();
        }

        int num_dimensiones = tensores[0].dimensiones.size();

        if (dim < 0 || dim >= num_dimensiones) {
            cout << "Error: Dimension de concatenacion fuera de rango" << endl;
            return Tensor();
        }

        // Validar que todos los tensores tengan el mismo número de dimensiones
        // y dimensiones compatibles (excepto en la dimensión de concatenación)
        for (int i = 1; i < (int)tensores.size(); i++) {
            if (tensores[i].dimensiones.size() != num_dimensiones) {
                cout << "Error: Todos los tensores deben tener el mismo numero de dimensiones" << endl;
                return Tensor();
            }

            for (int d = 0; d < num_dimensiones; d++) {
                if (d != dim && tensores[i].dimensiones[d] != tensores[0].dimensiones[d]) {
                    cout << "Error: Dimensiones incompatibles para concatenacion" << endl;
                    return Tensor();
                }
            }
        }

        // Calcular la nueva dimensión
        vector<int> nueva_forma = tensores[0].dimensiones;
        int tamanio_concat = 0;
        for (int i = 0; i < (int)tensores.size(); i++) {
            tamanio_concat += tensores[i].dimensiones[dim];
        }
        nueva_forma[dim] = tamanio_concat;

        // Calcular el tamaño total del nuevo tensor
        int nuevo_tamanio_total = 1;
        for (int d : nueva_forma) nuevo_tamanio_total *= d;

        // Crear vector para almacenar los datos concatenados
        vector<double> nuevos_datos(nuevo_tamanio_total);

        // Calcular el tamaño de cada bloque
        int bloque_tamanio = 1;
        for (int d = dim + 1; d < num_dimensiones; d++) {
            bloque_tamanio *= tensores[0].dimensiones[d];
        }

        // Copiar datos de cada tensor
        int offset = 0;
        for (int t = 0; t < (int)tensores.size(); t++) {
            int elementos_a_copiar = tensores[t].tamanio_total;
            for (int i = 0; i < elementos_a_copiar; i++) {
                nuevos_datos[offset + i] = tensores[t].datos[i];
            }
            offset += elementos_a_copiar;
        }

        return Tensor(nueva_forma, nuevos_datos);
    }

    // ============ MÉTODOS AUXILIARES ============

    void imprimir() const {
        cout << "Dimensiones: [";
        for (int i = 0; i < (int)dimensiones.size(); i++) {
            cout << dimensiones[i];
            if (i != (int)dimensiones.size() - 1) cout << ", ";
        }
        cout << "], Datos: ";

        if (datos != nullptr) {
            for (int i = 0; i < tamanio_total; i++) {
                cout << datos[i];
                if (i != tamanio_total - 1) cout << ", ";
            }
        }
        cout << endl;
    }

    vector<int> get_dimensiones() const { return dimensiones; }
    int get_tamanio() const { return tamanio_total; }

//--------------------------------------parte 5---------------------
    Tensor apply ( const TensorTransform & transform ) const {
        return transform.apply(*this);
    };
};


//----------------------------------------parte 5---------------------------


class ReLu : public TensorTransform {

public:
    Tensor apply ( const Tensor & t ) const override {

        Tensor res = t;
        for (int i = 0; i< res.tamanio_total;i++) {
            res.datos[i] = max(0.0, res.datos[i]);
        }
        return res;
    };
};
class Sigmoid : public TensorTransform {

public:
    Tensor apply ( const Tensor & t ) const override {

        Tensor res = t;
        for (int i = 0; i< res.tamanio_total;i++) {
            res.datos[i] = 1/(1.0 + exp(-res.datos[i]));
        }
        return res;
    };
};

// Operador * (escalar * tensor) - función externa
Tensor operator*(double escalar, const Tensor& tensor) {
    return tensor * escalar;
}
//-------------------------------------parte 9------------------------------

Tensor dot(const Tensor& t1, const Tensor& t2) {

    int d = 1;
    for (int v: t1.dimensiones) d*=v;

    int d2 = 1;
    for (int v: t2.dimensiones) d2*=v;

    double p_p = 0;
    if (d != d2) {
        cout << "No se puede realizar la operacion, porque no hay compatibilidad de dimensiones."<<endl;
        return Tensor();
    }

    for (int v = 0; v < d; v++) {
        p_p += t1.datos[v]*t2.datos[v];
    }

    return Tensor({1},{p_p});

}

Tensor matmul(const Tensor& t1, const Tensor& t2) {
    if (t1.dimensiones.size() != 2 || t2.dimensiones.size() !=2) {
        cout<< "No se puede realizar la multiplicación bidimensional"<<endl;
        return Tensor();
    }
    if (t1.dimensiones[1] != t2.dimensiones[0]) {
        cout <<"No cumples los requisitos para la multiplicacion bidimensional :("<< endl;
        return Tensor();
    }

    int dim = t1.dimensiones[0]*t2.dimensiones[1];
    vector<double> valores_nuevo;
    for (int i = 0; i< t1.dimensiones[0];i++) {
        for (int j = 0; j< t2.dimensiones[1];j++) {
            double sum = 0 ;
            for (int k = 0; k< t1.dimensiones[1];k++) {
                sum += t1.datos[i*t1.dimensiones[1]+k] * t2.datos[k*t2.dimensiones[1]+j];
            }
            valores_nuevo.push_back(sum);
        }
    }

    return Tensor({t1.dimensiones[0], t2.dimensiones[1]}, valores_nuevo);

}



//------------------------------------------------------------------------------------------------------------------
int main() {
    srand(time(0));

    cout << "========== PARTE 3, 6, 7 y 8 ==========\n" << endl;

    // ============ PARTE 3: Creación de tensores ============
    cout << "=== PARTE 3: Creacion de tensores ===" << endl;
    Tensor A = Tensor::ceros({2, 3});
    cout << "Tensor::ceros({2, 3}): ";
    A.imprimir();

    Tensor B = Tensor::unos({3, 3});
    cout << "Tensor::unos({3, 3}): ";
    B.imprimir();

    Tensor C = Tensor::aleatorio({2, 2}, 0.0, 1.0);
    cout << "Tensor::aleatorio({2, 2}, 0, 1): ";
    C.imprimir();

    Tensor D = Tensor::rango(0, 12);
    cout << "Tensor::rango(0, 12): ";
    D.imprimir();
    cout << endl;

    // ============ PARTE 6: Operadores ============
    cout << "=== PARTE 6: Sobrecarga de operadores ===" << endl;
    Tensor E({2, 2}, {1.0, 2.0, 3.0, 4.0});
    Tensor F({2, 2}, {5.0, 6.0, 7.0, 8.0});
    cout << "E: ";
    E.imprimir();
    cout << "F: ";
    F.imprimir();

    Tensor G = E + F;
    cout << "E + F: ";
    G.imprimir();

    Tensor H = E * F;
    cout << "E * F (elemento a elemento): ";
    H.imprimir();

    Tensor I = E * 2.0;
    cout << "E * 2.0: ";
    I.imprimir();

    Tensor J = 3.0 * E;
    cout << "3.0 * E: ";
    J.imprimir();
    cout << endl;

    // ============ PARTE 7: view y unsqueeze ============
    cout << "=== PARTE 7: view y unsqueeze ===" << endl;
    Tensor K = Tensor::rango(0, 12);
    cout << "Tensor original (1D): ";
    K.imprimir();

    Tensor L = K.view({3, 4});
    cout << "Despues de view({3, 4}): ";
    L.imprimir();

    Tensor M = K.view({2, 2, 3});
    cout << "Despues de view({2, 2, 3}): ";
    M.imprimir();

    Tensor N = K.unsqueeze(0);
    cout << "Despues de unsqueeze(0): ";
    N.imprimir();

    Tensor O = K.unsqueeze(1);
    cout << "Despues de unsqueeze(1): ";
    O.imprimir();
    cout << endl;

    // ============ PARTE 8: concat ============
    cout << "=== PARTE 8: Concatenacion ===" << endl;
    Tensor P = Tensor::unos({2, 3});
    Tensor Q = Tensor::ceros({2, 3});
    Tensor R = Tensor::rango(0, 6).view({2, 3});

    cout << "P (unos 2x3): ";
    P.imprimir();
    cout << "Q (ceros 2x3): ";
    Q.imprimir();
    cout << "R (rango 0-5 en 2x3): ";
    R.imprimir();

    Tensor S = Tensor::concat({P, Q}, 0);
    cout << "concat({P, Q}, 0) - vertical: ";
    S.imprimir();

    Tensor T = Tensor::concat({P, R}, 1);
    cout << "concat({P, R}, 1) - horizontal: ";
    T.imprimir();

    Tensor U = Tensor::concat({P, Q, R}, 0);
    cout << "concat({P, Q, R}, 0) - tres tensores: ";
    U.imprimir();

    // Demostrar que view NO copia datos (modificar original afecta la vista)
    cout << "\n=== Demostracion de que view NO copia datos ===" << endl;
    Tensor V = Tensor::rango(0, 6);
    cout << "Original V: ";
    V.imprimir();

    Tensor W = V.view({2, 3});
    cout << "Vista W (view): ";
    W.imprimir();

    // Nota: Como la vista no es dueña de los datos, no podemos modificar fácilmente
    // pero conceptualmente comparten la misma memoria
    cout << "(V y W comparten la misma memoria - no se copian los datos)" << endl;

    cout<< "-------------------------------------------Constructores y asignaciones-----------------------------------------------\n";
    E.imprimir();
    Tensor Lc = E;
    cout << "copia"<<endl;
    cout << "Lc\n";
    Lc.imprimir();
    cout << "E\n";
    E.imprimir();
    Tensor Lm = move(Lc);
    cout << "move (Lm)\n";
    Lm.imprimir();
    cout <<"Lc\n";
    Lc.imprimir();
    cout<<"-----------------------------------------------------Polimorfismo-----------------------\n";

    ReLu prueba;
    cout<<"E"<<endl;
    E.imprimir();
    Tensor p_relu = E.apply(prueba);
    cout<<"Prueba de relu"<<endl;
    p_relu.imprimir();
    Sigmoid prueba_2;
    cout<<"Utilizamos el tensor de relu para sigmoid"<<endl;
    Tensor p_sigmoid = p_relu.apply(prueba_2);
    p_sigmoid.imprimir();

    cout << "-------------------------------Funciones amigas dot & matmul---------------------------------------------------\n";
    cout <<"dot\n";
    Tensor Z = dot(E,F);
    Z.imprimir();
    cout <<"\n";
    cout <<"matmul\n";
    Tensor X = matmul(E,F);
    X.imprimir();


    Tensor red = Tensor ::aleatorio({1000,20,20},0,100);
    Tensor red_Transformado = red.view({1000,400});
    Tensor matri = Tensor ::aleatorio({400,100},0,100);
    Tensor red_mul = matmul(red_Transformado,matri);
    Tensor suma = Tensor ::aleatorio({1,100},0,100);
    Tensor red_suma = red_mul + suma;
    ReLu relu;
    Tensor re_lu = red_suma.apply(relu);
    Tensor matri2 = Tensor ::aleatorio({100,10},0,100);
    Tensor multi2 = matmul(re_lu , matri2);
    Tensor suma2 = Tensor::aleatorio({1,10},0,100);
    Tensor red_sum2 = multi2  + suma2;
    Sigmoid sigmoid_;
    Tensor sigmoid = red_sum2.apply(sigmoid_);

    return 0;
}