#include "acrylic.h"
#include "ui_acrylic.h"
#include <Windows.h>

#include <QPainter>
#include <QMouseEvent>
#include <QBitmap>
#include <QColorDialog>
#include <QLabel>

Acrylic::Acrylic(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Acrylic),
      _width(345),
      _height(500)
{
    ui->setupUi(this);
    ui->verticalLayout->setAlignment(Qt::AlignTop);
    this->resize(_width, _height);

    /****************************************************************************/
    /********************Things to do with the Acrylic Window********************/
    /********************         Do     Not   Touch         ********************/
    /****************************************************************************/

    //Configurations of Acrylic Effect
    acryBackground = QColor(240, 240, 240, 150);
    acryOpacity = 50;
    hwnd = HWND(winId());
    huser = GetModuleHandle(L"user32.dll");

    //Areo Effect
    if(huser){
        setWindowCompositionAttribute = (pfnSetWindowCompositionAttribute)GetProcAddress(huser, "SetWindowCompositionAttribute");
        if(setWindowCompositionAttribute){
            //DWORD gradientColor = DWORD(0x50FFFFFF);
            ACCENT_POLICY accent = { ACCENT_ENABLE_BLURBEHIND, 0, 0, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data;
            data.Attrib = WCA_ACCENT_POLICY;
            data.pvData = &accent;
            data.cbData = sizeof(accent);
            setWindowCompositionAttribute(hwnd, &data);
        }
    }

    //Acrylic Effect
    //Warning: Due to the API proplem, this effect is laggy when dragging | resizing

    //if(huser){
    //    setWindowCompositionAttribute = (pfnSetWindowCompositionAttribute)GetProcAddress(huser, "SetWindowCompositionAttribute");
    //    if(setWindowCompositionAttribute){
    //        DWORD gradientColor = DWORD(0x50F5F5F5);
    //        ACCENT_POLICY accent = { ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, gradientColor, 0 };
    //        WINDOWCOMPOSITIONATTRIBDATA data;
    //        data.Attrib = WCA_ACCENT_POLICY;
    //        data.pvData = &accent;
    //        data.cbData = sizeof(accent);
    //        setWindowCompositionAttribute(hwnd, &data);
    //    }
    //}

    //Configurations of Drag resizing
    this->setMouseTracking(true);

    //Some old time trash
    //QWidget* titleBar = new QWidget(this);
    //QLabel* text = new QLabel("Calculator", titleBar);
    //text->setGeometry(this->rect().x() + 10, this->rect().y(), 500, 50);
    //QFont font;
    //font.setPointSize(20);
    //font.setFamily("FuturaNo2DDemBol");
    //text->setFont(font);

    //ui->mainLayout->setAlignment(Qt::AlignTop);

    ui->opEqual->setColor(QColor(75, 156, 222, 200));
    ui->opEqual->setHoverColor(QColor(50, 156, 222, 210));
    ui->opEqual->setClickedColor(QColor(0, 120, 215, 200));

    tabPage = new AdditionTabWidget(this);
    tabPage->setMinimumWidth(200);
    tabPage->setMaximumWidth(350);
    ui->mainHLayout->addWidget(tabPage);
    tabPage->hide();

    connect(ui->maximumWindow, SIGNAL(clicked()), this, SLOT(windowMaximum()));
    //connect(ui->maximumWindow, SIGNAL(clicked()), this, SLOT(windowMinimum()));

    /****************************************************************************/

    /****************************************************************************/
    /********************  Things to do with the Calculator  ********************/
    /****************************************************************************/

    initOpMap();
    //ui->displayCur->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    //ui->displayCur->setAlignment(Qt::AlignBottom);
    ui->displayCur->setTextFormat(Qt::RichText);
    ui->displayCur->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->displayHis->setTextFormat(Qt::RichText);
    ui->displayHis->setAttribute(Qt::WA_TransparentForMouseEvents);

    //Connect the buttons with input
    connect(ui->num0, &QPushButton::clicked, [=](){expr.insert('0'); RfrInput(); UpdStack();});
    connect(ui->num1, &QPushButton::clicked, [=](){expr.insert('1'); RfrInput(); UpdStack();});
    connect(ui->num2, &QPushButton::clicked, [=](){expr.insert('2'); RfrInput(); UpdStack();});
    connect(ui->num3, &QPushButton::clicked, [=](){expr.insert('3'); RfrInput(); UpdStack();});
    connect(ui->num4, &QPushButton::clicked, [=](){expr.insert('4'); RfrInput(); UpdStack();});
    connect(ui->num5, &QPushButton::clicked, [=](){expr.insert('5'); RfrInput(); UpdStack();});
    connect(ui->num6, &QPushButton::clicked, [=](){expr.insert('6'); RfrInput(); UpdStack();});
    connect(ui->num7, &QPushButton::clicked, [=](){expr.insert('7'); RfrInput(); UpdStack();});
    connect(ui->num8, &QPushButton::clicked, [=](){expr.insert('8'); RfrInput(); UpdStack();});
    connect(ui->num9, &QPushButton::clicked, [=](){expr.insert('9'); RfrInput(); UpdStack();});
    connect(ui->opDot, &QPushButton::clicked, [=](){expr.insert('.'); RfrInput(); UpdStack();});
    connect(ui->opAdd, &QPushButton::clicked, [=](){expr.insert('+'); RfrInput(); UpdStack();});
    connect(ui->opSub, &QPushButton::clicked, [=](){expr.insert('-'); RfrInput(); UpdStack();});
    connect(ui->opMulti, &QPushButton::clicked, [=](){expr.insert('*'); RfrInput(); UpdStack();});
    connect(ui->opDiv, &QPushButton::clicked, [=](){expr.insert('/'); RfrInput(); UpdStack();});
    connect(ui->opSqr, &QPushButton::clicked, [=](){expr.insert('^'); RfrInput(); UpdStack(); expr.insert('2'); RfrInput(); UpdStack(); });
    connect(ui->opPow, &QPushButton::clicked, [=](){expr.insert('^'); RfrInput(); UpdStack();});
    connect(ui->opMod, &QPushButton::clicked, [=](){expr.insert('%'); RfrInput(); UpdStack();});
    connect(ui->opBrckL, &QPushButton::clicked, [=](){expr.insert('('); RfrInput(); UpdStack();});
    connect(ui->opBrckR, &QPushButton::clicked, [=](){expr.insert(')'); RfrInput(); UpdStack();});
    connect(ui->opEqual, &QPushButton::clicked, [=](){expr.insert('#');  UpdStack(); RfrInput(); RfrResult(); UpdHistory();});
    connect(ui->opClear, &QPushButton::clicked, [=](){expr.clr(); RfrInput(); UpdStack(true);});
    connect(ui->bckSpace, &QPushButton::clicked, [=](){expr.backSpace(); RfrInput(); UpdStack(true);});

    connect(this, SIGNAL(addHistoryTerm(const QString &, const QString &, const QString &)), tabPage, SLOT(Controller_AddHistoryTerm(const QString &, const QString &, const QString &)));
    connect(this, SIGNAL(updateStackPage(const QString &, const QString &)), tabPage, SLOT(Controller_UpdateStackPage(const QString &, const QString &)));


}

Acrylic::~Acrylic()
{
    delete ui;
}

void Acrylic::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen;
    pen.setColor(QColor(0,0,0,255));
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setBrush(acryBackground);
    painter.drawRect(this->rect());
    ui->TitleBar->resize(this->rect().width() - 10, ui->TitleBar->height());
}

