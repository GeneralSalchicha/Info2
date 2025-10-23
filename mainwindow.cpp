#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtMath>
#include <QDebug>
#include <QResizeEvent>
#include <QLayout>
#include <QScrollArea>
#include <QSplitter>

static double readLphFromLineEdit(QLineEdit* e,double fallback=0.0){
    if(!e) return fallback;
    bool ok=false;
    double v=e->text().toDouble(&ok);
    return ok ? qMax(0.0,v) : fallback;
}

//uso de ControlTanque

ControlTanque::ControlTanque(QProgressBar* bar,QObject* parent)
    : QObject(parent),
    m_bar(bar),
    m_timer(new QTimer(this)),
    m_capacityL(100.0),
    m_levelL(0.0),
    m_inputFlowLph(0.0),
    m_outputFlowLph(0.0),
    m_inputMaxLph(100.0),
    m_outputMaxLph(100.0),
    m_inputEnabled(true),
    m_canWithdraw(false)
{
    m_timer->setInterval(200);
    connect(m_timer,&QTimer::timeout,this,&ControlTanque::onTick);
    m_timer->start();
    updateBar();
}

void ControlTanque::setCapacityL(double L){
    double newCap=qMax(1.0,L);
    if(m_levelL>newCap) m_levelL=newCap;
    m_capacityL=newCap;
    updateBar();
}

void ControlTanque::setInputMaxLph(double Lph){ m_inputMaxLph=qMax(0.0,Lph); }
void ControlTanque::setOutputMaxLph(double Lph){ m_outputMaxLph=qMax(0.0,Lph); }
double ControlTanque::getInputMaxLph()const{return m_inputMaxLph;}
double ControlTanque::getOutputMaxLph()const{return m_outputMaxLph;}
double ControlTanque::capacityL()const{return m_capacityL;}
double ControlTanque::currentInputLph()const{return m_inputFlowLph;}
double ControlTanque::currentOutputLph()const{return m_outputFlowLph;}

void ControlTanque::setInputEnabled(bool enabled){
    m_inputEnabled=enabled;
    if(!m_inputEnabled) m_inputFlowLph=0.0;
}

bool ControlTanque::isInputEnabled()const{return m_inputEnabled;}
double ControlTanque::levelL()const{return m_levelL;}

void ControlTanque::applyInputFlowLph(double Lph){
    if(!m_inputEnabled){ m_inputFlowLph=0.0; return; }
    if(m_levelL>=m_capacityL){ m_inputFlowLph=0.0; return; }
    m_inputFlowLph=qMin(Lph,m_inputMaxLph);
    if(m_levelL>=m_capacityL) m_inputFlowLph=0.0;
}

void ControlTanque::applyOutputFlowLph(double Lph){
    if(m_levelL<=0.1*m_capacityL){ m_outputFlowLph=0.0; return; }
    m_outputFlowLph=qMin(Lph,m_outputMaxLph);
}

void ControlTanque::onTick(){
    const double dt_s=m_timer->interval()/1000.0;
    double inL=m_inputFlowLph/3600.0*dt_s;
    double outL=m_outputFlowLph/3600.0*dt_s;

    double newLevel=m_levelL+inL-outL;
    double minAllowed=0.1*m_capacityL;

    if(outL>0.0&&newLevel<minAllowed){
        double maxRemovableL=m_levelL-minAllowed;
        if(maxRemovableL<=0.0){
            newLevel=m_levelL+inL;
            m_outputFlowLph=0.0;
        }else{
            double allowedOutLph=(maxRemovableL/dt_s)*3600.0;
            m_outputFlowLph=qMin(m_outputFlowLph,allowedOutLph);
            outL=m_outputFlowLph/3600.0*dt_s;
            newLevel=m_levelL+inL-outL;
            if(newLevel<minAllowed){ newLevel=minAllowed; m_outputFlowLph=0.0; }
        }
    }

    if(newLevel>=m_capacityL){
        m_levelL=m_capacityL;
        m_inputFlowLph=0.0;
        emit becameFull();
    }else if(newLevel<=0.0){
        m_levelL=0.0;
        m_outputFlowLph=0.0;
        emit becameEmpty();
    }else{
        m_levelL=newLevel;
    }

    bool newCanWithdraw=(m_levelL>0.1*m_capacityL);
    if(newCanWithdraw!=m_canWithdraw){
        m_canWithdraw=newCanWithdraw;
        if(!m_canWithdraw) m_outputFlowLph=0.0;
        emit canWithdrawChanged(m_canWithdraw);
    }

    updateBar();
    emit levelChanged(m_levelL);
}

