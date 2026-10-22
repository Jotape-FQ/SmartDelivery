#include <iostream>
#include <string>
#include <vector>
    using namespace std;
//Creamos la clase madre NodoBase
class NodoBase {
};
//Creamos la clase madre estructura
class Estructura {
public:
    int contador;
    Estructura() : contador(0) {}
    //tamano, devuelve el tamaño de la pila
    int tamano() {
        return contador;
    }
};
//Creamos Nodoestado, heredando nodo, para usarlo en Historial, es un estado del paquete en la pila
class NodoEstado : public NodoBase {
public:
    NodoEstado* siguiente;
    string estado;
    NodoEstado(const string& est): estado(est), siguiente(nullptr) {}
};
//Implementamos la Pila creando un Historial para cada Paquete
class HistorialEstados : public Estructura {
private:
    //Solo historial puede acceder a la direccion del tope de la pila y el contador
    NodoEstado* tope;
public:
    //El codigo completo puede acceder a las funciones
    //Constructor, no recibe argumentos, y establece la pila
    HistorialEstados() : tope(nullptr) {}
    //Destructor, no recibe argumentos, y libera la memoria de la pila
    ~HistorialEstados() {
        while (!esVacia()) {
            pop();
        }
    }
    //Push, permite colocar otro elemento encima de la pila
    void push(const string& estado) {
        NodoEstado* nuevo = new NodoEstado(estado);
        nuevo->siguiente = tope;
        tope = nuevo;
        contador++;
    }
    //Pop, permite eliminar el elemento que se encuentra encima de la pila
    string pop() {
        if (esVacia()) {
            return "";
        }
        NodoEstado* aux = tope;
        string estadoEliminado = aux->estado;
        tope = tope->siguiente;
        delete aux;
        contador--;
        return estadoEliminado;
    }
    //peek, permite ver el elemento que se encuentra encima de la pila
    string peek() const {
        if (esVacia()) {
            return "";
        }
        return tope->estado;
    }
    //esvacia, revisa si la pila esta vacia
    bool esVacia() const {
        return tope == nullptr;
    }
};
//Creamos el plano para la creacion de cada paquete como objeto
class Paquete {
public:
    //se establece una identificacion, un cliente, una direccion, y un historial para cada paquete
    string idPaquete;
    string cliente;
    string direccion;
    HistorialEstados historial;
    //el constructor recibe los argumentos idPaquete, cliente y direccion y construye el paquete
    Paquete(string id, string clie, string dire):
        idPaquete(id), cliente(clie), direccion(dire) {
        cambiarEstado("Registrado");
    }
    //cambiarEstado, permite guardar un estado en el historial del paquete
    void cambiarEstado(const string& nuevoEstado) {
        historial.push(nuevoEstado);
    }
    //deshacerEstado, permite eliminar un estado en el historial del paquete, mostrando el estado anterior como el actual
    bool deshacerEstado() {
        if (historial.tamano() > 1) {
            string estadoNo = historial.pop();
            cout << "-> Estado " << estadoNo
                 << " revertido. Estado actual: "
                 << obtenerEstadoActual() << endl;
            return true;
        }
        cout << "No hay estados anteriores para deshacer" << endl;
        return false;
    }
    //obtenerEstadoactual, permite ver el estado actual en el historial de un paquete
    string obtenerEstadoActual() const {
        return historial.peek();
    }
};
//Creamos NodoSolicitud, heredando nodo, y usando Paquete para usarlo en Cola de Solicitudes
class NodoSolicitud : public NodoBase {
public:
    NodoSolicitud* siguiente;
    Paquete* paquete;
    NodoSolicitud(Paquete* p): paquete(p), siguiente(nullptr){}
};
//Implementamos la Cola creando el orden de atencion para las solicitudes
class ColaSolicitudes : public Estructura{
private:
    NodoSolicitud* final;
    NodoSolicitud* inicio;
public:
    ColaSolicitudes() : final(nullptr), inicio(nullptr){}
    ~ColaSolicitudes(){
        while(!esVacia()){
            pop();
        }
    }
    void push(Paquete* p){
        NodoSolicitud* nuevo = new NodoSolicitud(p);
        if(esVacia()){
            inicio = final = nuevo;
        }
        else{
            final->siguiente = nuevo;
            final = nuevo;
        }
        contador++;
        cout << "Paquete " << p->idPaquete << " agregado a la cola de atencion." << endl;
    }
    Paquete* pop(){
        if(esVacia()){
            return nullptr;
        }
        NodoSolicitud* aux = inicio;
        Paquete* PaqueteNo = aux->paquete;
        inicio = inicio->siguiente;
        if (inicio == nullptr){
            final = nullptr;
        }
        delete aux;
        contador--;
        return PaqueteNo;
    }
    void peek(){
        if(esVacia()){
            cout<<"La cola esta vacia"<<endl;
            return;
        }
        cout<<"ID: "<< inicio->paquete->idPaquete<<". Cliente: "<< inicio->paquete->cliente<<endl;
    }
    bool esVacia() const {
        return inicio == nullptr;
    }
};
class ArregloDinamicoPaquetes : public Estructura {
private:
    vector<Paquete*> arreglo;
public:
    ArregloDinamicoPaquetes() {}
    void agregar(Paquete* p) {
        arreglo.push_back(p);
        contador++;
    }
    Paquete* buscarPorId(const string& id) {
        for (Paquete* p : arreglo) {
            if (p && p->idPaquete == id) {
                return p;
            }
        }
        return nullptr;
    }
    const vector<Paquete*>& obtenerTodos() const {
        return arreglo;
    }
};
//Creamos NodoRuta, heredando nodo, para usarlo en la Ruta de Transporte, es un paquete en la ruta de transporte
class NodoRuta : public NodoBase {
public:
    NodoRuta* siguiente;
    NodoRuta* anterior;
    Paquete* paquete;
    NodoRuta(Paquete* p): siguiente(nullptr), anterior(nullptr), paquete(p){}
};
//Implementamos la lista doblemente enlazada para crear la ruta del transporte
class RutaTransporte : public Estructura{
private:
    NodoRuta* cabeza;
    NodoRuta* cola;
public:
    RutaTransporte(): cabeza(nullptr), cola(nullptr){}
    ~RutaTransporte() {
        NodoRuta* actual = cabeza;
        while (actual != nullptr){
            NodoRuta* sig = actual->siguiente;
            delete actual;
            actual = sig;
        }
    }
    void push(Paquete* p){
        NodoRuta* nuevo = new NodoRuta(p);
        if(cabeza== nullptr){
            cabeza= cola = nuevo;
        }
        else{
            cola->siguiente=nuevo;
            nuevo->anterior=cola;
            cola= nuevo;
        }
        contador++;
        p->cambiarEstado("Asignado a Ruta de Transporte");
    }
    void mostrarRuta1() const{
        if (cabeza == nullptr){
            cout << "La ruta de transporte esta vacia." << endl;
            return;
        }
        cout << "\n--- RUTA DE TRANSPORTE (Inicio -> Fin) ---" << endl;
        NodoRuta* actual = cabeza;
        while (actual != nullptr){
            cout << "[" << actual->paquete->idPaquete << " - "
                 << actual->paquete->cliente << " - "
                 << actual->paquete->obtenerEstadoActual() << "] <-> ";
            actual = actual->siguiente;
        }
        cout<< "Fin" <<endl;
    }
    void mostrarRuta2() const{
        if (cabeza == nullptr){
            cout << "La ruta de transporte esta vacia." << endl;
            return;
        }
        cout << "\n--- RUTA DE TRANSPORTE (Fin -> Inicio) ---" << endl;
        NodoRuta* actual = cola;
        while (actual != nullptr){
            cout << "[" << actual->paquete->idPaquete << " - "
                 << actual->paquete->cliente << " - "
                 << actual->paquete->obtenerEstadoActual() << "] <-> ";
            actual = actual->anterior;
        }
        cout<< "Inicio" <<endl;
    }
    Paquete* buscar(const string& id){
        NodoRuta* actual= cabeza;
        while(actual!=nullptr){
            if(actual->paquete->idPaquete == id){
                return actual->paquete;
            }
            actual = actual->siguiente;
        }
        return nullptr;
    }
    bool esVacia() const {
        return cabeza == nullptr;
    }
};