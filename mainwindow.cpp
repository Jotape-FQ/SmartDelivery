// ============================================================================
// PROYECTO: SMARTDELIVERY
// CÓDIGO GENERADO POR IA: 35% asistido por IA / 65% desarrollo propio
// ============================================================================

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QLabel>
#include <QRegularExpression>
#include <QStringList>
#include <iostream>
#include <sstream>


// =========== FUNCIONES AUXILIARES ===========

QString MainWindow::crearId(int numero) const
{
    return "PKG-" + QString::number(numero);
}

void MainWindow::mostrarSiguienteId()
{
    ui->lblIdPaquete->setText(crearId(contadorPaquetes + 1));
}

void MainWindow::irAlMenu()
{
    ui->stackedWidget->setCurrentIndex(MENU_PRINCIPAL);
}

void MainWindow::mostrarPaquete(Paquete *p, QLabel *lblId, QLabel *lblCliente, QLabel *lblDireccion)
{
    if (p == nullptr){
        lblId->setText("-");
        lblCliente->setText("-");
        lblDireccion->setText("-");
        return;
    }
    lblId->setText(QString::fromStdString(p->idPaquete));
    lblCliente->setText(QString::fromStdString(p->cliente));
    lblDireccion->setText(QString::fromStdString(p->direccion));
}

QString MainWindow::capturarSalida(const std::function<void()> &accion)
{
    std::ostringstream buffer;
    std::streambuf *original = std::cout.rdbuf(buffer.rdbuf());  //desviar cout al bufer
    accion();
    std::cout.rdbuf(original);  //restaurar
    return QString::fromStdString(buffer.str());
}

// Llama a mostrarRuta1() o mostrarRuta2() y reparte su texto en la lista
void MainWindow::mostrarRuta(bool inicioAFin)
{
    QString texto = capturarSalida([&]() {
        if (inicioAFin) {
            ruta.mostrarRuta1();
        } else {
            ruta.mostrarRuta2();
        }
    });

    ui->lblRutaActual->setText(inicioAFin ? "Mostrando: Inicio -> Fin" : "Mostrando: Fin -> Inicio");
    ui->lstRuta->clear();

    const QStringList lineas = texto.split('\n', Qt::SkipEmptyParts);
    for (const QString &linea : lineas){
        if(linea.startsWith("[")){
            // "[PKG-1 - ...] <-> [PKG-2 - ...] <-> Fin": un item por nodo
            const QStringList partes = linea.split(" <-> ");
            for (const QString &parte : partes){
                ui->lstRuta->addItem(parte.trimmed());
            }
        } else {
            ui->lstRuta->addItem(linea.trimmed());
        }
    }
}

//Pregunta al backend (peek) cual es el primero de la cola y busca sus datos
Paquete* MainWindow::obtenerTope()
{
    if (colaSolicitudes.esVacia()){
        return nullptr;
    }
    //peek() imprime algo como "ID: PKG-1. Cliente: Juan"
    QString salida = capturarSalida([&]() {colaSolicitudes.peek();});

    QRegularExpression patron("ID: (.+?)\\. Cliente:");
    QRegularExpressionMatch encontrado = patron.match(salida);
    if(!encontrado.hasMatch()){
        return nullptr;
    }
    return registro.buscarPorId(encontrado.captured(1).trimmed().toStdString());
}

//Busca un paquete en la ruta por el ID escrito; si falla, avisa en el label
Paquete* MainWindow::buscarEnRuta(const QString &id, QLabel *lblResultado)
{
    QString limpio = id.trimmed().toUpper();  //buscar() distingue mayusculas
    if(limpio.isEmpty()){
        lblResultado->setText("Escribe el ID del paquete.");
        return nullptr;
    }
    Paquete *p = ruta.buscar(limpio.toStdString());
    if(p == nullptr){
        lblResultado->setText("Paquete no encontrado en la ruta.");
    }
    return p;
}