void ControlTanque::updateBar(){
    if(!m_bar) return;
    int pct=(m_capacityL>0.0)?int((m_levelL/m_capacityL)*100.0+0.5):0;
    pct=qBound(0,pct,100);
    m_bar->setValue(pct);
}

QJsonObject ControlTanque::toJson()const{
    QJsonObject obj;
    obj["capacityL"]=m_capacityL;
    obj["levelL"]=m_levelL;
    obj["inputMaxLph"]=m_inputMaxLph;
    obj["outputMaxLph"]=m_outputMaxLph;
    obj["inputEnabled"]=m_inputEnabled;
    return obj;
}

void ControlTanque::fromJson(const QJsonObject& obj){
    if(obj.contains("capacityL")) setCapacityL(obj["capacityL"].toDouble());
    if(obj.contains("levelL")) m_levelL=obj["levelL"].toDouble();
    if(obj.contains("inputMaxLph")) setInputMaxLph(obj["inputMaxLph"].toDouble());
    if(obj.contains("outputMaxLph")) setOutputMaxLph(obj["outputMaxLph"].toDouble());
    if(obj.contains("inputEnabled")) setInputEnabled(obj["inputEnabled"].toBool());
    updateBar();
}

// parte de Mainwindow

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    TanquePrincipal(nullptr),
    TanqueAuxiliar1(nullptr),
    TanqueAuxiliar2(nullptr)
{
    ui->setupUi(this);
    if(ui->centralwidget) ui->centralwidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    if(ui->spinBox_CapPrincipal){ ui->spinBox_CapPrincipal->setRange(1,10000000); ui->spinBox_CapPrincipal->setSingleStep(10); }
    if(ui->spinBox_CapAux1){ ui->spinBox_CapAux1->setRange(1,10000000); ui->spinBox_CapAux1->setSingleStep(10); }
    if(ui->spinBox_CapAux2){ ui->spinBox_CapAux2->setRange(1,10000000); ui->spinBox_CapAux2->setSingleStep(10); }

    const auto widgets=this->findChildren<QWidget*>();
    for(QWidget* w:widgets){
        if(!w->objectName().isEmpty()){ w->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding); w->setMinimumSize(16,16); }
    }

    TanquePrincipal=new ControlTanque(ui->Principal,this);
    TanqueAuxiliar1=new ControlTanque(ui->Auxiliar1,this);
    TanqueAuxiliar2=new ControlTanque(ui->Auxiliar2,this);

    TanquePrincipal->setCapacityL(1000.0);
    TanqueAuxiliar1->setCapacityL(200.0);
    TanqueAuxiliar2->setCapacityL(200.0);

    TanquePrincipal->setInputMaxLph(500.0);
    TanquePrincipal->setOutputMaxLph(500.0);

    TanqueAuxiliar1->setInputMaxLph(300.0);
    TanqueAuxiliar2->setInputMaxLph(300.0);

    setupConnections();

    if(ui->checkBox) TanqueAuxiliar1->setInputEnabled(ui->checkBox->isChecked());
    if(ui->checkBox_2) TanqueAuxiliar2->setInputEnabled(ui->checkBox_2->isChecked());

    syncUiFromState();

    if(ui->Salida) applyDistributionFromDial(ui->Salida->value());
}

MainWindow::~MainWindow(){ delete ui; }