void Acrylic::mouseMoveEvent(QMouseEvent *event){
    if(pressed){
        if(mouseState == 0){
            QPoint displacement = event->globalPos() - mousePos_strt;
            this->move(displacement);
        }
        else if(mouseState == 1){
            //resize width
            int displacementX = event->pos().x() - this->rect().width();
            if(this->rect().width() + displacementX >= _width){
                this->resize(this->rect().width() + displacementX, this->rect().height());
            }
        }
        else if(mouseState == 2){
            //resize height
            int displacementY = event->pos().y() - this->rect().height();
            if(this->rect().height() + displacementY >= _height)
                this->resize(this->rect().width(), this->rect().height() + displacementY);
        }
        else{
            int displacementX = event->pos().x() - this->rect().width();
            if(this->rect().width() + displacementX >= _width)
                this->resize(this->rect().width() + displacementX, this->rect().height());
            int displacementY = event->pos().y() - this->rect().height();
            if(this->rect().height() + displacementY >= _height)
                this->resize(this->rect().width(), this->rect().height() + displacementY);
        }
        if(!tabShown && this->rect().width() >= 595){
            tabPage->show();
            tabShown = true;
        }
        if(tabShown && this->rect().width() < 595){
            tabPage->hide();
            tabShown = false;
        }
    }
    else{
        int _x = abs(event->pos().x()- this->rect().width());
        int _y = abs(event->pos().y() - this->rect().height());
        //qDebug() << _x << "," << _y;
        if(_x < 10 && _y < 10){
            this->setCursor(Qt::SizeFDiagCursor);
            mouseState = 3;
        }
        else if(_x < 5 && _y > 5 && event->pos().y() > ui->TitleBar->height()){
            this->setCursor(Qt::SizeHorCursor);
            mouseState = 1;
        }
        else if(_x > 5 && _y < 5){
            this->setCursor(Qt::SizeVerCursor);
            mouseState = 2;
        }
        else{
            this->setCursor(Qt::ArrowCursor);
            mouseState = 0;
        }
    }
}

void Acrylic::mousePressEvent(QMouseEvent *event){
    mousePos_strt = event->pos();
    pressed = true;
}

void Acrylic::mouseReleaseEvent(QMouseEvent *event){
    pressed = false;
    this->setCursor(Qt::ArrowCursor);
    mouseState = 0;
}

void Acrylic::RfrInput(){
    ui->displayHis->setText("");
    if(expr.expToHtml().isEmpty()){
        ui->displayCur->setText("0");
        ui->displayHis->clear();
    }
    else
        ui->displayCur->setText(expr.expToHtml().replace('/', "÷").replace('*', "×"));
}

void Acrylic::RfrResult(){
    ui->displayHis->setText(expr.expToHtml().replace('/', "÷").replace('*', "×"));
    ui->displayCur->setText(expr.resToHtml());
}

void Acrylic::UpdStack(bool r){
    QString numDif;
    if(r){
        numDif.append('r');
        numDif.append(expr.numStackToStr());
    }
    else{
        numDif = expr.numDifStr();
    }
    QString opDif;
    if(r){
        opDif.append('r');
        opDif.append(expr.opStackToStr());
    }
    else{
        opDif = expr.opDifStr();
    }
    //qDebug() << opDif << "\n";
    emit updateStackPage(numDif, opDif);
    update();
}

void Acrylic::UpdHistory(){
    //qDebug() << expr.resToHtml() << "\n";
    emit addHistoryTerm(expr.expStr(), expr.expToHtml(), expr.resToHtml());
    if(expr.prepare())
        UpdStack(true);
    update();
}


void Acrylic::windowMaximum(){
    if(!isMaximum){
        lastWidth = this->width();
        lastHeight = this->height();
        this->showFullScreen();
        ui->maximumWindow->setText("▽");
        isMaximum = true;
    }
    else{
        this->showNormal();
        this->resize(lastWidth, lastHeight);
        ui->maximumWindow->setText("□");
        isMaximum = false;
    }
}