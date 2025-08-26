#include "mycombobox.h"

MyComboBox::MyComboBox(QWidget *parent) : QComboBox(parent)
{
    //
}

void MyComboBox::setTextAlignment(Qt::Alignment alignment) const
{
    auto* model = dynamic_cast<QStandardItemModel*>(this->view()->model());
    if (Q_NULLPTR == model)
    {
        return;
    }

    for (int index = 0, size = model->rowCount(); index < size; ++index)
    {
        if (Q_NULLPTR != model->item(index))
        {
            model->item(index)->setTextAlignment(alignment);
        }
    }
}


void MyComboBox::paintEvent(QPaintEvent* e)
{
    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));

    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    painter.drawComplexControl(QStyle::CC_ComboBox, opt);

    if(opt.editable)
    {
        painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
        return;
    }

    QRect editRect = this->style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, this);

    QStyleOptionButton buttonOpt;
    buttonOpt.initFrom(this);
    buttonOpt.rect = editRect;
    buttonOpt.text = opt.currentText;
    buttonOpt.icon = opt.currentIcon;
    buttonOpt.iconSize = opt.iconSize;

#if 1
    painter.drawControl(QStyle::CE_PushButtonLabel, buttonOpt);
#else
    painter.end();
    QPainter pt(this);
    this->style()->drawControl(QStyle::CE_PushButtonLabel, &buttonOpt, &pt, this);
#endif

    (void)*e;
}