void MainWindow::resizeEvent(QResizeEvent* event){
    QMainWindow::resizeEvent(event);
    QWidget* cw=centralWidget();
    if(!cw) return;
    QLayout* L=cw->layout();
    if(!L) return;
    L->invalidate();
    L->activate();
    cw->updateGeometry();
    cw->update();
}

double MainWindow::mapDialToLph(int dialValue)const{
    if(!TanquePrincipal) return 0.0;
    double maxLph=TanquePrincipal->getOutputMaxLph();
    if(maxLph<=0.0) maxLph=TanquePrincipal->capacityL()*2.0;
    return (dialValue/100.0)*maxLph;
}

// Distribucion para auxiliares
void MainWindow::updateAuxiliaryInputs(double totalOutLph){
    if(!TanquePrincipal||!TanqueAuxiliar1||!TanqueAuxiliar2) return;

    if(TanquePrincipal->levelL()<=0.1*TanquePrincipal->capacityL()){
        TanquePrincipal->applyOutputFlowLph(0.0);
        if(ui->Salida) ui->Salida->setValue(0);
        TanqueAuxiliar1->applyInputFlowLph(0.0);
        TanqueAuxiliar2->applyInputFlowLph(0.0);
        return;
    }

    bool a1can=TanqueAuxiliar1->isInputEnabled()&&(TanqueAuxiliar1->levelL()<TanqueAuxiliar1->capacityL());
    bool a2can=TanqueAuxiliar2->isInputEnabled()&&(TanqueAuxiliar2->levelL()<TanqueAuxiliar2->capacityL());

    if(!a1can&&!a2can){
        TanquePrincipal->applyOutputFlowLph(0.0);
        if(ui->Salida) ui->Salida->setValue(0);
        TanqueAuxiliar1->applyInputFlowLph(0.0);
        TanqueAuxiliar2->applyInputFlowLph(0.0);
        return;
    }

    double space1=a1can?(TanqueAuxiliar1->capacityL()-TanqueAuxiliar1->levelL()):0.0;
    double space2=a2can?(TanqueAuxiliar2->capacityL()-TanqueAuxiliar2->levelL()):0.0;

    if(space1<=1e-9&&a2can){
        TanqueAuxiliar1->applyInputFlowLph(0.0);
        double take=qMin(totalOutLph,TanqueAuxiliar2->getInputMaxLph());
        TanqueAuxiliar2->applyInputFlowLph(take);
        TanquePrincipal->applyOutputFlowLph(take);
        return;
    }
    if(space2<=1e-9&&a1can){
        TanqueAuxiliar2->applyInputFlowLph(0.0);
        double take=qMin(totalOutLph,TanqueAuxiliar1->getInputMaxLph());
        TanqueAuxiliar1->applyInputFlowLph(take);
        TanquePrincipal->applyOutputFlowLph(take);
        return;
    }

    double sumSpace=qMax(1.0,space1+space2);
    double desired1=totalOutLph*(space1/sumSpace);
    double desired2=totalOutLph*(space2/sumSpace);

    TanqueAuxiliar1->applyInputFlowLph(desired1);
    TanqueAuxiliar2->applyInputFlowLph(desired2);

    double realIn1=TanqueAuxiliar1->currentInputLph();
    double realIn2=TanqueAuxiliar2->currentInputLph();
    double totalApplied=realIn1+realIn2;

    TanquePrincipal->applyOutputFlowLph(totalApplied);

    if(TanquePrincipal->levelL()>=TanquePrincipal->capacityL()-1e-6){
        TanquePrincipal->setInputEnabled(false);
        if(ui->Entrada) ui->Entrada->setValue(0);
    }

    if(TanquePrincipal->levelL()<=0.1*TanquePrincipal->capacityL()){
        TanquePrincipal->applyOutputFlowLph(0.0);
        if(ui->Salida) ui->Salida->setValue(0);
    }
}