// constructor y destructor
void MainWindow::liberarMemoria()
{
    // Todos los paquetes estan en 'registro' (Arreglo Dinamico). La cola y la ruta solo borran sus nodos
    for (Paquete* p : registro.obtenerTodos()) {
        delete p;
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(INICIO);
    mostrarSiguienteId();
}

MainWindow::~MainWindow()
{
    liberarMemoria();
    delete ui;
}

// ============ INICIO ============

void MainWindow::on_btnEmpezar_clicked()
{
    irAlMenu();
}

// ============ MENU PRINCIPAL ============

void MainWindow::on_btnRegistrar_clicked()
{
    mostrarSiguienteId();
    ui->lblMensajeIngreso->clear();
    ui->stackedWidget->setCurrentIndex(REGISTRO);
    ui->txtCliente->setFocus();
}

void MainWindow::on_btnVerPrioridad_clicked()
{
    Paquete *p = obtenerTope();
    if(p == nullptr){
        QMessageBox::information(this, "Cola vacía", "No hay paquetes en la cola de prioridad.");
        return;
    }
    mostrarPaquete(p, ui->lblTopeId, ui->lblTopeCliente, ui->lblTopeDireccion);
    ui->stackedWidget->setCurrentIndex(VER_PRIORIDAD);
}

void MainWindow::on_btnDespacharPrioridad_clicked()
{
    Paquete *p = obtenerTope();
    if (p == nullptr){
        QMessageBox::information(this, "Cola vacia", "No hay solicitudes pendeintes en cola");
        return;
    }
    mostrarPaquete(p, ui->lblDespachoId, ui->lblDespachoCliente, ui->lblDespachoDireccion);
    ui->stackedWidget->setCurrentIndex(DESPACHO);
}

void MainWindow::on_btnVerOrdenRuta_clicked()
{
    if (ruta.esVacia()){
        QMessageBox::information(this, "Ruta vacia", "La ruta de transporte esta vacia. Despacha un paquete primero");
        return;
    }
    mostrarRuta(true);
    ui->stackedWidget->setCurrentIndex(ORDEN_RUTA);
}

void MainWindow::on_btnCambiarEstado_clicked()
{
    if (ruta.esVacia()){
        QMessageBox::information(this, "Ruta vacia", "La ruta esta vacia. Despacha un paquete primero.");
        return;
    }
    ui->txtIdCambiar->clear();
    ui->txtNuevoEstado->clear();
    ui->lblResultadoCambiar->clear();
    ui->stackedWidget->setCurrentIndex(CAMBIAR_ESTADO);
    ui->txtIdCambiar->setFocus();
}

void MainWindow::on_btnConsultarEstado_clicked()
{
    if(ruta.esVacia()){
        QMessageBox::information(this, "Ruta vacia", "La ruta esta vacia. Despacha un paquete primero.");
        return;
    }
    ui->txtIdConsultar->clear();
    ui->lblResultadoConsultar->clear();
    ui->stackedWidget->setCurrentIndex(CONSULTAR_ESTADO);
    ui->txtIdConsultar->setFocus();
}

void MainWindow::on_btnRevertirEstado_clicked()
{
    if (ruta.esVacia()){
        QMessageBox::information(this, "Ruta vacia", "La ruta esta vacia. Despacha un paquete primero.");
        return;
    }
    ui->txtIdRevertir->clear();
    ui->lblResultadoRevertir->clear();
    ui->stackedWidget->setCurrentIndex(REVERTIR_ESTADO);
    ui->txtIdRevertir->setFocus();
}

void MainWindow::on_btnCerrarSesion_clicked()
{
    ui->stackedWidget->setCurrentIndex(INICIO);
}

// ============ REGISTRAR PAQUETE ============

void MainWindow::on_btnLimpiar_clicked()
{
    ui->txtCliente->clear();
    ui->txtDireccion->clear();
    ui->txtCliente->setFocus();
    ui->lblMensajeIngreso->setText("Datos borrados. El paquete no se guardo.");
}

void MainWindow::on_btnGuardar_clicked()
{
    QString cliente = ui->txtCliente->text().trimmed();
    QString direccion = ui->txtDireccion->text().trimmed();

    if (cliente.isEmpty() || direccion.isEmpty()){
        ui->lblMensajeIngreso->setText("Completa el cliente y la direccion.");
        return;
    }

    contadorPaquetes++;
    QString id = crearId(contadorPaquetes);

    Paquete *nuevo = new Paquete(id.toStdString(), cliente.toStdString(), direccion.toStdString());
    colaSolicitudes.push(nuevo);   //Entra al final de la cola
    registro.agregar(nuevo);       //Se guarda en el Arreglo Dinamico

    ui->lblMensajeIngreso->setText(id + " guardado. En cola: " + QString::number(colaSolicitudes.tamano()));
    ui->txtCliente->clear();
    ui->txtDireccion->clear();
    ui->txtCliente->setFocus();
    mostrarSiguienteId();
}

void MainWindow::on_btnFinalizar_clicked()
{
    irAlMenu();
}

// ============ CONSULTAR TOPE ============

void MainWindow::on_btnVolverTope_clicked()
{
    irAlMenu();
}

// ============ DESPACHO ============

void MainWindow::on_btnDespachar_clicked()
{
    Paquete *p = colaSolicitudes.pop();         //Sale el primero en llegar
    if(p == nullptr){
        irAlMenu();
        return;
    }
    ruta.push(p);         //Entra al final de la lista doble

    QMessageBox::information(this, "Despachado", QString::fromStdString(p->idPaquete) + "Fue despachado ala ruta de trasporte");

    if(colaSolicitudes.esVacia()){
        QMessageBox::information(this, "Cola vacia", "Ya no quedan paquetes en la cola.");
        irAlMenu();
    } else {
        mostrarPaquete(obtenerTope(), ui->lblDespachoId, ui->lblDespachoCliente, ui->lblDespachoDireccion);
    }
}

void MainWindow::on_btnVolverDespacho_clicked()
{
    irAlMenu();
}

// ============ ORDEN DE LA RUTA ============

void MainWindow::on_btnRutaInicioFin_clicked()
{
    mostrarRuta(true);
}

void MainWindow::on_btnRutaFinInicio_clicked()
{
    mostrarRuta(false);
}

void MainWindow::on_btnVolverRuta_clicked()
{
    irAlMenu();
}

// ============ CAMBIAR ESTADO ============

void MainWindow::on_btnConfirmarCambio_clicked()
{
    Paquete *p = buscarEnRuta(ui->txtIdCambiar->text(), ui->lblResultadoCambiar);
    if(p == nullptr){
        return;
    }
    QString nuevo = ui->txtNuevoEstado->text().trimmed();
    if(nuevo.isEmpty()){
        ui->lblResultadoCambiar->setText("Escribe el nuevo estado.");
        return;
    }
    p->cambiarEstado(nuevo.toStdString());   //Push a la pila de estados
    ui->lblResultadoCambiar->setText("Estado actualizado correctamente.");
    ui->txtNuevoEstado->clear();
}

void MainWindow::on_btnVolverCambio_clicked()
{
    irAlMenu();
}

// ============ CONSULTAR ESTADO ============

void MainWindow::on_btnConfirmarConsulta_clicked()
{
    Paquete *p = buscarEnRuta(ui->txtIdConsultar->text(), ui->lblResultadoConsultar);
    if(p==nullptr){
        return;
    }
    ui->lblResultadoConsultar->setText("El paquete " + QString::fromStdString(p->idPaquete) + "tiene estado: " + QString::fromStdString(p->obtenerEstadoActual()));
}

void MainWindow::on_btnVolverConsulta_clicked()
{
    irAlMenu();
}

// ============ REVERTIR ESTADO ============

void MainWindow::on_btnConfirmarRevertir_clicked()
{
    Paquete *p = buscarEnRuta(ui->txtIdRevertir->text(), ui->lblResultadoRevertir);
    if(p == nullptr){
        return;
    }
    //deshacerEstado() imprime su mensaje en un cout
    QString mensaje = capturarSalida([&]() {p->deshacerEstado();});  //pop a la pila
    ui->lblResultadoRevertir->setText(mensaje.trimmed());
}

void MainWindow::on_btnVolverRevertir_clicked()
{
    irAlMenu();
}