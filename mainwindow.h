#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QDial>
#include <QTimer>
#include <QCheckBox>
#include <QJsonObject>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

QT_BEGIN_NAMESPACE
namespace Ui {class MainWindow; }
QT_END_NAMESPACE

class ControlTanque:public QObject {
    Q_OBJECT
public:
    explicit ControlTanque(QProgressBar* bar,QObject*parent=nullptr);

    void setCapacityL(double L);
    void setInputMaxLph(double Lph);
    void setOutputMaxLph(double Lph);
    double getInputMaxLph()const;
    double getOutputMaxLph()const;
    double capacityL()const;
    double currentInputLph()const;
    double currentOutputLph()const;
    void setInputEnabled(bool enabled);
    bool isInputEnabled()const;
    double levelL()const;
    void applyInputFlowLph(double Lph);
    void applyOutputFlowLph(double Lph);

    QJsonObject toJson()const;
    void fromJson(const QJsonObject&obj);

signals:
    void levelChanged(double newLevelL);
    void becameFull();
    void becameEmpty();
    void canWithdrawChanged(bool canWithdraw);

private slots:
    void onTick();

private:
    QProgressBar*m_bar;
    QTimer*m_timer;

    double m_capacityL;
    double m_levelL;
    double m_inputFlowLph;
    double m_outputFlowLph;
    double m_inputMaxLph;
    double m_outputMaxLph;
    bool m_inputEnabled;
    bool m_canWithdraw;
    void updateBar();
};

class MainWindow:public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget*parent=nullptr);
    ~MainWindow()override;

protected:
    void resizeEvent(QResizeEvent* event)override;

private slots:
    void Distribucion(int outputDialValue);
    void onSaveState();
    void onLoadState();
    void onApplyCapacities();
    void onApplyFlows();

private:
    Ui::MainWindow *ui;

    ControlTanque* TanquePrincipal;
    ControlTanque* TanqueAuxiliar1;
    ControlTanque* TanqueAuxiliar2;

    double mapDialToLph(int dialValue) const;
    void setupConnections();
    void applyDistributionFromDial(int dialValue);
    void updateAuxiliaryInputs(double totalOutLph);
    void syncUiFromState();
};

#endif // MAINWINDOW_H