void MainWindow::setupConnections(){
    if(ui->Salida){
        connect(ui->Salida,&QDial::valueChanged,this,[this](int v){
            double valLph=mapDialToLph(v);
            if(ui->label_OutRate) ui->label_OutRate->setText(QString::number(valLph,'f',0)+" L/h ->");
            if(TanquePrincipal) TanquePrincipal->applyOutputFlowLph(valLph);
            double actualOut=TanquePrincipal?TanquePrincipal->currentOutputLph():valLph;
            updateAuxiliaryInputs(actualOut);
        });
    }

    if(ui->dial_Aux1Out){
        connect(ui->dial_Aux1Out,&QDial::valueChanged,this,[this](int v){
            if(!TanqueAuxiliar1) return;
            double out=(v/100.0)*TanqueAuxiliar1->getOutputMaxLph();
            TanqueAuxiliar1->applyOutputFlowLph(out);
        });
    }
    if(ui->dial_Aux2Out){
        connect(ui->dial_Aux2Out,&QDial::valueChanged,this,[this](int v){
            if(!TanqueAuxiliar2) return;
            double out=(v/100.0)*TanqueAuxiliar2->getOutputMaxLph();
            TanqueAuxiliar2->applyOutputFlowLph(out);
        });
    }

    if(ui->checkBox){
        connect(ui->checkBox,&QCheckBox::toggled,this,[this](bool checked){
            TanqueAuxiliar1->setInputEnabled(checked);
            applyDistributionFromDial(ui->Salida?ui->Salida->value():0);
        });
    }
    if(ui->checkBox_2){
        connect(ui->checkBox_2,&QCheckBox::toggled,this,[this](bool checked){
            TanqueAuxiliar2->setInputEnabled(checked);
            applyDistributionFromDial(ui->Salida?ui->Salida->value():0);
        });
    }

    if(TanquePrincipal&&ui->label_PrincipalLevel){
        connect(TanquePrincipal,&ControlTanque::levelChanged,this,[this](double L){
            ui->label_PrincipalLevel->setText(QString::number(L,'f',1)+" L");
        });
    }
    if(TanqueAuxiliar1&&ui->label_Aux1Level){
        connect(TanqueAuxiliar1,&ControlTanque::levelChanged,this,[this](double L){
            ui->label_Aux1Level->setText(QString::number(L,'f',1)+" L");
        });
    }
    if(TanqueAuxiliar2&&ui->label_Aux2Level){
        connect(TanqueAuxiliar2,&ControlTanque::levelChanged,this,[this](double L){
            ui->label_Aux2Level->setText(QString::number(L,'f',1)+" L");
        });
    }

    if(ui->pushButtonLoadState) connect(ui->pushButtonLoadState,&QPushButton::clicked,this,&MainWindow::onLoadState);
    if(ui->pushButtonSaveState) connect(ui->pushButtonSaveState,&QPushButton::clicked,this,&MainWindow::onSaveState);

    if(ui->pushButton_ApplyFlows) connect(ui->pushButton_ApplyFlows,&QPushButton::clicked,this,&MainWindow::onApplyFlows);
    if(ui->pushButton_ApplyCaps) connect(ui->pushButton_ApplyCaps,&QPushButton::clicked,this,&MainWindow::onApplyCapacities);

    if(ui->Entrada){
        connect(ui->Entrada,&QDial::valueChanged,this,[this](int v){
            if(TanquePrincipal&&TanquePrincipal->levelL()<TanquePrincipal->capacityL()-1e-6) TanquePrincipal->setInputEnabled(true);
            double inRequested=(v/100.0)*TanquePrincipal->getInputMaxLph();
            TanquePrincipal->applyInputFlowLph(inRequested);
            if(ui->label_InRate) ui->label_InRate->setText(QString::number(inRequested,'f',0)+" L/h <-");
        });
    }

    if(TanquePrincipal){
        connect(TanquePrincipal,&ControlTanque::becameFull,this,[this](){
            if(ui->Entrada) ui->Entrada->setValue(0);
            TanquePrincipal->setInputEnabled(false);
        });
        connect(TanquePrincipal,&ControlTanque::canWithdrawChanged,this,[this](bool canWithdraw){
            if(!canWithdraw) if(ui->Salida) ui->Salida->setValue(0);
        });
    }

    if(TanqueAuxiliar1){
        connect(TanqueAuxiliar1,&ControlTanque::becameFull,this,[this](){
            if(ui->dial_Aux1Out) ui->dial_Aux1Out->setValue(0);
            applyDistributionFromDial(ui->Salida?ui->Salida->value():0);
        });
    }
    if(TanqueAuxiliar2){
        connect(TanqueAuxiliar2,&ControlTanque::becameFull,this,[this](){
            if(ui->dial_Aux2Out) ui->dial_Aux2Out->setValue(0);
            applyDistributionFromDial(ui->Salida?ui->Salida->value():0);
        });
    }
}

