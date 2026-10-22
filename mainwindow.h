#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "backend.h"
#include <QHash>
#include <functional>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:

    // Botones del INICIO y MENU_PRINCIPAL
    void on_btnEmpezar_clicked();
    void on_btnRegistrar_clicked();
    void on_btnVerPrioridad_clicked();
    void on_btnDespacharPrioridad_clicked();
    void on_btnVerOrdenRuta_clicked();
    void on_btnCambiarEstado_clicked();
    void on_btnConsultarEstado_clicked();
    void on_btnRevertirEstado_clicked();
    void on_btnCerrarSesion_clicked();

    // Botones registro
    void on_btnLimpiar_clicked();
    void on_btnGuardar_clicked();
    void on_btnFinalizar_clicked();

    // Botones VerPrioridad
    void on_btnVolverTope_clicked();

    // Botones DespacharPrioridad
    void on_btnDespachar_clicked();
    void on_btnVolverDespacho_clicked();

    // Botones VerOrdenRuta
    void on_btnRutaInicioFin_clicked();
    void on_btnRutaFinInicio_clicked();
    void on_btnVolverRuta_clicked();

    // Botones Cambiar, Consultar y Revertir Estado
    void on_btnConfirmarCambio_clicked();
    void on_btnVolverCambio_clicked();
    void on_btnConfirmarConsulta_clicked();
    void on_btnVolverConsulta_clicked();
    void on_btnConfirmarRevertir_clicked();
    void on_btnVolverRevertir_clicked();

private:
    Ui::MainWindow *ui;

    // Nombres para los indices de las paginas (mismo orden que en el diseñador)
    enum Pagina {
        INICIO,
        MENU_PRINCIPAL,
        REGISTRO,
        VER_PRIORIDAD,
        DESPACHO,
        ORDEN_RUTA,
        CAMBIAR_ESTADO,
        CONSULTAR_ESTADO,
        REVERTIR_ESTADO
    };
    ColaSolicitudes colaSolicitudes;
    RutaTransporte ruta;

    int contadorPaquetes = 0;
    ArregloDinamicoPaquetes registro;

    QString crearId(int numero) const;
    void mostrarSiguienteId();
    void irAlMenu();
    void mostrarPaquete(Paquete *p, QLabel *lblId, QLabel *lblCliente, QLabel *lblDireccion);
    void mostrarRuta(bool inicioAFin);
    Paquete* obtenerTope();
    Paquete* buscarEnRuta(const QString &id, QLabel *lblResultado);
    QString capturarSalida(const std::function<void()> &action);
    void liberarMemoria();
};
#endif // MAINWINDOW_H