void MainWindow::Distribucion(int outputDialValue){
    applyDistributionFromDial(outputDialValue);
}

void MainWindow::applyDistributionFromDial(int dialValue){
    if(!TanquePrincipal) return;

    double requestedOutLph=mapDialToLph(dialValue);
    TanquePrincipal->applyOutputFlowLph(requestedOutLph);

    double actualOutLph=TanquePrincipal->currentOutputLph();
    updateAuxiliaryInputs(actualOutLph);

    if(ui->Entrada){
        if(TanquePrincipal->capacityL()-TanquePrincipal->levelL()>1e-6) TanquePrincipal->setInputEnabled(true);
        double inRequested=(ui->Entrada->value()/100.0)*TanquePrincipal->getInputMaxLph();
        TanquePrincipal->applyInputFlowLph(inRequested);
        if(ui->label_InRate) ui->label_InRate->setText(QString::number(inRequested,'f',0)+" L/h <-");
    }

    if(TanqueAuxiliar1&&ui->dial_Aux1Out&&TanqueAuxiliar1->levelL()>=TanqueAuxiliar1->capacityL()-1e-6) ui->dial_Aux1Out->setValue(0);
    if(TanqueAuxiliar2&&ui->dial_Aux2Out&&TanqueAuxiliar2->levelL()>=TanqueAuxiliar2->capacityL()-1e-6) ui->dial_Aux2Out->setValue(0);

    if(TanqueAuxiliar1&&TanqueAuxiliar2&&
        TanqueAuxiliar1->levelL()>=TanqueAuxiliar1->capacityL()-1e-6&&
        TanqueAuxiliar2->levelL()>=TanqueAuxiliar2->capacityL()-1e-6){
        if(ui->Salida) ui->Salida->setValue(0);
        TanquePrincipal->applyOutputFlowLph(0.0);
    }
}

void MainWindow::onSaveState(){
    QJsonObject root;
    root["principal"]=TanquePrincipal->toJson();
    root["aux1"]=TanqueAuxiliar1->toJson();
    root["aux2"]=TanqueAuxiliar2->toJson();

    QFile f("sim_state.json");
    if(f.open(QFile::WriteOnly)){
        QJsonDocument doc(root);
        f.write(doc.toJson());
        f.close();
    }
}

void MainWindow::onLoadState(){
    QFile f("sim_state.json");
    if(!f.open(QFile::ReadOnly)) return;
    QJsonDocument doc=QJsonDocument::fromJson(f.readAll());
    f.close();
    if(!doc.isObject()) return;
    QJsonObject root=doc.object();
    if(root.contains("principal")) TanquePrincipal->fromJson(root["principal"].toObject());
    if(root.contains("aux1")) TanqueAuxiliar1->fromJson(root["aux1"].toObject());
    if(root.contains("aux2")) TanqueAuxiliar2->fromJson(root["aux2"].toObject());
    syncUiFromState();
    applyDistributionFromDial(ui->Salida?ui->Salida->value():0);
}

void MainWindow::onApplyCapacities(){
    if(ui->spinBox_CapPrincipal){ double v=ui->spinBox_CapPrincipal->value(); TanquePrincipal->setCapacityL(v); }
    if(ui->spinBox_CapAux1){ double v=ui->spinBox_CapAux1->value(); TanqueAuxiliar1->setCapacityL(v); }
    if(ui->spinBox_CapAux2){ double v=ui->spinBox_CapAux2->value(); TanqueAuxiliar2->setCapacityL(v); }
    syncUiFromState();
    applyDistributionFromDial(ui->Salida?ui->Salida->value():0);
}

void MainWindow::onApplyFlows(){
    if(ui->lineEdit_CisternaInLph){
        double v=readLphFromLineEdit(ui->lineEdit_CisternaInLph,TanquePrincipal->getInputMaxLph());
        TanquePrincipal->setInputMaxLph(v);
    }
    if(ui->lineEdit_CisternaOutLph){
        double v=readLphFromLineEdit(ui->lineEdit_CisternaOutLph,TanquePrincipal->getOutputMaxLph());
        TanquePrincipal->setOutputMaxLph(v);
        TanqueAuxiliar1->setInputMaxLph(v);
        TanqueAuxiliar2->setInputMaxLph(v);
    }
    if(ui->lineEdit_Aux1OutLph){
        double v=readLphFromLineEdit(ui->lineEdit_Aux1OutLph,TanqueAuxiliar1->getOutputMaxLph());
        TanqueAuxiliar1->setOutputMaxLph(v);
    }
    if(ui->lineEdit_Aux2OutLph){
        double v=readLphFromLineEdit(ui->lineEdit_Aux2OutLph,TanqueAuxiliar2->getOutputMaxLph());
        TanqueAuxiliar2->setOutputMaxLph(v);
    }
    syncUiFromState();
    if(ui->Salida&&TanquePrincipal){
        double requestedMainOut=mapDialToLph(ui->Salida->value());
        TanquePrincipal->applyOutputFlowLph(requestedMainOut);
    }else if(TanquePrincipal) TanquePrincipal->applyOutputFlowLph(TanquePrincipal->getOutputMaxLph());

    if(ui->dial_Aux1Out&&TanqueAuxiliar1){
        double aux1OutRequested=(ui->dial_Aux1Out->value()/100.0)*TanqueAuxiliar1->getOutputMaxLph();
        TanqueAuxiliar1->applyOutputFlowLph(aux1OutRequested);
    }
    if(ui->dial_Aux2Out&&TanqueAuxiliar2){
        double aux2OutRequested=(ui->dial_Aux2Out->value()/100.0)*TanqueAuxiliar2->getOutputMaxLph();
        TanqueAuxiliar2->applyOutputFlowLph(aux2OutRequested);
    }

    if(ui->Salida) applyDistributionFromDial(ui->Salida->value());
    else{ double manualOut=TanquePrincipal->currentOutputLph(); updateAuxiliaryInputs(manualOut); }
}

void MainWindow::syncUiFromState(){
    if(ui->spinBox_CapPrincipal) ui->spinBox_CapPrincipal->setValue((int)TanquePrincipal->capacityL());
    if(ui->spinBox_CapAux1) ui->spinBox_CapAux1->setValue((int)TanqueAuxiliar1->capacityL());
    if(ui->spinBox_CapAux2) ui->spinBox_CapAux2->setValue((int)TanqueAuxiliar2->capacityL());

    if(ui->lineEdit_CisternaOutLph) ui->lineEdit_CisternaOutLph->setText(QString::number(TanquePrincipal->getOutputMaxLph()));
    if(ui->lineEdit_CisternaInLph) ui->lineEdit_CisternaInLph->setText(QString::number(TanquePrincipal->getInputMaxLph()));
    if(ui->lineEdit_Aux1OutLph) ui->lineEdit_Aux1OutLph->setText(QString::number(TanqueAuxiliar1->getOutputMaxLph()));
    if(ui->lineEdit_Aux2OutLph) ui->lineEdit_Aux2OutLph->setText(QString::number(TanqueAuxiliar2->getOutputMaxLph()));
}
